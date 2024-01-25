#include <gtest/gtest.h>
#include "glog/logging.h"
#include "Http/httprequest.hpp"
#include "Http/httpconn.hpp"

TEST(Httprequest_Test, test_basic) {
    HttpConn hc;
    hc.kv->print();
    hc.kv->set("1", "2");
}
