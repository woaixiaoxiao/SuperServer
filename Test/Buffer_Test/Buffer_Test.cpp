#include <gtest/gtest.h>
#include "glog/logging.h"
#include "Buffer/buffer.hpp"
#include <fcntl.h> // For open function
#include <string>
#include "SkipList/newskiplist.hpp"

TEST(BufferPool_Test, test_buffer) {
    const char *filePath = "example.txt";
    int newFd = open(filePath, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    std::string str1 = "Hello World";
    Buffer buffer;
    buffer.RetrieveAll();
    CHECK(buffer.ReadableBytes() == 0);
    buffer.Append(str1);
    CHECK(buffer.ReadableBytes() == str1.size());
    int errn;
    size_t bytes = buffer.WriteFd(newFd, &errn);
    CHECK(bytes == str1.size());
    close(newFd);
}
