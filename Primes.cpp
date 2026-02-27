//Libraries
#include <thread>
#include <queue>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <iostream>
#include "ThreadPool.h"

using namespace std;

inline int sign(int v)
{
    if(v != 0) {
        return v / abs(v);
    }
    return 0;
}

inline int sign(double v)
{
    if(v != 0.0) {
        return int(v / abs(v));
    }
    return 0;
}

inline int sign(float v)
{
    if(v != 0.0F) {
        return int(v / abs(v));
    }
    return 0;
}

inline vector<int> find_primes(int target = 100000)
{
    //Initialize variables
    int i;
    int n = 5;
    bool done = false;
    bool skip;
    bool synced;
    bool proceed;
    bool is_prime;
    double root_n;
    int primes_queued;

    //Initialize class variables
    mutex mtx;
    ThreadPool pool;
    condition_variable cv_;
    vector<int> primes = {2,3,5};

    thread timer([&done, &cv_, &mtx] {
        auto start = chrono::high_resolution_clock::now();
        while (true) {
            if(done) {
                cout << "\r" << string(32, ' ') << "\r";
                cout.flush();
                chrono::duration<double> duration = chrono::high_resolution_clock::now() - start;
                if(int(duration.count()) % 60 < 10) {
                    cout << "Complete! (" << floor(duration.count() / 60) << ":0" << int(duration.count()) % 60 << ")\n";
                } else {
                    cout << "Complete! (" << floor(duration.count() / 60) << ":" << int(duration.count()) % 60 << ")\n";
                }
                break;
            }
            this_thread::sleep_for(chrono::milliseconds(100));
            chrono::duration<double> duration = chrono::high_resolution_clock::now() - start;
            ostream& out = cout << "\rRunning... {";
            for(int i = 0; i < int(duration.count()) % 13; i++) {
                out << "▊";
            }
            for(int i = 0; i < 12 - (int(duration.count()) % 13); i++) {
                out << " ";
            }
            if(int(duration.count()) % 60 < 10) {
                out << "} (" << floor(duration.count() / 60) << ":0" << int(duration.count()) % 60 << ")";
                cout.flush();
            } else {
                out << "} (" << floor(duration.count() / 60) << ":" << int(duration.count()) % 60 << ")";
                cout.flush();
            }
        }
    });

    while (primes.size() < target) {
        //Set variable values 
        n += 2;
        i = 0;
        skip = false;
        synced = false;
        proceed = false;
        is_prime = true;
        root_n = sqrt(n);
        primes_queued = 0;

        //Check if n is divisible by common primes(Optimiztion)
        if(n % 3 == 0 || n % 5 == 0) {
            continue;
        }

        while (true) {
            //Create a reference variable for prime #(i+1)
            auto& prime = primes[i];

            //Check if it as been determined to not be a prime
            //Check if it's root is a prime
            //Only check primes less than √n
            if(!is_prime) {
                break;
            } else if(prime == root_n) {
                is_prime = false;
                break;
            } else if(!(prime < root_n)) {
                break;
            }

            if(!skip) {
                //Update counters
                i++;
                primes_queued++;

                //Create a new task in the thread pool
                pool.enqueue([&prime, &synced, &primes_queued, &is_prime, &proceed, &mtx, &cv_, &n] {
                    {
                        {
                            //Makes sure the main thread is waiting
                            unique_lock<mutex> lock(mtx);
                            cv_.wait(lock,[&synced] {
                                return synced;
                            });
                            //Checks if n is divisible by a prime
                            if(n % prime == 0) {
                                is_prime = false;
                            }
                            primes_queued--;
                            if(primes_queued == 0) {
                                proceed = true;
                            }
                        }
                        //Notify main thread that this_thread has finished
                        cv_.notify_one();
                    }
                });
            }
        }

        //Memory lock to ensure the main thread waits for the thread pool to finish
        {
            unique_lock<mutex> lock(mtx);
            synced = true;
            //Notify threads in thread pool to begin
            cv_.notify_all();
            //Wait for threads in thread pool to finish
            cv_.wait(lock, [&proceed] {
                return proceed;
            });
        }

        if(is_prime) {
            //Add new prime to list of primes
            primes.emplace_back(n);
            //cout << n << " " << primes.size() << endl;
        }
    }
    
    done = true;
    timer.join();

    return primes;
}

int main() {
    int target;
    cout << "Input target prime: ";
    cin >> target;
    vector<int> primes = find_primes(target);
    cout << primes[target - 1] << endl;
    return 0;
}