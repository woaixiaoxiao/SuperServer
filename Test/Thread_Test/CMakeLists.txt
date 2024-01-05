# 查找 GTest 库
find_package(GTest REQUIRED)
# GTest 的头文件
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test01 test01.cpp)

# 链接测试库
target_link_libraries( test01 PUBLIC
        ${GTEST_BOTH_LIBRARIES}
        pthread Thread )

# target_link_libraries(test01 PUBLIC Thread)

# 添加到测试
gtest_discover_tests(test01)
