// Libraries
#include <iostream>
#include <thread>

// Define class
class Prime101
{
public:
    // Create empty list of primes
    std::vector<long long> primes;

    // Setup stuff
    Prime101(long long n) : Sieve("auto") {
        timer();
        find_primes(n);
    }

    Prime101(std::string sieve, long long n) : Sieve(sieve) {
        timer();
        find_primes(n);
    }

    Prime101(long long n, std::string sieve) : Sieve(sieve) {
        timer();
        find_primes(n);
    }

    // Destructor
    ~Prime101()
    {
        done = true;
        for(auto& time : clock) {
            time.join();
        }
    }

    // Prime finding function
    void find_primes(long long n) {
        // Select sieve to use
        if(Sieve == "eratosthenes" || Sieve == "sieve of eratosthenes" || Sieve == "soe" || Sieve == "SoE" || Sieve == "SOE" || Sieve == "eratos") {
            // Sieve of Eratosthenes
            Soe(n);
        } else if(Sieve == "Atkin" || Sieve == "sieve of atkin" || Sieve == "soa" || Sieve == "SoA" || Sieve == "SOA" || Sieve == "atkin") {
            // Sieve of Atkin
            Soa(n);
        } else if(Sieve == "auto" || Sieve == "Auto") {
            // Automatic sieve selection based on search range
            // Selection determind by the time complexity of each sieve
            if(n < 10000000000LL) {
                Soe(n);
            } else if(n > 10000000000LL) {
                Soa(n);
            }
        }

        // Add primes to list of primes
        for(long long i = 2; i <= n; ++i) {
                if(primes_[i]) {
                    primes.emplace_back(i);
                }
            }
    }

    void Soe(long long n) {
        // Sieve of Eratosthenes
        primes_.assign(n + 1, 1); // Create a list of 1's (1 = True) with a length of n + 1

        // Loop through all natural numbers less than √n
        for(long long i = 2; i * i <= n; ++i) {

            // Skip if it is a multiple of a prime
            if(primes_[i]) {

                // Mark all multiples of primes as not prime
                for(long long j = i * i; j <= n; j += i) {
                    primes_[j] = 0;
                }
            }
        }
    }

    void Soa(long long n) {
        // Sieve of Atkin
        primes_.assign(n + 1, 0); // Create a list of 0's (0 = False) with a length of n + 1

        if(n > 2) primes_[2] = 1; // Mark 2 as prime
        if(n > 3) primes_[3] = 1; // Mark 3 as prime

        // Loop through all natural numbers less than √n
        for(long long i = 1; i * i <= n; ++i) {

            // Nested loop of all natural numbers less than √n
            for(long long j = 1; j * j <= n; ++j) {

                // Check #1
                long long k = 4 * i * i + j * j; // n = 4x^2 + y^2
                // If n is odd, has a remainder of 1 or 5 when divided by 12, and is not a perfect square, then n is prime
                if(k <= n && (k % 12 == 1 || k % 12 == 5)) {
                    primes_[k] = (primes_[k] + 1) % 2; // Flip the value of k
                }

                k = 3 * i * i + j * j; // n = 3x^2 + y^2
                // If n is odd, has a remainder of 7 when divided by 12, and is not a perfect square, then n is prime
                if(k <= n && k % 12 == 7) {
                    primes_[k] = (primes_[k] + 1) % 2; // Flip the value of k
                }

                k = 3 * i * i - j * j; // n = 3x^2 - y^2
                // If n is odd, has a remainder of 11 when divided by 12, is not a perfect square, and x > y, then n is prime
                if(i > j && k <= n && k % 12 == 11) {
                    primes_[k] = (primes_[k] + 1) % 2; // Flip the value of k
                }
            }
        }

        // Remove perfect squares
        for(long long i = 5; i * i <= n; ++i) {
            if(!primes_[i]) continue; // Skip if it is not marked as prime

            // If it is marked as prime mark all its multiples as not prime
            for(long long j = i * i; j <= n; j += i * i) {
                primes_[j] = 0;
            }
        }
    }

    void timer() {
        // Can ignore this it is just a timer
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
                    std::cout << primes.size() << " primes found!\n"; // Output number of primes found
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
    // Variables
    std::string Sieve;
    std::vector<long long> primes_;
    std::vector<std::thread> clock;
    bool done = false;
};

inline std::ostream& operator<<(std::ostream& out, const Prime101& v) {
    // Print function
    for(auto& prime : v.primes) {
        out << prime << " ";
    }
    return out;
}

int main() {
    long long limit; // Empty long long int
    std::cout << "Input search range: "; // Prints message to terminal
    std::cin >> limit; // Set limit to user input
    Prime101 prime(limit); // Initalize Prime101 with input(limit)
    std::cout << prime << "\n"; // Output all found primes
    return 0;
}