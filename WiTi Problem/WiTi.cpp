#include "WiTi.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm>

using namespace std;

ostream& operator<<(ostream& os, const Task& task) {
    os << "p: " << task.p << ", w: " << task.w << ", d: " << task.d;
    return os;
}

vector<Task> readTasksFromFile(const string& filename) {
    vector<Task> tasks;
    ifstream file(filename);

    if (!file) {
        cerr << "Could not open file: " << filename << endl;
        return tasks;
    }

    int numTasks;
    file >> numTasks;

    for (int i = 0; i < numTasks; ++i) {
        Task task;
        if (file >> task.p >> task.w >> task.d) {
            tasks.push_back(task);
        } else {
            cerr << "Error reading task at line " << (i + 2) << endl;
            break;
        }
    }

    return tasks;
}

void printTasks(const vector<Task>& tasks) {
    for (const auto& task : tasks) {
        cout << task << endl;
    }
}

int calculatePenalty(const Task& task, int completionTime) {
    return max(0, (completionTime - task.d) * task.w);
}

int weightedTardinessDP(const vector<Task>& tasks, vector<int>& resultOrder) {
    int n = tasks.size();
    int size = 1 << n;

    vector<vector<int>> Penalty_CompletionTime(2, vector<int>(size));
    vector<int> parent(size, -1);

    Penalty_CompletionTime[0][0] = 0;
    Penalty_CompletionTime[1][0] = 0;

    for (int i = 1; i < size; ++i) {
        Penalty_CompletionTime[0][i] = numeric_limits<int>::max();
    }

    for (int mask = 1; mask < size; ++mask) {
        for (int k = 0; k < n; ++k) {
            if (mask & (1 << k)) {
                int prevMask = mask ^ (1 << k);
                int time = Penalty_CompletionTime[1][prevMask] + tasks[k].p;
                int penalty = Penalty_CompletionTime[0][prevMask] + calculatePenalty(tasks[k], time);

                if (penalty < Penalty_CompletionTime[0][mask]) {
                    Penalty_CompletionTime[0][mask] = penalty;
                    Penalty_CompletionTime[1][mask] = time;
                    parent[mask] = k;
                }
            }
        }
    }

    int mask = size - 1;
    while (mask > 0) {
        int lastTask = parent[mask];
        resultOrder.push_back(lastTask);
        mask ^= (1 << lastTask);
    }
    reverse(resultOrder.begin(), resultOrder.end());

    return Penalty_CompletionTime[0][size - 1];
}
