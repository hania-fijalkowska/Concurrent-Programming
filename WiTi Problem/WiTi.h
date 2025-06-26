#ifndef WITI_H
#define WITI_H

#include <vector>
#include <string>

struct Task {
    int p, w, d;
};

std::ostream& operator<<(std::ostream& os, const Task& task);
std::vector<Task> readTasksFromFile(const std::string& filename);
void printTasks(const std::vector<Task>& tasks);
int calculatePenalty(const Task& task, int completionTime);
int weightedTardinessDP(const std::vector<Task>& tasks, std::vector<int>& resultOrder);

#endif // WITI_H
