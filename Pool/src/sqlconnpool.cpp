#include "Pool/sqlconnpool.hpp"
using namespace std;

SqlConnPool::SqlConnPool() {
    useCount_ = 0;
    freeCount_ = 0;
}

SqlConnPool *SqlConnPool::Instance() {
    static SqlConnPool connPool;
    return &connPool;
}
// 初始化若干个mysql连接
void SqlConnPool::Init(const char *host, int port, const char *user, const char *pwd,
                       const char *dbName, int connSize = 10) {
    assert(connSize > 0);

    for (int i = 0; i < connSize; i++) {
        MYSQL *sql = nullptr;
        sql = mysql_init(sql);
        if (!sql) {
            LOG_ERROR("MySql init error!");
            assert(sql);
        }
        sql = mysql_real_connect(sql, host, user, pwd, dbName, port, nullptr, 0);
        if (!sql) {
            LOG_ERROR("MySql Connect error!");
        }
        connQue_.push(sql);
    }
    MAX_CONN_ = connSize;
    sem_init(&semId_, 0, MAX_CONN_);
}

// 获取一个mysql连接
MYSQL *SqlConnPool::GetConn() {
    MYSQL *sql = nullptr;
    if (connQue_.empty()) {
        LOG_WARN("SqlConnPool busy!");
        return nullptr;
    }
    // 先通过信号量确定是否有
    sem_wait(&semId_);
    {
        // 信号量确定有，锁帮助不会发生竞态条件
        // 但是这里的代码在关闭连接池时，好像没有考虑到并发的问题，有可能是因为关闭连接池的时候所有线程都结束了，所以不会发生？
        // 加上判断为空的条件还是要好点
        lock_guard<mutex> locker(mtx_);
        if (connQue_.empty()) {
            return sql; // nullptr
        }
        sql = connQue_.front();
        connQue_.pop();
    }
    return sql;
}

// 归还mysql连接，加锁放入，然后将信号量加1
void SqlConnPool::FreeConn(MYSQL *sql) {
    assert(sql);
    lock_guard<mutex> locker(mtx_);
    connQue_.push(sql);
    sem_post(&semId_);
}

// 关闭连接池
void SqlConnPool::ClosePool() {
    lock_guard<mutex> locker(mtx_);
    while (!connQue_.empty()) {
        auto item = connQue_.front();
        connQue_.pop();
        mysql_close(item);
    }
    mysql_library_end();
}

int SqlConnPool::GetFreeConnCount() {
    lock_guard<mutex> locker(mtx_);
    return connQue_.size();
}

SqlConnPool::~SqlConnPool() {
    ClosePool();
}
