#include <chrono>
#include <cmath>
#include <iostream>
#include <cstdint>
#include <thread>
#include <vector>

// Forward Declarations

using namespace std;

// ===== Global Variables =====

// Thread Pool Information
int poolSize = 6;   // Change depending on the # of cores in your machine, 6 for my machine
auto threadPool = new thread[poolSize];
uint64_t currentTask[6] = {0, 0, 0, 0, 0, 0};
bool tasksDone = false;

// Task Information
int completedTasks = 0;
int primeCount = 0;
vector<uint64_t> primeCheckTasks;

bool isPrime(const uint64_t n, const string &runner) {
    bool is_prime = true;
    const auto sqrt_n = static_cast<uint64_t>(ceil(sqrt(n)));

    if (n == 0 || n == 1) {
        return false;
    }

    // Only check up to the square root of n
    // Since every divisor > sqrt(n) has a corresponding divisor < sqrt(n)
    for (uint64_t i = 2; i <= sqrt_n; i++) {
        if (n % i == 0) {
            is_prime = false;
            break;
        }
    }

    if (is_prime) {
        cout << runner << ": " << n << " is PRIME." << endl;
        primeCount++;
    }

    return is_prime;
}

void primeTask(const int index) {
    uint64_t numToCheck = 0;
    string threadName = "T-" + to_string(index);

    while (!tasksDone) {
        numToCheck = currentTask[index];
        if (numToCheck != 0) {
            isPrime(numToCheck, threadName);
            currentTask[index] = 0;
        }
    }
}

/* ===== DEMO 02: Parallel Primality Check ===== */
void demo02(const size_t listSize, uint64_t *numberList) {
    vector<thread> workerThreads;
    for (int i = 0; i < listSize; i++) {
        workerThreads.emplace_back(isPrime, numberList[i], "T-" + to_string(i));
    }

    for (auto &t: workerThreads) {
        t.join();
    }
}

/* ====== DEMO 01: Serial Primality Check ===== */
void demo01(const size_t listSize, const uint64_t *numberList) {
    for (int i = 0; i < listSize; i++) {
        isPrime(numberList[i], "MAIN");
    }
}

/* ===== DEMO 03: Thread Pool ===== */
void demo03(const size_t listSize, const uint64_t *numberList) {
    for (int i = 0; i < listSize; i++) {
        primeCheckTasks.push_back(numberList[i]);
    }

    for (int i = 0; i < poolSize; i++) {
        threadPool[i] = thread{primeTask, i};
    }

    while (!primeCheckTasks.empty()) {
        for (int i = 0; i < poolSize; i++) {
            if (currentTask[i] == 0) {
                currentTask[i] = primeCheckTasks[0];
                primeCheckTasks.erase(primeCheckTasks.begin());
                break;
            }
        }
    }

    tasksDone = true;

    for (int i = 0; i < poolSize; i++) {
        threadPool[i].join();
    }

    cout << "Prime Count: " << to_string(primeCount) << endl;
    cout << "===== MAIN: END =====" << endl << endl;
}

int main() {
    cout << endl << "===== MAIN: START =====";

    constexpr size_t listSize = 60;
    auto *numberList = new uint64_t[listSize];

    /* ===== Fill numberList 1/8 prime numbers and 7/8 non-prime ===== */

    for (int i = 0; i < listSize; i++) {
        if (i % 6 == 0) {
            numberList[i] = (uint64_t) 9446744074328015681; // 9446744074328015681 is a prime number
            continue;
        }
        numberList[i] = (uint64_t) 9446744074709551617; // 9446744074709551617 is a non-prime number
    }

    const auto start = chrono::high_resolution_clock::now();

    switch (constexpr int demoChoice = 1) {
        case 1:
            demo01(listSize, numberList);   // ~ 50 seconds
            break;
        case 2:
            demo02(listSize, numberList);   // ~ 14 seconds
            break;
        case 3:
            demo03(listSize, numberList);   // ~ 13 seconds
            break;
        default:
            cout << "Invalid demo choice." << endl;
    }

    const auto end = chrono::high_resolution_clock::now();
    const chrono::duration<double> duration = end - start;
    cout << "Time taken: " << duration.count() << " seconds" << endl;

    // Delete the allocated memory to prevent memory leaks
    delete[] numberList;
    delete[] threadPool;

    return 0;
}