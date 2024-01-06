#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <thread>
#include "glog/logging.h"

//TODO ：加入无锁队列

class ThreadPool {
public:
    explicit ThreadPool(size_t threadCount = 8) : pool_(std::make_shared<Pool>()) {
        // 检查线程数量是否合法
        CHECK(threadCount > 0) << "threadCount is less than 0";
        // 使用lambda函数创建工作线程
        for (size_t i = 0; i < threadCount; i++) {
            std::thread([pool = pool_] {
                // 工作线程唯一的作用就是运行一个循环，不断地从task中取出待处理的函数
                std::unique_lock<std::mutex> uq_lock(pool->mtx);
                while (true) {
                    // 这里if循环的顺序很重要，就算是缓存池已经被关了，如果任务队列不空，也必须将所有任务处理完了再退出线程
                    if (!pool->tasks.empty()) {
                        // 任务队列有待处理的任务
                        // 在锁的保护下取出任务队列的第一个任务
                        auto task = pool->tasks.front();
                        pool->tasks.pop();
                        uq_lock.unlock();
                        task();
                        uq_lock.lock();
                    } else if (pool->isClosed) {
                        // 线程池已经关闭
                        break;
                    } else {
                        // 暂时无任务，通过条件变量阻塞
                        pool->cond.wait(uq_lock);
                    }
                }
            }).detach();
        }
    }

    ThreadPool() = default;
    ThreadPool(ThreadPool &&) = default;

    ~ThreadPool() {
        if (static_cast<bool>(pool_)) {
            {
                std::lock_guard<std::mutex> lg(pool_->mtx);
                pool_->isClosed = true;
            }
            pool_->cond.notify_all();
        }
    }

    template <class F>
    void AddTask(F &&task) {
        {
            std::lock_guard<std::mutex> lg(pool_->mtx);
            pool_->tasks.emplace(std::forward<F>(task));
        }
        pool_->cond.notify_one();
    }

private:
    // 缓存池对象，封装了缓存池的锁，条件变量，状态，任务队列
    struct Pool {
        std::mutex mtx;
        std::condition_variable cond;
        bool isClosed;
        std::queue<std::function<void()>> tasks;
    };
    std::shared_ptr<Pool> pool_;
};