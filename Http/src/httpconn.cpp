#include "Http/httpconn.hpp"
using namespace std;

const char *HttpConn::srcDir;
std::atomic<int> HttpConn::userCount;
bool HttpConn::isET;

HttpConn::HttpConn() : kv(new KvStore()), request_(kv) {
    fd_ = -1;
    addr_ = {0};
    isClose_ = true;
};

HttpConn::~HttpConn() { Close(); };

void HttpConn::init(int fd, const sockaddr_in &addr) {
    assert(fd > 0);
    userCount++;
    addr_ = addr;
    fd_ = fd;
    writeBuff_.RetrieveAll();
    readBuff_.RetrieveAll();
    isClose_ = false;
    LOG_INFO("Client[%d](%s:%d) in, userCount:%d", fd_, GetIP(), GetPort(), (int)userCount);
}

// 关闭Http连接
void HttpConn::Close() {
    // 关闭unmap
    response_.UnmapFile();
    if (isClose_ == false) {
        isClose_ = true;
        userCount--;
        close(fd_);
        LOG_INFO("Client[%d](%s:%d) quit, UserCount:%d", fd_, GetIP(), GetPort(), (int)userCount);
    }
}

int HttpConn::GetFd() const { return fd_; };

struct sockaddr_in HttpConn::GetAddr() const {
    return addr_;
}

const char *HttpConn::GetIP() const { return inet_ntoa(addr_.sin_addr); }

int HttpConn::GetPort() const { return addr_.sin_port; }

// 从fd中读取数据到readBuff_
ssize_t HttpConn::read(int *saveErrno) {
    ssize_t len = -1;
    do {
        len = readBuff_.ReadFd(fd_, saveErrno);
        if (len <= 0) {
            break;
        }
    } while (isET); // 边缘触发，所以要一直读
    readBuff_.show();
    return len;
}

// 写数据到fd中
ssize_t HttpConn::write(int *saveErrno) {
    ssize_t len = -1;
    do {
        len = writev(fd_, iov_, iovCnt_);
        if (len <= 0) {
            *saveErrno = errno;
            break;
        }
        // 传输结束
        if (iov_[0].iov_len + iov_[1].iov_len == 0) {
            break;
        }
        // 使用到了第二块iov缓冲区
        else if (static_cast<size_t>(len) > iov_[0].iov_len) {
            iov_[1].iov_base = (uint8_t *)iov_[1].iov_base + (len - iov_[0].iov_len);
            iov_[1].iov_len -= (len - iov_[0].iov_len);
            if (iov_[0].iov_len) {
                writeBuff_.RetrieveAll();
                iov_[0].iov_len = 0;
            }
        }
        // 只用到了第一块缓冲区
        else {
            iov_[0].iov_base = (uint8_t *)iov_[0].iov_base + len;
            iov_[0].iov_len -= len;
            writeBuff_.Retrieve(len);
        }
    } while (isET || ToWriteBytes() > 10240);
    return len;
}

// 调用了process后，writeBuff_就已经准备好了，再调用write就可以将http回复发送出去
bool HttpConn::process() {
    // 将http请求从readBuff_中读出并解析，并初始化response
    request_.Init();
    if (readBuff_.ReadableBytes() <= 0) {
        return false;
    } else if (request_.parse(readBuff_)) {
        LOG_DEBUG("%s", request_.path().c_str());
        response_.Init(srcDir, request_.path(), request_.IsKeepAlive(), 200);
    } else {
        response_.Init(srcDir, request_.path(), false, 400);
    }
    // 根据已经初始化的response_将http回复写到writeBuff_中（这个函数因为kv被改了）
    response_.MakeResponse(writeBuff_);
    // 以下两行kv相关
    writeBuff_.Append("Content-length: " + to_string(request_.value.size()) + "\r\n\r\n");
    writeBuff_.Append(request_.value);
    writeBuff_.Append("\n");

    cout << "value:  " << request_.value << endl;
    // Buffer中只存了报文头部
    iov_[0].iov_base = const_cast<char *>(writeBuff_.Peek());
    iov_[0].iov_len = writeBuff_.ReadableBytes();
    iovCnt_ = 1;
    // 为了避免文件过大，用了mmap和iov机制（截胡，这里先修改为kv，如果要取消kv存储，则将下面这个if注释取消）
    // if (response_.FileLen() > 0 && response_.File()) {
    //     iov_[1].iov_base = response_.File();
    //     iov_[1].iov_len = response_.FileLen();
    //     iovCnt_ = 2;
    // }
    LOG_DEBUG("filesize:%d, %d  to %d", response_.FileLen(), iovCnt_, ToWriteBytes());
    return true;
}
