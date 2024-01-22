#ifndef SkipList2_H
#define SkipList2_H
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>
#include <unistd.h>
#include "glog/logging.h"
#include <filesystem>
#include <sys/stat.h>
#include <sys/types.h>

#define STORE_FILE "store/dumpFile"
#define STORE_PRE "store"

std::mutex mtx2; // mutex for critical section
const std::string delimiter = ":";

//Class template to implement Node2
template <typename K, typename V>
class Node2 {

public:
    Node2() {
    }

    Node2(K k, V v, int);

    ~Node2();

    K get_key() const;

    V get_value() const;

    void set_value(V);

    // Linear array to hold pointers to next Node2 of different level
    Node2<K, V> **forward;

    int Node2_level;

private:
    K key;
    V value;
};

template <typename K, typename V>
Node2<K, V>::Node2(const K k, const V v, int level) {
    this->key = k;
    this->value = v;
    this->Node2_level = level;

    // level + 1, because array index is from 0 - level
    this->forward = new Node2<K, V> *[level + 1];

    // Fill forward array with 0(NULL)
    memset(this->forward, 0, sizeof(Node2<K, V> *) * (level + 1));
};

template <typename K, typename V>
Node2<K, V>::~Node2() {
    delete[] forward;
};

template <typename K, typename V>
K Node2<K, V>::get_key() const {
    return key;
};

template <typename K, typename V>
V Node2<K, V>::get_value() const {
    return value;
};

template <typename K, typename V>
void Node2<K, V>::set_value(V value) {
    this->value = value;
};

// Class template for Skip list
template <typename K, typename V>
class SkipList2 {

public:
    SkipList2(int);
    ~SkipList2();
    int get_random_level();
    Node2<K, V> *create_Node2(K, V, int);
    int insert_element(K, V);
    void display_list();
    bool search_element(K);
    void delete_element(K);
    void dump_file();
    void load_file();
    int size();

private:
    void get_key_value_from_string(const std::string &str, std::string *key, std::string *value);
    bool is_valid_string(const std::string &str);

private:
    // Maximum level of the skip list
    int _max_level;

    // current level of skip list
    int _skip_list_level;

    // pointer to header Node2
    Node2<K, V> *_header;

    // file operator
    std::ofstream _file_writer;
    std::ifstream _file_reader;

    // SkipList2 current element count
    int _element_count;
};

// construct skip list
template <typename K, typename V>
SkipList2<K, V>::SkipList2(int max_level) {

    this->_max_level = max_level;
    this->_skip_list_level = 0;
    this->_element_count = 0;

    // create header Node2 and initialize key and value to null
    K k;
    V v;
    this->_header = new Node2<K, V>(k, v, _max_level);
};

// create new Node2
template <typename K, typename V>
Node2<K, V> *SkipList2<K, V>::create_Node2(const K k, const V v, int level) {
    Node2<K, V> *n = new Node2<K, V>(k, v, level);
    return n;
}

// Insert given key and value in skip list
// return 1 means element exists
// return 0 means insert successfully
/* 
                           +------------+
                           |  insert 50 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |                      insert +----+
level 3         1+-------->10+---------------> | 50 |          70       100
                                               |    |
                                               |    |
level 2         1          10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 1         1    4     10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 0         1    4   9 10         30   40  | 50 |  60      70       100
                                               +----+

*/
template <typename K, typename V>
int SkipList2<K, V>::insert_element(const K key, const V value) {

    mtx2.lock();
    Node2<K, V> *current = this->_header;

    // create update array and initialize it
    // update is array which put Node2 that the Node2->forward[i] should be operated later
    // update[i]代表了第i层需要修改的节点，即需要将插入的元素放到这个节点后面
    Node2<K, V> *update[_max_level + 1];
    memset(update, 0, sizeof(Node2<K, V> *) * (_max_level + 1));

    // start form highest level of skip list
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] != NULL && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    // reached level 0 and forward pointer to right Node2, which is desired to insert key.
    // 指向第0层的待插入的位置
    current = current->forward[0];

    // if current Node2 have key equal to searched key, we get it
    // 如果已存在，则返回1
    if (current != NULL && current->get_key() == key) {
        std::cout << "key: " << key << ", exists" << std::endl;
        mtx2.unlock();
        return 1;
    }

    // if current is NULL that means we have reached to end of the level
    // if current's key is not equal to key that means we have to insert Node2 between update[0] and current Node2
    // 如果该位置不是当前的Key，则需要执行插入操作
    if (current == NULL || current->get_key() != key) {

        // Generate a random level for Node2
        int random_level = get_random_level();

        // If random level is greater thar skip list's current level, initialize update value with pointer to header
        // 如果索引级别高于现有的级别，将update数组的新增级别都指向header
        if (random_level > _skip_list_level) {
            for (int i = _skip_list_level + 1; i < random_level + 1; i++) {
                update[i] = _header;
            }
            _skip_list_level = random_level;
        }

        // create new Node2 with random level generated
        Node2<K, V> *inserted_Node2 = create_Node2(key, value, random_level);

        // insert Node2
        for (int i = 0; i <= random_level; i++) {
            inserted_Node2->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_Node2;
        }
        std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
        _element_count++;
    }
    mtx2.unlock();
    return 0;
}

