#ifndef THREADPOOL_H
#define THREADPOOL_H
// Libraries
#include <queue>
#include <mutex>
#include <functional>
#include <iostream>
#include <thread>
#include <condition_variable>

// Define ThreadPool class
class ThreadPool
{
    public:
        ThreadPool(size_t n_threads = std::thread::hardware_concurrency())
        {
            for(size_t i = 0; i < n_threads; i++) {
                threads_.emplace_back([this] {
                    while (true) {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(queue_mutex_);
                            cv_.wait(lock, [this] {
                                return !tasks_.empty() || stop_;
                            });
                            if(tasks_.empty() && stop_) {
                                return;
                            }
                            task = std::move(tasks_.front());
                            tasks_.pop();
                        }
                        task();
                    }
                });
            }
        }
        ~ThreadPool()
        {
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                stop_ = true;
            }
            cv_.notify_all();
            for(auto& thread : threads_) {
                thread.join();
            }
        }
        void enqueue(std::function<void()> task)
        {
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                tasks_.emplace(std::move(task));
            }
            cv_.notify_one();
        }
    private:
        // Define variables
        // Conditional variable for thread communication
        // Queue of tasks(no output[void])
        // Vector of threads
        // Mutex memory lock
        // Stop boolean
        std::condition_variable cv_;
        std::queue<std::function<void()>> tasks_;
        std::vector<std::thread> threads_;
        std::mutex queue_mutex_;
        bool stop_ = false;

};
#endif