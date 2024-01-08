#include <gtest/gtest.h>
#include "glog/logging.h"
#include "Log/log.hpp"

TEST(Log_Test, test_log) {
    Log::Instance()->init();
    LOG_INFO("hello world %d",520);
}