// Display skip list
template <typename K, typename V>
void SkipList2<K, V>::display_list() {

    std::cout << "\n*****Skip List*****"
              << "\n";
    for (int i = 0; i <= _skip_list_level; i++) {
        Node2<K, V> *Node2 = this->_header->forward[i];
        std::cout << "Level " << i << ": ";
        while (Node2 != NULL) {
            std::cout << Node2->get_key() << ":" << Node2->get_value() << ";";
            Node2 = Node2->forward[i];
        }
        std::cout << std::endl;
    }
}

// Dump data in memory to file
template <typename K, typename V>
void SkipList2<K, V>::dump_file() {

    std::cout << "dump_file-----------------" << std::endl;
    // C++不能直接创建文件夹，因此需要先创建文件夹，再创建文件。其中文件时可以通过ofstream默认操作的
    if (!std::filesystem::exists(STORE_PRE)) {
        mkdir(STORE_PRE, S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
    }
    _file_writer.open(STORE_FILE);
    if (!_file_writer) {
        std::cout << "open unSuccessfully" << std::endl;
        std::cout << std::strerror(errno) << std::endl;
    }
    Node2<K, V> *Node2 = this->_header->forward[0];

    while (Node2 != NULL) {
        _file_writer << Node2->get_key() << ":" << Node2->get_value() << "\n";
        std::cout << Node2->get_key() << ":" << Node2->get_value() << ";\n";
        Node2 = Node2->forward[0];
    }

    _file_writer.flush();
    _file_writer.close();
    return;
}

// Load data from disk
template <typename K, typename V>
void SkipList2<K, V>::load_file() {

    _file_reader.open(STORE_FILE);
    if (_file_reader.is_open()) {
        std::cout << "load Successfully" << std::endl;
    }
    std::cout << "load_file-----------------" << std::endl;
    std::string line;
    std::string *key = new std::string();
    std::string *value = new std::string();
    while (getline(_file_reader, line)) {
        get_key_value_from_string(line, key, value);
        if (key->empty() || value->empty()) {
            continue;
        }
        insert_element(*key, *value);
        std::cout << "key:" << *key << "value:" << *value << std::endl;
    }
    _file_reader.close();
}

// Get current SkipList2 size
template <typename K, typename V>
int SkipList2<K, V>::size() {
    return _element_count;
}

template <typename K, typename V>
void SkipList2<K, V>::get_key_value_from_string(const std::string &str, std::string *key,
                                               std::string *value) {

    if (!is_valid_string(str)) {
        return;
    }
    *key = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter) + 1, str.length());
}

template <typename K, typename V>
bool SkipList2<K, V>::is_valid_string(const std::string &str) {

    if (str.empty()) {
        return false;
    }
    if (str.find(delimiter) == std::string::npos) {
        return false;
    }
    return true;
}

// Delete element from skip list
template <typename K, typename V>
void SkipList2<K, V>::delete_element(K key) {

    mtx2.lock();
    Node2<K, V> *current = this->_header;
    Node2<K, V> *update[_max_level + 1];
    memset(update, 0, sizeof(Node2<K, V> *) * (_max_level + 1));

    // start from highest level of skip list
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] != NULL && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];
    if (current != NULL && current->get_key() == key) {

        // start for lowest level and delete the current Node2 of each level
        for (int i = 0; i <= _skip_list_level; i++) {

            // if at level i, next Node2 is not target Node2, break the loop.
            if (update[i]->forward[i] != current)
                break;

            update[i]->forward[i] = current->forward[i];
        }

        // Remove levels which have no elements
        while (_skip_list_level > 0 && _header->forward[_skip_list_level] == 0) {
            _skip_list_level--;
        }

        std::cout << "Successfully deleted key " << key << std::endl;
        _element_count--;
    }
    mtx2.unlock();
    return;
}

// Search for element in skip list
/*
                           +------------+
                           |  select 60 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |
level 3         1+-------->10+------------------>50+           70       100
                                                   |
                                                   |
level 2         1          10         30         50|           70       100
                                                   |
                                                   |
level 1         1    4     10         30         50|           70       100
                                                   |
                                                   |
level 0         1    4   9 10         30   40    50+-->60      70       100
*/
template <typename K, typename V>
bool SkipList2<K, V>::search_element(K key) {

    std::cout << "search_element-----------------" << std::endl;
    Node2<K, V> *current = _header;

    // start from highest level of skip list
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
    }

    //reached level 0 and advance pointer to right Node2, which we search
    current = current->forward[0];

    // if current Node2 have key equal to searched key, we get it
    if (current and current->get_key() == key) {
        std::cout << "Found key: " << key << ", value: " << current->get_value() << std::endl;
        return true;
    }

    std::cout << "Not Found Key:" << key << std::endl;
    return false;
}

template <typename K, typename V>
SkipList2<K, V>::~SkipList2() {

    if (_file_writer.is_open()) {
        _file_writer.close();
    }
    if (_file_reader.is_open()) {
        _file_reader.close();
    }
    delete _header;
}

// 返回1的时候不建立索引，返回n的时候建立n-1级索引
template <typename K, typename V>
int SkipList2<K, V>::get_random_level() {

    int k = 1;
    while (rand() % 2) {
        k++;
    }
    k = (k < _max_level) ? k : _max_level;
    return k;
};
// vim: et tw=100 ts=4 sw=4 cc=120
#endif