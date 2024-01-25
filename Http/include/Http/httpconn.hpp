#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <sys/types.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <errno.h>

#include "Log/log.hpp"
#include "Pool/sqlconnRALL.hpp"
#include "Buffer/buffer.hpp"
#include "Http/httprequest.hpp"
#include "Http/httpresponse.hpp"
#include "SkipList/kvstore.hpp"

/*
使用逻辑
1. 调用init
2. 调用read从fd将http请求读到readBuff_
3. 调用process，从readBuff_解析，然后准备好了writeBuff_（可能还有mmap，iov机制）
4. 调用write，将http回复发送出去
*/

class HttpConn {
public:
    HttpConn();

    ~HttpConn();

    void init(int sockFd, const sockaddr_in &addr);

    ssize_t read(int *saveErrno);

    ssize_t write(int *saveErrno);

    void Close();

    int GetFd() const;

    int GetPort() const;

    const char *GetIP() const;

    sockaddr_in GetAddr() const;

    bool process();

    int ToWriteBytes() { return iov_[0].iov_len + iov_[1].iov_len; }

    bool IsKeepAlive() const { return request_.IsKeepAlive(); }

    static bool isET;
    static const char *srcDir;
    static std::atomic<int> userCount;
    std::shared_ptr<KvStore> kv;

private:
    int fd_;
    struct sockaddr_in addr_;

    bool isClose_;

    int iovCnt_;
    struct iovec iov_[2];

    Buffer readBuff_;  // 读缓冲区
    Buffer writeBuff_; // 写缓冲区

    HttpRequest request_;
    HttpResponse response_;
};

#endif //HTTP_CONN_H