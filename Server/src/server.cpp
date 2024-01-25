#include "Server/server.hpp"
#include <libgen.h>

using namespace std;

WebServer::WebServer(int port, int trigMode, int timeoutMS, bool OptLinger, int sqlPort,
                     const char *sqlUser, const char *sqlPwd, const char *dbName, int connPoolNum,
                     int threadNum, bool openLog, int logLevel, int logQueSize)
    : port_(port), openLinger_(OptLinger), timeoutMS_(timeoutMS), isClose_(false),
      timer_(new HeapTimer()), threadpool_(new ThreadPool(threadNum)), epoller_(new Epoller()) {
    srcDir_ = getcwd(nullptr, 256);
    assert(srcDir_);
    // 获取上一级目录，当前目录是build目录
    srcDir_ = dirname(srcDir_);
    printf("%s\n", srcDir_);
    strncat(srcDir_, "/resources/", 16);
    HttpConn::userCount = 0;
    HttpConn::srcDir = srcDir_;
    SqlConnPool::Instance()->Init("localhost", sqlPort, sqlUser, sqlPwd, dbName, connPoolNum);
    // 初始化epoll相关
    InitEventMode_(trigMode);
    if (!InitSocket_()) {
        isClose_ = true;
    }
    // 初始化跳表kv存储
    kv = std::make_shared<KvStore>();
    // 日志设置
    if (openLog) {
        Log::Instance()->init(logLevel, "./log", ".log", logQueSize);
        if (isClose_) {
            LOG_ERROR("========== Server init error!==========");
        } else {
            LOG_INFO("========== Server init ==========");
            LOG_INFO("Port:%d, OpenLinger: %s", port_, OptLinger ? "true" : "false");
            LOG_INFO("Listen Mode: %s, OpenConn Mode: %s", (listenEvent_ & EPOLLET ? "ET" : "LT"),
                     (connEvent_ & EPOLLET ? "ET" : "LT"));
            LOG_INFO("LogSys level: %d", logLevel);
            LOG_INFO("srcDir: %s", HttpConn::srcDir);
            LOG_INFO("SqlConnPool num: %d, ThreadPool num: %d", connPoolNum, threadNum);
        }
    }
}

WebServer::~WebServer() {
    close(listenFd_);
    isClose_ = true;
    free(srcDir_);
    SqlConnPool::Instance()->ClosePool();
}
/*
测试监听的事件类型
EPOLLRDHUP监听对方是否关闭了fd
EPOLLONESHOT保证一个fd只会引起一件事，除非重新加入epoll
*/
void WebServer::InitEventMode_(int trigMode) {
    listenEvent_ = EPOLLRDHUP;
    connEvent_ = EPOLLONESHOT | EPOLLRDHUP;
    switch (trigMode) {
        case 0:
            break;
        case 1:
            connEvent_ |= EPOLLET;
            break;
        case 2:
            listenEvent_ |= EPOLLET;
            break;
        case 3:
            listenEvent_ |= EPOLLET;
            connEvent_ |= EPOLLET;
            break;
        default:
            listenEvent_ |= EPOLLET;
            connEvent_ |= EPOLLET;
            break;
    }
    HttpConn::isET = (connEvent_ & EPOLLET);
}

void WebServer::Start() {
    // 0不会阻塞，-1会阻塞，其他正数代表阻塞的时间
    int timeMS = -1;
    if (!isClose_) {
        LOG_INFO("========== Server start ==========");
    }
    while (!isClose_) {
        // 获取最近的一个定时器的时间
        if (timeoutMS_ > 0) {
            timeMS = timer_->GetNextTick();
        }
        // epoll_wait
        int eventCnt = epoller_->Wait(timeMS);
        for (int i = 0; i < eventCnt; i++) {
            // 处理事件
            int fd = epoller_->GetEventFd(i);
            uint32_t events = epoller_->GetEvents(i);
            // 新的连接
            if (fd == listenFd_) {
                DealListen_();
            }
            // 关闭连接
            else if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                assert(users_.count(fd) > 0);
                CloseConn_(&users_[fd]);
            }
            // 可读
            else if (events & EPOLLIN) {
                assert(users_.count(fd) > 0);
                DealRead_(&users_[fd]);
            }
            // 可写
            else if (events & EPOLLOUT) {
                assert(users_.count(fd) > 0);
                DealWrite_(&users_[fd]);
            } else {
                LOG_ERROR("Unexpected event");
            }
        }
    }
}

void WebServer::SendError_(int fd, const char *info) {
    assert(fd > 0);
    int ret = send(fd, info, strlen(info), 0);
    if (ret < 0) {
        LOG_WARN("send error to client[%d] error!", fd);
    }
    close(fd);
}

// 删除客户端连接
void WebServer::CloseConn_(HttpConn *client) {
    assert(client);
    LOG_INFO("Client[%d] quit!", client->GetFd());
    epoller_->DelFd(client->GetFd());
    client->Close();
}

