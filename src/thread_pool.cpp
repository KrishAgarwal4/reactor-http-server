#include "thread_pool.hpp"
#include <iostream>

ThreadPool::ThreadPool(size_t num_threads) : stop(false) {
    // spawn our worker threads
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back(&ThreadPool::worker_loop, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        // lock the queue before flipping the stop flag
        // this guarantees no worker is in the middle of checking the flag without holding the lock
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }

    // wake up all sleeping workers so they see stop == true and exit their loop
    cv.notify_all();

    // wait for all worker threads to finish before destroying the pool
    for (std::thread &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        // acquire lock to safely push to the shared queue
        // using std::unique_lock (or lock_guard) so it automatically unlocks when leaving this scope
        std::unique_lock<std::mutex> lock(queue_mutex);

        // dont allow enqueing if the pool is stopping
        if (stop) {
            std::cerr << "warning: tried to enqueue on stopped ThreadPool\n";
            return;
        }

        tasks.push(task);
    }

    // notify one waiting thread that there is a new task ready
    // we do this outside the lock so the woken thread doesnt immediately block trying to grab the mutex
    cv.notify_one();
}

void ThreadPool::worker_loop() {
    while (true) {
        std::function<void()> task;

        {
            // unique_lock is required here because condition_variable::wait needs to unlock/relock the mutex
            std::unique_lock<std::mutex> lock(queue_mutex);

            // cv.wait will:
            // 1. release the lock and sleep the thread until notified
            // 2. re-acquire the lock when woken up and check the predicate (stop || !tasks.empty())
            // 3. protects against spurious wakeups (when OS wakes thread for no reason)
            cv.wait(lock, [this]() {
                return stop || !tasks.empty();
            });

            // if server is shutting down and no more tasks remain, exit the thread loop
            if (stop && tasks.empty()) {
                return;
            }

            // grab the next task from the queue
            task = std::move(tasks.front());
            tasks.pop();
        }

        // execute the task outside of the lock so other threads can access the queue concurrently!
        task();
    }
}
