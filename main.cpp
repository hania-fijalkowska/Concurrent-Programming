//#include "Dining Philosophers Problem/dining_philosophers.h"
//#include "Readers-Writers Problem/readers_writers.h"
//#include <thread>
//#include <vector>
//#include <iostream>
//
//int main() {
//    {
//        for (int i = 0; i < N; ++i) {
//            A_Chopsticks.emplace_back(std::make_unique<chopstick>(i));
//        }
//        for (int i = 0; i < N; ++i) {
//            A_Philosophers.emplace_back(std::make_unique<philosopher>(i));
//        }
//
//        std::vector<std::thread> threads;
//        for (auto &phil: A_Philosophers) {
//            threads.emplace_back(Dine, phil.get());
//        }
//
//        for (auto &t: threads) {
//            t.join();
//        }
//
//        std::cout << "All philosophers are done eating.\n";
//    }
//
//    {
//        data shared_data;
//
//        const int num_readers = 6;
//        const int num_writers = 2;
//
//        std::vector<std::thread> threads;
//
//        // Launch writers
//        for (int i = 0; i < num_writers; ++i) {
//            threads.emplace_back(writer(i + 1, shared_data));
//        }
//
//        // Launch readers
//        for (int i = 0; i < num_readers; ++i) {
//            threads.emplace_back(reader(i + 1, shared_data));
//        }
//
//        // Wait for threads to complete
//        for (auto& t : threads) {
//            if (t.joinable()) t.join();
//        }
//
//        // Shutdown signal (in case some threads are still waiting)
//        shared_data.Shutdown();
//
//        std::cout << "All reader and writer threads completed.\n";
//    }
//
//    return 0;
//}

#include "Readers-Writers Problem/readers_writers.h"
#include <thread>
#include <vector>

int main() {
    data shared_data;

    std::vector<std::thread> readers;
    std::vector<std::thread> writers;

    // Start 3 readers
    for (int i = 0; i < 3; ++i) {
        readers.emplace_back(reader(i + 1, shared_data));
    }

    // Start 1 writer
    writers.emplace_back(writer(1, shared_data));

    // Run for a while then shutdown
    std::this_thread::sleep_for(std::chrono::seconds(10));
    shared_data.Shutdown();

    for (auto& r : readers) r.join();
    for (auto& w : writers) w.join();

    return 0;
}