#include "dining_philosophers.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>
#include <ctime>
#include <random>
#include <vector>
#include <mutex>

const int N = 5; // number of philosophers and chopsticks
const int MAX_EATS = 5;

std::mutex Monitor_Mutex;

std::vector<std::unique_ptr<philosopher>> A_Philosophers;
std::vector<std::unique_ptr<chopstick>> A_Chopsticks;

philosopher::philosopher(int id) : Phil_ID(id), Eat_Count(0), State(THINKING), Time_Hungry(0) {}
philosopher::~philosopher() = default;

chopstick::chopstick(int id) : Chopstick_ID(id), Is_Used(false) {}
chopstick::~chopstick() = default;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dist_eat(1000, 3000);
std::uniform_int_distribution<> dist_think(500, 1500);

chopstick* Get_Left_Chopstick(philosopher* Phil) {
    return A_Chopsticks[Phil->Phil_ID].get();
}

chopstick* Get_Right_Chopstick(philosopher* Phil) {
    return A_Chopsticks[(Phil->Phil_ID + N - 1) % N].get();
}

philosopher* Get_Left_Philosopher(philosopher* Phil) {
    return A_Philosophers[(Phil->Phil_ID + N - 1) % N].get();
}

philosopher* Get_Right_Philosopher(philosopher* Phil) {
    return A_Philosophers[(Phil->Phil_ID + 1) % N].get();
}

bool Can_Eat(philosopher* Phil, int aging_seconds = 5) {
    auto now = time(nullptr);
    philosopher* left_philosopher = Get_Left_Philosopher(Phil);
    philosopher* right_philosopher = Get_Right_Philosopher(Phil);

    if (left_philosopher->State == EATING || right_philosopher->State == EATING)
        return false;

    if (left_philosopher->State == HUNGRY && left_philosopher->Time_Hungry < Phil->Time_Hungry && (now - left_philosopher->Time_Hungry) >= aging_seconds)
        return false;

    if (right_philosopher->State == HUNGRY && right_philosopher->Time_Hungry < Phil->Time_Hungry && (now - right_philosopher->Time_Hungry) >= aging_seconds)
        return false;

    return true;
}

void Pick_Up(philosopher* Phil) {
    std::unique_lock<std::mutex> lock(Monitor_Mutex);

    Phil->State = HUNGRY;
    Phil->Time_Hungry = time(nullptr);

    while (!Can_Eat(Phil)) {
        Phil->Phil_CV.wait(lock);
    }

    Phil->State = EATING;

    lock.unlock();

    chopstick* left_chopstick = Get_Left_Chopstick(Phil);
    chopstick* right_chopstick = Get_Right_Chopstick(Phil);

    chopstick* first = (left_chopstick->Chopstick_ID < right_chopstick->Chopstick_ID) ? left_chopstick : right_chopstick;
    chopstick* second = (first == left_chopstick) ? right_chopstick : left_chopstick;

    {
        std::unique_lock<std::mutex> lk(first->Chopstick_Mutex);
        first->Chopstick_CV.wait(lk, [&]{ return !first->Is_Used; });
        first->Is_Used = true;
    }

    {
        std::unique_lock<std::mutex> lk(second->Chopstick_Mutex);
        second->Chopstick_CV.wait(lk, [&]{ return !second->Is_Used; });
        second->Is_Used = true;
    }
}

void Put_Down(philosopher* Phil) {
    chopstick* left_chopstick = Get_Left_Chopstick(Phil);
    chopstick* right_chopstick = Get_Right_Chopstick(Phil);

    {
        std::lock_guard<std::mutex> lock(left_chopstick->Chopstick_Mutex);
        assert(left_chopstick->Is_Used);
        left_chopstick->Is_Used = false;
        left_chopstick->Chopstick_CV.notify_one();
    }

    {
        std::lock_guard<std::mutex> lock(right_chopstick->Chopstick_Mutex);
        assert(right_chopstick->Is_Used);
        right_chopstick->Is_Used = false;
        right_chopstick->Chopstick_CV.notify_one();
    }

    std::lock_guard<std::mutex> lock(Monitor_Mutex);
    Phil->State = THINKING;

    philosopher* left_philosopher = Get_Left_Philosopher(Phil);
    philosopher* right_philosopher = Get_Right_Philosopher(Phil);

    if (left_philosopher->State == HUNGRY && Can_Eat(left_philosopher)) {
        left_philosopher->Phil_CV.notify_one();
    }
    if (right_philosopher->State == HUNGRY && Can_Eat(right_philosopher)) {
        right_philosopher->Phil_CV.notify_one();
    }
}

void Eat(philosopher* Phil) {
    Phil->Eat_Count++;
    std::cout << "Philosopher " << Phil->Phil_ID << " eats (count: " << Phil->Eat_Count << ").\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(dist_eat(gen)));
}

void Dine(philosopher* Phil) {
    while (Phil->Eat_Count < MAX_EATS) {
        std::this_thread::sleep_for(std::chrono::milliseconds(dist_think(gen)));

        Pick_Up(Phil);
        Eat(Phil);
        Put_Down(Phil);
    }
    std::cout << "Philosopher " << Phil->Phil_ID << " is done eating.\n";
}
