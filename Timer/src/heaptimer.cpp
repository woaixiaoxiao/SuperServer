#include "Timer/heaptimer.hpp"

// 当前节点上移，父节点是(i-1)/2
void HeapTimer::siftup_(size_t i) {
    assert(i >= 0 && i < heap_.size());
    size_t j = (i - 1) / 2;
    while (j >= 0) {
        if (heap_[j] < heap_[i]) {
            break;
        }
        SwapNode_(i, j);
        i = j;
        j = (i - 1) / 2;
    }
}

// 交换节点，并记录堆的id->数组的下标
void HeapTimer::SwapNode_(size_t i, size_t j) {
    assert(i >= 0 && i < heap_.size());
    assert(j >= 0 && j < heap_.size());
    std::swap(heap_[i], heap_[j]);
    ref_[heap_[i].id] = i;
    ref_[heap_[j].id] = j;
}

// 节点下移，左孩子是2*i+1
bool HeapTimer::siftdown_(size_t index, size_t n) {
    assert(index >= 0 && index < heap_.size());
    assert(n >= 0 && n <= heap_.size());
    size_t i = index;
    size_t j = i * 2 + 1;
    while (j < n) {
        if (j + 1 < n && heap_[j + 1] < heap_[j])
            j++;
        if (heap_[i] < heap_[j])
            break;
        SwapNode_(i, j);
        i = j;
        j = i * 2 + 1;
    }
    return i > index;
}

// 添加定时器node
void HeapTimer::add(int id, int timeout, const TimeoutCallBack &cb) {
    assert(id >= 0);
    size_t i;
    if (ref_.count(id) == 0) {
        // 新节点，插入尾端，然后调整
        i = heap_.size();
        ref_[id] = i;
        heap_.push_back({id, Clock::now() + MS(timeout), cb});
        siftup_(i);
    } else {
        // 老节点，修改值，然后尝试上移或者下移
        i = ref_[id];
        heap_[i].expires = Clock::now() + MS(timeout);
        heap_[i].cb = cb;
        if (!siftdown_(i, heap_.size())) {
            siftup_(i);
        }
    }
}

// 执行id对应的定时器函数
void HeapTimer::doWork(int id) {
    // 如果没有这个id
    if (heap_.empty() || ref_.count(id) == 0) {
        return;
    }
    // 执行函数并删除节点
    size_t i = ref_[id];
    TimerNode node = heap_[i];
    node.cb();
    del_(i);
}

// 删除index位置的节点
void HeapTimer::del_(size_t index) {
    // 判断节点是否合法
    assert(!heap_.empty() && index >= 0 && index < heap_.size());
    // 将当前节点a和末尾节点b交换位置，调整b的位置，然后将a节点从末尾pop掉
    size_t i = index;
    size_t n = heap_.size() - 1;
    assert(i <= n);
    if (i < n) {
        SwapNode_(i, n);
        if (!siftdown_(i, n)) {
            siftup_(i);
        }
    }
    ref_.erase(heap_.back().id);
    heap_.pop_back();
}

// 调整节点的超时时间
void HeapTimer::adjust(int id, int timeout) {
    assert(!heap_.empty() && ref_.count(id) > 0);
    heap_[ref_[id]].expires = Clock::now() + MS(timeout);
    ;
    siftdown_(ref_[id], heap_.size());
}

// 尝试执行超时事务
void HeapTimer::tick() {
    if (heap_.empty()) {
        return;
    }
    while (!heap_.empty()) {
        TimerNode node = heap_.front();
        // 检查定时器是否到期
        if (std::chrono::duration_cast<MS>(node.expires - Clock::now()).count() > 0) {
            break;
        }
        node.cb();
        pop();
    }
}

// 弹出一个已完成的定时器节点
void HeapTimer::pop() {
    assert(!heap_.empty());
    del_(0);
}

void HeapTimer::clear() {
    ref_.clear();
    heap_.clear();
}

// 执行一个定时器事务，并且判断下一次需要执行tick需要等待多久
int HeapTimer::GetNextTick() {
    tick();
    size_t res = -1;
    if (!heap_.empty()) {
        res = std::chrono::duration_cast<MS>(heap_.front().expires - Clock::now()).count();
        if (res < 0) {
            res = 0;
        }
    }
    return res;
}