#include <queue>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <iostream>
#include "ThreadPool.h"

class Prime101
{
public:
    Prime101() : n_threads(std::thread::hardware_concurrency()), pool(std::thread::hardware_concurrency()) {}

    Prime101(size_t num_threads) : n_threads(num_threads), pool(num_threads) {}

    std::vector<int> find_primes(int target_prime_)
    {
        std::thread timer([this] {
            auto start = std::chrono::high_resolution_clock::now();
            while (true) {
                if(done) {
                    std::cout << "\r" << std::string(32, ' ') << "\r";
                    std::cout.flush();
                    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;
                    if(int(duration.count()) % 60 < 10) {
                        std::cout << "Complete! (" << floor(duration.count() / 60) << ":0" << int(duration.count()) % 60 << ")\n";
                    } else {
                        std::cout << "Complete! (" << floor(duration.count() / 60) << ":" << int(duration.count()) % 60 << ")\n";
                    }
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;
                std::ostream& out = std::cout << "\rRunning... {";
                for(int i = 0; i < int(duration.count()) % 13; i++) {
                    out << "▊";
                }
                for(int i = 0; i < 12 - (int(duration.count()) % 13); i++) {
                    out << " ";
                }
                if(int(duration.count()) % 60 < 10) {
                    out << "} (" << floor(duration.count() / 60) << ":0" << int(duration.count()) % 60 << ")";
                    std::cout.flush();
                } else {
                    out << "} (" << floor(duration.count() / 60) << ":" << int(duration.count()) % 60 << ")";
                    std::cout.flush();
                }
            }
        });
        while (primes_.size() < target_prime_) {
            
            if(primes_.back() < sqrt(target_prime_)) {
                prime2 = primes_.back() * primes_.back();
            } else {
                prime2 = n + target_prime_;
            }
            
            primes_queued_ = 0;
            proceed = false;
            synced = false;

            while (n < prime2 && n_cache_.size() + primes_.size() < target_prime_) {
                n += 2;
                if(n % 3 == 0 || n % 5 ==0) {
                    continue;
                }

                n_cache_.emplace(n);
                primes_queued_++;

                pool.enqueue([this, target_prime_] {
                    bool is_prime = true;
                    int np;
                    {
                        std::unique_lock<std::mutex> lock(mute_);
                        cv_.wait(lock, [this] {
                            return synced;
                        });
                        np = std::move(n_cache_.front());
                        n_cache_.pop();
                        for(auto& prime : primes_) {;
                            if(prime == sqrt(np)) {
                                is_prime = false;
                                break;
                            } else if(!(prime < sqrt(np))) {
                                break;
                            } else if(np % prime == 0) {
                                is_prime = false;
                                break;
                            }
                        }
                        if(is_prime) {
                            prime_cache_.emplace(std::move(np));
                        }
                        primes_queued_--;
                        if(primes_queued_ == 0) {
                            proceed = true;
                            cv_.notify_one();
                        }
                    }
                });
                
            }
            {
                std::unique_lock<std::mutex> lock(mute_);
                synced = true;
                cv_.notify_all();
                cv_.wait(lock, [this] {
                    return proceed;
                });
                int i = 0;
                while(!prime_cache_.empty()) {
                    if(prime_cache_.front() > primes_.back()) {
                        primes_.emplace_back(prime_cache_.front());
                        prime_cache_.pop();
                    }else if(prime_cache_.front() < primes_[i]) {
                        primes_.emplace(primes_.begin() + i, std::move(prime_cache_.front()));
                        prime_cache_.pop();
                    }
                    i++;
                }
            }
        }
        done = true;
        timer.join();
        return primes_;
    }

private:
    int n = 5;
    int prime2;
    bool done = false;
    ThreadPool pool;
    std::mutex mute_;
    int n_threads;
    int primes_queued_ = 0;
    bool synced;
    bool proceed;
    std::queue<int> n_cache_;
    std::condition_variable cv_;
    std::queue<int> prime_cache_;
    std::vector<int> primes_ = {2,3,5};
};

int main() {
    int target;
    Prime101 prime101;
    std::cout << "Input target prime: ";
    std::cin >> target;
    std::vector<int> primes = prime101.find_primes(target);
    std::cout << primes[target - 1] << std::endl;
}