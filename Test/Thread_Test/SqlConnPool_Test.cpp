#include <gtest/gtest.h>
#include "glog/logging.h"
#include "Pool/sqlconnRALL.hpp"

int sqlPort = 3306;
const char *sqlUser = "root";
const char *sqlPwd = "123456";
// dbName, connPoolNum
const char *dbName = "yourdb";
int connPoolNum = 12;

TEST(SqlConnPool_Test, test_basic) {

    SqlConnPool::Instance()->Init("localhost", sqlPort, sqlUser, sqlPwd, dbName, connPoolNum);
    MYSQL **sqlconn;
    SqlConnRAII sCR(sqlconn, SqlConnPool::Instance());
}
