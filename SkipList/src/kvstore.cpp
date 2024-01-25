#include "SkipList/kvstore.hpp"

bool KvStore::set(std::string key, std::string value) {
    return skip_list.insert_element(key, value);
}
std::string KvStore::get(std::string key) { return skip_list.get_element(key); }
void KvStore::del(std::string key) { skip_list.delete_element(key); }