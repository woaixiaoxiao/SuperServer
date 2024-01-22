#include <gtest/gtest.h>
#include "glog/logging.h"
#include "Buffer/buffer.hpp"
#include "SkipList/newskiplist.hpp"
using namespace std;

void test() {
    Buffer b;
    Node<std::string, std::string> node;
    SkipList<std::string, std::string> sl;
}

TEST(SkipList_Test, test_new) {
    SkipList<std::string, std::string> skipList(6);
    skipList.insert_element("1", "学");
    skipList.insert_element("3", "算法");
    skipList.insert_element("7", "认准");
    skipList.insert_element("8", "微信公众号：代码随想录");
    skipList.insert_element("9", "学习");
    skipList.insert_element("19", "算法不迷路");
    skipList.insert_element("19", "赶快关注吧你会发现详见很晚！");
    std::cout << "skipList size:" << skipList.size() << std::endl;
    skipList.display_list();
    skipList.delete_element("3");
    skipList.delete_element("7");
    std::cout << "skipList size:" << skipList.size() << std::endl;
    skipList.display_list();
    std::cout << skipList.search_element("19") << std::endl;
}