// 添加新的客户端
void WebServer::AddClient_(int fd, sockaddr_in addr) {

    assert(fd > 0);
    users_[fd].init(fd, addr);
    // 一个客户端最长连接时间
    if (timeoutMS_ > 0) {
        timer_->add(fd, timeoutMS_, std::bind(&WebServer::CloseConn_, this, &users_[fd]));
    }
    epoller_->AddFd(fd, EPOLLIN | connEvent_);
    SetFdNonblock(fd);
    LOG_INFO("Client[%d] in!", users_[fd].GetFd());
}

// 处理新的客户端连接
void WebServer::DealListen_() {
    LOG(INFO) << "新的客户端连接";
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    // 如果是边缘触发，就要保证读干净了
    do {
        int fd = accept(listenFd_, (struct sockaddr *)&addr, &len);
        if (fd <= 0) {
            return;
        } else if (HttpConn::userCount >= MAX_FD) {
            SendError_(fd, "Server busy!");
            LOG_WARN("Clients is full!");
            return;
        }
        AddClient_(fd, addr);
    } while (listenEvent_ & EPOLLET);
}

// 处理可读事件，分发到线程池
void WebServer::DealRead_(HttpConn *client) {
    LOG(INFO) << "客户端有请求";
    assert(client);
    ExtentTime_(client);
    threadpool_->AddTask(std::bind(&WebServer::OnRead_, this, client));
}

// 处理可写事件，分发到线程池
void WebServer::DealWrite_(HttpConn *client) {
    LOG(INFO) << "发东西给客户端";
    assert(client);
    ExtentTime_(client);
    threadpool_->AddTask(std::bind(&WebServer::OnWrite_, this, client));
}

// 给客户端续命（超出一定时间没有发消息，就会删除这个客户端）
void WebServer::ExtentTime_(HttpConn *client) {
    assert(client);
    if (timeoutMS_ > 0) {
        timer_->adjust(client->GetFd(), timeoutMS_);
    }
}

// 工作线程从客户端的http连接中读取客户端发来的信息
void WebServer::OnRead_(HttpConn *client) {
    assert(client);
    int ret = -1;
    int readErrno = 0;
    ret = client->read(&readErrno);

    if (ret <= 0 && readErrno != EAGAIN) {
        CloseConn_(client);
        return;
    }
    OnProcess(client);
}

// 处理客户端的请求
void WebServer::OnProcess(HttpConn *client) {
    // 如果现在是在读事件，处理完之后就设置为监听写事件
    if (client->process()) {
        epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLOUT);
    } else {
        // 这个else代表是写事件调用的，写事件处理完了，就开始继续监听读事件
        epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLIN);
    }
}

// 处理客户端写事件
void WebServer::OnWrite_(HttpConn *client) {
    assert(client);
    int ret = -1;
    int writeErrno = 0;
    // 将给客户端的http回复写到fd中
    ret = client->write(&writeErrno);
    // 如果写完了，并且是长连接
    if (client->ToWriteBytes() == 0) {
        if (client->IsKeepAlive()) {
            OnProcess(client);
            return;
        }
    }
    // 如果写操作没成功，并且是因为内核缓冲区满了，那就再接再厉，继续监听可写事件
    else if (ret < 0) {
        if (writeErrno == EAGAIN) {
            epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLOUT);
            return;
        }
    }
    // 如果写成功了，并且是短连接，那处理完一次http事件后就关闭连接
    CloseConn_(client);
}

// 初始化listen的fd到epoll中
bool WebServer::InitSocket_() {
    int ret;
    struct sockaddr_in addr;
    if (port_ > 65535 || port_ < 1024) {
        LOG_ERROR("Port:%d error!", port_);
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port_);
    // 设置关闭fd时的模式
    struct linger optLinger = {0};
    if (openLinger_) {
        optLinger.l_onoff = 1;
        optLinger.l_linger = 1;
    }

    listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd_ < 0) {
        LOG_ERROR("Create socket error!", port_);
        return false;
    }

    ret = setsockopt(listenFd_, SOL_SOCKET, SO_LINGER, &optLinger, sizeof(optLinger));
    if (ret < 0) {
        close(listenFd_);
        LOG_ERROR("Init linger error!", port_);
        return false;
    }

    int optval = 1;
    // 避免重启时之前的socket还没有被销毁导致无法监听端口
    ret = setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));
    if (ret == -1) {
        LOG_ERROR("set socket setsockopt error !");
        close(listenFd_);
        return false;
    }

    ret = bind(listenFd_, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        LOG_ERROR("Bind Port:%d error!", port_);
        close(listenFd_);
        return false;
    }

    ret = listen(listenFd_, 6);
    if (ret < 0) {
        LOG_ERROR("Listen port:%d error!", port_);
        close(listenFd_);
        return false;
    }
    ret = epoller_->AddFd(listenFd_, listenEvent_ | EPOLLIN);
    if (ret == 0) {
        LOG_ERROR("Add listen error!");
        close(listenFd_);
        return false;
    }
    // 设置listenFd为非阻塞的，这样在没有数据时调用read函数也不会阻塞，而是返回一个错误值
    SetFdNonblock(listenFd_);
    LOG_INFO("Server port:%d", port_);
    return true;
}

// 设置fd为非阻塞
int WebServer::SetFdNonblock(int fd) {
    assert(fd > 0);
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}
