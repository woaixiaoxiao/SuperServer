#include <gtest/gtest.h>
#include "glog/logging.h"
#include "Pool/threadpool.hpp"
// #include <Thread/test1.hpp>

// 测试基本功能，四种函数类型
TEST(ThreadPool_Test, test_basic) {

    ThreadPool myThreadPool(8);
    myThreadPool.AddTask([] { std::cout << "Task 1" << std::endl; });

    // 插入Lambda表达式
    int x = 42;
    myThreadPool.AddTask([x] { std::cout << "Task 2 with captured variable: " << x << std::endl; });

    // 插入函数对象
    struct MyFunctor {
        void operator()() {
            std::cout << "Task 3 from functor" << std::endl;
        }
    };
    myThreadPool.AddTask(MyFunctor());

    // 插入函数指针
    void (*functionPointer)() = [] { std::cout << "Task 4 from function pointer" << std::endl; };
    myThreadPool.AddTask(functionPointer);
}
