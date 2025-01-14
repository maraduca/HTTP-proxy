#include "threadpool.h"

ThreadPool::ThreadPool(size_t threadCount) : stop(false)
{
    for (size_t i = 0; i < threadCount; ++i)
    {
        threads.emplace_back([this] {   this->workerThread(); });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();

    for (std::thread &worker : threads)
    {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::addTask(const std::function<void()> &task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.push(task);
    }
    condition.notify_one();
}

void ThreadPool::workerThread() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            condition.wait(lock, [this] { return stop || !tasks.empty(); });

            if (stop && tasks.empty())
            {
                return;
            }

            task = tasks.front();

            tasks.pop();
        }

        task();
    }
}
