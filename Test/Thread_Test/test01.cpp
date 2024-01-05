#include <gtest/gtest.h>

#include <Thread/test1.hpp>

TEST(test_my_class, get_age) {
    thread_::thread T;
    T.print();
}

TEST(test_my_class, get_name) {
    thread_::print2();
}
