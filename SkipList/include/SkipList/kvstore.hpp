#ifndef KVSTORE
#define KVSTORE
#include "SkipList/newskiplist.hpp"

class KvStore {
public:
    KvStore() : skip_list(5) {}
    bool set(std::string, std::string);
    std::string get(std::string);
    void del(std::string);
    void print() { printf("hehe\n"); }

private:
    SkipList<std::string, std::string> skip_list;
};
#endif