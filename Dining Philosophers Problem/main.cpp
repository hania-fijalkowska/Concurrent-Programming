#include "dining_philosophers.h"
#include <thread>
#include <vector>
#include <iostream>

int main() {
    for (int i = 0; i < N; ++i) {
        A_Chopsticks.emplace_back(std::make_unique<chopstick>(i));
    }
    for (int i = 0; i < N; ++i) {
        A_Philosophers.emplace_back(std::make_unique<philosopher>(i));
    }

    std::vector<std::thread> threads;
    for (auto& phil : A_Philosophers) {
        threads.emplace_back(Dine, phil.get());
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "All philosophers are done eating.\n";

    return 0;
}
