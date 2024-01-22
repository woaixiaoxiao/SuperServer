#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <iostream>
#include <mutex>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>
#include <sys/types.h>

#define STORE_FILE "store/dumpFile"
#define STORE_PRE "store"

const std::string delimiter = ":";

template <typename K, typename V>
class Node {

public:
    Node() {}
    Node(K key, V value, size_t level) {
        this->key = key;
        this->value = value;
        this->node_level = level;
        this->forward.resize(level + 1);
    }
    ~Node(){};
    void set_value(V value) { this->value = value; }
    K get_key() { return key; }
    V get_value() { return value; }
    int node_level;
    std::vector<std::shared_ptr<Node<K, V>>> forward;

private:
    K key;
    V value;
};

template <typename K, typename V>
class SkipList {
public:
    SkipList(){};
    ~SkipList(){};
    SkipList(int);
    int get_random_level();
    std::shared_ptr<Node<K, V>> create_node(K, V, int);
    int insert_element(K, V);
    void display_list();
    bool search_element(K);
    void delete_element(K);
    void dump_file();
    void load_file();
    int size();

private:
    void get_key_value_from_string(const std::string &str, std::string &key, std::string &value);
    bool is_valid_string(const std::string &str);

private:
    // 跳表的最大高度
    int _max_level;

    // 跳表当前的高度
    int _skip_list_level;

    // 虚拟头节点，方便操作
    std::shared_ptr<Node<K, V>> _header;

    // file operator
    std::ofstream _file_writer;
    std::ifstream _file_reader;

    // 跳表的size
    int _element_count;
    // 锁
    std::mutex mtx;
};

template <typename K, typename V>
SkipList<K, V>::SkipList(int _max_level) {
    this->_max_level = _max_level;
    this->_skip_list_level = 0;
    this->_header = std::make_shared<Node<K, V>>(K(), V(), _max_level);
    this->_element_count = 0;
}

template <typename K, typename V>
std::shared_ptr<Node<K, V>> SkipList<K, V>::create_node(const K k, const V v, int level) {
    return std::make_shared<Node<K, V>>(k, v, level);
}

// 如果要控制索引比例为1/n，则条件为(rand() % n)==0
template <typename K, typename V>
int SkipList<K, V>::get_random_level() {
    int k = 1;
    while ((rand() % 2) == 0) {
        k++;
    }
    return (k > _max_level) ? _max_level : k;
}

template <typename K, typename V>
int SkipList<K, V>::size() {
    std::lock_guard<std::mutex> lgmtx(mtx);
    return _element_count;
}

