#include "Server/server.hpp"

void server_::test_main() {
    thread_::thread T;
    T.print();
}

int main() {
    server_::test_main();
    thread_::print2();
    return 0;
}