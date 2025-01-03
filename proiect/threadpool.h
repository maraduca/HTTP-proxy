#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
public:
    ThreadPool(size_t threadCount); // Constructor
    ~ThreadPool();                 // Destructor

    void addTask(const std::function<void()> &task); // Adaugă un task

private:
    void workerThread(); // Funcția rulată de fiecare thread

    std::vector<std::thread> threads;            // Vector de thread-uri
    std::queue<std::function<void()>> tasks;     // Coada de task-uri
    std::mutex queueMutex;                       // Mutex pentru protejarea cozii
    std::condition_variable condition;           // Variabilă condițională
    bool stop;                                   // Flag pentru oprirea thread-urilor
};

#endif // THREADPOOL_H