template <typename K, typename V>
int SkipList<K, V>::insert_element(const K key, const V value) {

    std::lock_guard<std::mutex> lgmtx(mtx);
    std::shared_ptr<Node<K, V>> current = this->_header;

    // create update array and initialize it
    // update is array which put node that the node->forward[i] should be operated later
    // update[i]代表了第i层需要修改的节点，即需要将插入的元素放到这个节点后面
    std::shared_ptr<Node<K, V>> update[_max_level + 1];

    // start form highest level of skip list
    // here
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    // reached level 0 and forward pointer to right node, which is desired to insert key.
    // 指向第0层的待插入的位置
    current = current->forward[0];

    // if current node have key equal to searched key, we get it
    // 如果已存在，则返回1
    if (current && current->get_key() == key) {
        std::cout << "key: " << key << ", exists" << std::endl;
        return 1;
    }

    // if current is NULL that means we have reached to end of the level
    // if current's key is not equal to key that means we have to insert node between update[0] and current node
    // 如果该位置不是当前的Key，则需要执行插入操作
    if (!current || current->get_key() != key) {

        // Generate a random level for node
        int random_level = get_random_level();

        // If random level is greater thar skip list's current level, initialize update value with pointer to header
        // 如果索引级别高于现有的级别，将update数组的新增级别都指向header
        if (random_level > _skip_list_level) {
            for (int i = _skip_list_level + 1; i < random_level + 1; i++) {
                update[i] = _header;
            }
            _skip_list_level = random_level;
        }

        // create new node with random level generated
        std::shared_ptr<Node<K, V>> inserted_node = create_node(key, value, random_level);

        // insert node
        for (int i = 0; i <= random_level; i++) {
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
        std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
        _element_count++;
    }
    return 0;
}

template <typename K, typename V>
void SkipList<K, V>::display_list() {

    std::cout << "\n*****Skip List*****"
              << "\n";
    for (int i = 0; i <= _skip_list_level; i++) {
        std::shared_ptr<Node<K, V>> node = this->_header->forward[i];
        std::cout << "Level " << i << ": ";
        while (node != NULL) {
            std::cout << node->get_key() << ":" << node->get_value() << " | ";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

template <typename K, typename V>
void SkipList<K, V>::dump_file() {

    std::cout << "dump_file-----------------" << std::endl;
    // C++不能直接创建文件夹，因此需要先创建文件夹，再创建文件。其中文件时可以通过ofstream默认操作的
    if (!std::filesystem::exists(STORE_PRE)) {
        mkdir(STORE_PRE, S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
    }
    _file_writer.open(STORE_FILE);

    std::shared_ptr<Node<K, V>> Node2 = this->_header->forward[0];

    while (Node2 != NULL) {
        _file_writer << Node2->get_key() << ":" << Node2->get_value() << "\n";
        std::cout << Node2->get_key() << ":" << Node2->get_value() << ";\n";
        Node2 = Node2->forward[0];
    }

    _file_writer.flush();
    _file_writer.close();
    return;
}

template <typename K, typename V>
void SkipList<K, V>::load_file() {
    _file_reader.open(STORE_FILE);
    std::cout << "load_file-----------------" << std::endl;
    std::string line;
    std::string key;
    std::string value;
    while (getline(_file_reader, line)) {
        get_key_value_from_string(line, key, value);
        if (key.empty() || value.empty()) {
            continue;
        }
        insert_element(key, value);
        std::cout << "key:" << key << "value:" << value << std::endl;
    }
    _file_reader.close();
}

template <typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const std::string &str, std::string &key,
                                               std::string &value) {

    if (!is_valid_string(str)) {
        return;
    }
    key = str.substr(0, str.find(delimiter));
    // 若pos+size的值超出范围，则调整为字符串末尾就结束
    value = str.substr(str.find(delimiter) + 1, str.length());
}

template <typename K, typename V>
bool SkipList<K, V>::is_valid_string(const std::string &str) {

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
void SkipList<K, V>::delete_element(K key) {
    std::lock_guard<std::mutex> lgmtx(mtx);
    std::shared_ptr<Node<K, V>> current = this->_header;
    std::shared_ptr<Node<K, V>> update[_max_level + 1];

    // start from highest level of skip list
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] != NULL && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];
    if (current && current->get_key() == key) {

        // start for lowest level and delete the current Node2 of each level
        for (int i = 0; i <= _skip_list_level; i++) {

            // if at level i, next Node2 is not target Node2, break the loop.
            if (update[i]->forward[i] != current)
                break;

            update[i]->forward[i] = current->forward[i];
        }

        // Remove levels which have no elements
        while (_skip_list_level > 0 && _header->forward[_skip_list_level] == nullptr) {
            _skip_list_level--;
        }

        std::cout << "Successfully deleted key " << key << std::endl;
        _element_count--;
    }
    return;
}

// 跳表中查找，在这里，第0层和第1层都包含了所有元素，有点浪费
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
bool SkipList<K, V>::search_element(K key) {
    std::lock_guard<std::mutex> lgmtx(mtx);
    std::cout << "search_element-----------------" << std::endl;
    std::shared_ptr<Node<K, V>> current = _header;

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

#endif