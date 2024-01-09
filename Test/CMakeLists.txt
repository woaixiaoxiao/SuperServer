# 查找 GTest 库
find_package(GTest REQUIRED)
# GTest 的头文件
include_directories(${GTEST_INCLUDE_DIRS})
# Glog的lib
find_package(glog REQUIRED)
# 获得该项目的各种模块，包括thread，memory等，以及glog
set(MY_LIBRARIES
    Pool
    Buffer
    Log
    Http
    Timer
    glog::glog
    # Serverlib
)
# 获取所有的需要测试的cpp文件
file(GLOB_RECURSE BUSTUB_TEST_SOURCES "${PROJECT_SOURCE_DIR}/Test/*/*Test.cpp")

foreach (bustub_test_source ${BUSTUB_TEST_SOURCES})
    # Create a human readable name.
    # 取出文件名字
    get_filename_component(bustub_test_filename ${bustub_test_source} NAME)
    # 将模板文件名字去掉.cpp
    string(REPLACE ".cpp" "" bustub_test_name ${bustub_test_filename})
    # 构建目标
    # add_executable(${bustub_test_name} EXCLUDE_FROM_ALL ${bustub_test_source})
    add_executable(${bustub_test_name} ${bustub_test_source})
    # 加入需要的库
    target_link_libraries(${bustub_test_name}  PUBLIC ${GTEST_BOTH_LIBRARIES} pthread ${MY_LIBRARIES})
    # 加入测试
    gtest_discover_tests(${bustub_test_name})
endforeach ()
