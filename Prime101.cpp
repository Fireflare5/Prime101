#include <iostream>
#include <thread>

class Prime101
{
public:
    std::vector<long long> primes;
    Prime101(long long n) : Sieve("auto") {
        timer();
        primes = find_primes(n);
    }

    Prime101(std::string sieve, long long n) : Sieve(sieve) {
        timer();
        primes = find_primes(n);
    }

    Prime101(long long n, std::string sieve) : Sieve(sieve) {
        timer();
        primes = find_primes(n);
    }
    ~Prime101()
    {
        done = true;
        for(auto& time : clock) {
            time.join();
        }
    }
    std::vector<long long> find_primes(long long n) {
        if(Sieve == "eratosthenes" || Sieve == "sieve of eratosthenes" || Sieve == "soe" || Sieve == "SoE" || Sieve == "SOE" || Sieve == "eratos") {
            Soe(n);
        } else if(Sieve == "Atkin" || Sieve == "sieve of atkin" || Sieve == "soa" || Sieve == "SoA" || Sieve == "SOA" || Sieve == "atkin") {
            Soa(n);
        } else if(Sieve == "auto" || Sieve == "Auto") {
            if(n < 10000000000LL) {
                Soe(n);
            } else if(n > 10000000000LL) {
                Soa(n);
            }
        }
        for(long long i = 2; i <= n; ++i) {
                if(primes_[i]) {
                    primes.emplace_back(i);
                }
            }
        return primes;
    }

    void Soe(long long n) {
        primes_.assign(n + 1, 1);
        for(long long i = 2; i * i <= n; ++i) {
            if(primes_[i]) {
                for(long long j = i * i; j <= n; j += i) {
                    primes_[j] = 0;
                }
            }
        }
    }

    void Soa(long long n) {
        primes_.assign(n + 1, 0);
        if(n > 2) primes_[2] = 1;
        if(n > 3) primes_[3] = 1;
        for(long long i = 1; i * i <= n; ++i) {
            for(long long j = 1; j * j <= n; ++j) {
                long long k = 4 * i * i + j * j;
                if(k <= n && (k % 12 == 1 || k % 12 == 5)) {
                    primes_[k] = (primes_[k] + 1) % 2;
                }
                k = 3 * i * i + j * j;
                if(k <= n && k % 12 == 7) {
                    primes_[k] = (primes_[k] + 1) % 2;
                }
                k = 3 * i * i - j * j;
                if(i > j && k <= n && k % 12 == 11) {
                    primes_[k] = (primes_[k] + 1) % 2;
                }
            }
        }
        for(long long i = 5; i * i <= n; ++i) {
            if(!primes_[i]) continue;
            for(long long j = i * i; j <= n; j += i * i) {
                primes_[j] = 0;
            }
        }
    }

    void timer() {
        clock.emplace_back([this] {
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
                    std::cout << primes.size() << " primes found!\n";
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
    }

private:
    std::string Sieve;
    std::vector<long long> primes_;
    std::vector<std::thread> clock;
    bool done = false;
};

inline std::ostream& operator<<(std::ostream& out, const Prime101& v) {
    for(auto& prime : v.primes) {
        out << prime << " ";
    }
    return out;
}

int main() {
    long long limit;
    std::cout << "Input search range: ";
    std::cin >> limit;
    Prime101 prime(limit);
    std::cout << prime << "\n";
    return 0;
}