#include "Buffer/buffer.hpp"

Buffer::Buffer(int initBuffSize) : buffer_(initBuffSize), readPos_(0), writePos_(0) {}

size_t Buffer::ReadableBytes() const { return writePos_ - readPos_; }
size_t Buffer::WritableBytes() const { return buffer_.size() - writePos_; }

size_t Buffer::PrependableBytes() const { return readPos_; }

// 指向未读取位置的指针，即通过char*+len得到指针
const char *Buffer::Peek() const { return BeginPtr_() + readPos_; }

// 标记读出len字节的数据
void Buffer::Retrieve(size_t len) {
    assert(len <= ReadableBytes());
    readPos_ += len;
}

// 一直读到指针到end为止
void Buffer::RetrieveUntil(const char *end) {
    assert(Peek() <= end);
    Retrieve(end - Peek());
}

// 设置标志位，代表将所有东西都取完
void Buffer::RetrieveAll() {
    bzero(&buffer_[0], buffer_.size());
    readPos_ = 0;
    writePos_ = 0;
}

// 取出所有可读的数据，并且将读写指针置0
std::string Buffer::RetrieveAllToStr() {
    std::string str(Peek(), ReadableBytes());
    RetrieveAll();
    return str;
}

// 获取写指针（常量指针）
const char *Buffer::BeginWriteConst() const { return BeginPtr_() + writePos_; }

// 获取写指针（普通指针）
char *Buffer::BeginWrite() { return BeginPtr_() + writePos_; }

// 更新写指针的下标
void Buffer::HasWritten(size_t len) { writePos_ += len; }

// 写入字符串到缓冲区（std::string形式）
void Buffer::Append(const std::string &str) { Append(str.data(), str.length()); }

// 写入字符串到缓冲区（void*形式）
void Buffer::Append(const void *data, size_t len) {
    assert(data);
    Append(static_cast<const char *>(data), len);
}

// 将char*真正地写入字符串，并且更新写指针
void Buffer::Append(const char *str, size_t len) {
    assert(str);
    EnsureWriteable(len);
    std::copy(str, str + len, BeginWrite());
    HasWritten(len);
}

// 讲一个Buffer类添加到当前Buffer
void Buffer::Append(const Buffer &buff) { Append(buff.Peek(), buff.ReadableBytes()); }

// 确保能写这么多个字节，不够的话就扩展空间
void Buffer::EnsureWriteable(size_t len) {
    if (WritableBytes() < len) {
        MakeSpace_(len);
    }
    assert(WritableBytes() >= len);
}

// 亮点设计
// 1. 保证了在可读数据量较大的情况下，也只调用一次readv，减小了系统调用的开销
// 2. 在只调用一次readv的情况下，也可以保证Buffer的vector不会太大，如果每个Buffer都很大，资源开销就大
// 之所以可以实现1,2，就是利用了分散读取，具体来说，是利用了局部空间栈做了一个暂时的缓冲
ssize_t Buffer::ReadFd(int fd, int *saveErrno) {
    char buff[65535];
    struct iovec iov[2];
    const size_t writable = WritableBytes();
    /* 分散读， 保证数据全部读完 */
    iov[0].iov_base = BeginPtr_() + writePos_;
    iov[0].iov_len = writable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const ssize_t len = readv(fd, iov, 2);
    if (len < 0) {
        *saveErrno = errno;
    } else if (static_cast<size_t>(len) <= writable) {
        writePos_ += len;
    } else {
        writePos_ = buffer_.size();
        Append(buff, len - writable);
    }
    return len;
}

void Buffer::show() {
    printf("--------------show begin=---------\n");
    for (int i = readPos_; i < writePos_; i++) {
        printf("%c", buffer_[i]);
    }
    printf("\n");
    printf("--------------show end=---------\n");
}

ssize_t Buffer::WriteFd(int fd, int *saveErrno) {
    size_t readSize = ReadableBytes();
    ssize_t len = write(fd, Peek(), readSize);
    if (len < 0) {
        *saveErrno = errno;
        return len;
    }
    readPos_ += len;
    return len;
}

// 返回缓冲区的起始位置
char *Buffer::BeginPtr_() { return &*buffer_.begin(); }

// 返回缓冲区的起始位置（常量版）
const char *Buffer::BeginPtr_() const { return &*buffer_.begin(); }

// 扩展空间，要么是调整读写指针，要么是直接给vector扩容
void Buffer::MakeSpace_(size_t len) {
    if (WritableBytes() + PrependableBytes() < len) {
        buffer_.resize(writePos_ + len + 1);
    } else {
        size_t readable = ReadableBytes();
        std::copy(BeginPtr_() + readPos_, BeginPtr_() + writePos_, BeginPtr_());
        readPos_ = 0;
        writePos_ = readPos_ + readable;
        assert(readable == ReadableBytes());
    }
}