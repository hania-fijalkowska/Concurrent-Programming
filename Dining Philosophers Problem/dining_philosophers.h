#ifndef CONCURRENT_PROGRAMMING_DINING_PHILOSOPHERS_H
#define CONCURRENT_PROGRAMMING_DINING_PHILOSOPHERS_H

#include <mutex>
#include <vector>
#include <condition_variable>
#include <memory>
#include <ctime>

// Forward declarations
class philosopher;
class chopstick;

// Constants
extern const int N;
extern const int MAX_EATS;

// Global containers
extern std::vector<std::unique_ptr<philosopher>> A_Philosophers;
extern std::vector<std::unique_ptr<chopstick>> A_Chopsticks;

// Philosopher states
enum Phil_State { THINKING, HUNGRY, EATING };

// Philosopher class
class philosopher {
public:
    int Phil_ID;
    int Eat_Count;
    Phil_State State;
    std::condition_variable Phil_CV;
    time_t Time_Hungry;

    explicit philosopher(int id);
    ~philosopher();
};

// Chopstick class
class chopstick {
public:
    int Chopstick_ID;
    bool Is_Used;
    std::mutex Chopstick_Mutex;
    std::condition_variable Chopstick_CV;

    explicit chopstick(int id);
    ~chopstick();
};

// Function declarations
void Dine(philosopher* Phil);
chopstick* Get_Left_Chopstick(philosopher* Phil);
chopstick* Get_Right_Chopstick(philosopher* Phil);
philosopher* Get_Left_Philosopher(philosopher* Phil);
philosopher* Get_Right_Philosopher(philosopher* Phil);

#endif