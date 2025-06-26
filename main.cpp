#include "WiTi Problem/WiTi.h"
#include "Dining Philosophers Problem/dining_philosophers.h"
#include "Readers-Writers Problem/readers_writers.h"

#include <iostream>
#include <thread>
#include <vector>

void Run_WiTi(){
    std::string filename = "data.txt";
    std::vector<Task> tasks = readTasksFromFile(filename);

    std::vector<int> order;
    int minPenalty = weightedTardinessDP(tasks, order);

    std::cout << "Minimalna kara: " << minPenalty << std::endl;

    std::cout << "Kolejnosc wykonywania zadan: ";
    for (int idx : order) {
        std::cout << (idx + 1) << " ";
    }
    std::cout << std::endl;
}

void Run_Philosophers(){

    for (int i = 0; i < N; ++i) {
        A_Chopsticks.emplace_back(std::make_unique<chopstick>(i));
    }

    for (int i = 0; i < N; ++i) {
        A_Philosophers.emplace_back(std::make_unique<philosopher>(i));
    }

    std::vector<std::thread> threads;

    for (auto &phil: A_Philosophers) {
        threads.emplace_back(Dine, phil.get());
    }

    for (auto &t: threads) {
        t.join();
    }
    std::cout << "All philosophers are done eating.\n";
}

void Run_Readers_Writers(){
    data shared_data;
    std::vector<std::thread> threads;

    for (int i = 0; i < 5; ++i)
        threads.emplace_back(reader(i, shared_data));

    for (int i = 0; i < 2; ++i)
        threads.emplace_back(writer(i, shared_data));

    std::this_thread::sleep_for(std::chrono::seconds(5));

    shared_data.StopSimulation();

    for (auto& t : threads){
        t.join();
    }


    std::cout << "Readers and writes are done.\n";
}

int main() {
    int choice;
    std::cout << "Select a problem to rub:\n";
    std::cout << "1. WiTi\n";
    std::cout << "2. Dining Philosophers\n";
    std::cout << "3. Readers Writers\n";
    std::cout << "Your choice: ";
    std::cin >> choice;

    switch (choice) {
        case 1:
            Run_WiTi();
            break;
        case 2:
            Run_Philosophers();
            break;
        case 3:
            Run_Readers_Writers();
            break;
        default:
            std::cout << "Niepoprawny wybor.\n";
    }
    return 0;
}
