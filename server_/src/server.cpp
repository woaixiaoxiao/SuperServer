#include "server_/server.hpp"

void server_::test_main() {
    thread_::thread T;
    T.print();
}

int main() {
    server_::test_main();
    return 0;
}