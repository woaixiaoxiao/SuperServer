// #include <项目名/模块名.h>
// namespace 项目名 {
//     void 函数名() { 函数实现 }
// }
#include "Thread/test1.hpp"

namespace thread_ {

void print2() {
    std::cout << "hello world2" << std::endl;
}

void thread::print() {
    std::cout << "hello world" << std::endl;
}

} // namespace thread_