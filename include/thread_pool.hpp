#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
public:
    // create pool with specified number of worker threads (e.g. 4 or 8)
    ThreadPool(size_t num_threads);
    
    // graceful shutdown in destructor
    ~ThreadPool();

    // push a new task into the queue for any available worker thread
    void enqueue(std::function<void()> task);

private:
    // worker thread list
    std::vector<std::thread> workers;

    // shared task queue
    std::queue<std::function<void()>> tasks;

    // synchronization stuff
    std::mutex queue_mutex;            // protects access to the tasks queue
    std::condition_variable cv;        // notifies sleeping workers when new task arrives
    bool stop;                         // flag to tell worker threads to exit when shutting down

    // the function each worker thread runs in the background
    void worker_loop();
};

#endif // THREAD_POOL_HPP
