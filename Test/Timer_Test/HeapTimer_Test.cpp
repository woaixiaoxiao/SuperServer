#include <gtest/gtest.h>
#include "glog/logging.h"
#include "Timer/heaptimer.hpp"
#include "glog/logging.h"

void test() {
    LOG(INFO) << "Hello World";
}

TEST(HeapTimer_Test, test_basic) {

    HeapTimer ht;
    ht.add(1, 2, std::bind(&test));
    sleep(3);
    ht.tick();
}
