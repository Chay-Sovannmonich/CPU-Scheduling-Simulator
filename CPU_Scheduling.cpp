#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <limits>

using namespace std;

struct Process {
    string id;
    int arrival, burst, remaining, completion, waiting, turnaround, start;
};

// Function to calculate waiting and turnaround times
void calculateTimes(vector<Process> &processes) {
    int n = processes.size();
    double total_waiting = 0, total_turnaround = 0;

    cout << "\nProcess\tWaiting Time\tTurnaround Time\n";
    for (auto &p : processes) {
        p.turnaround = p.completion - p.arrival;
        p.waiting = p.turnaround - p.burst;
        total_waiting += p.waiting;
        total_turnaround += p.turnaround;
        cout << p.id << "\t" << p.waiting << "\t\t" << p.turnaround << "\n";
    }
    cout << "\nAverage Waiting Time: " << total_waiting / n;
    cout << "\nAverage Turnaround Time: " << total_turnaround / n << "\n";
}

// First-Come, First-Served (FCFS)
void FCFS(vector<Process> processes) {
    sort(processes.begin(), processes.end(), [](Process a, Process b) {
        return a.arrival < b.arrival;
    });
    
    int time = 0;
    cout << "\nGantt Chart: ";
    for (auto &p : processes) {
        if (time < p.arrival) time = p.arrival;
        p.start = time;
        cout << "| " << p.id << " ";
        time += p.burst;
        p.completion = time;
    }
    cout << "|\n";
    calculateTimes(processes);
}

// Shortest-Job-First (Non-Preemptive SJF)
void SJF(vector<Process> processes) {
    int time = 0, completed = 0;
    vector<bool> done(processes.size(), false);
    cout << "\nGantt Chart: ";
    
    while (completed < processes.size()) {
        int idx = -1, min_burst = 1e9;
        for (int i = 0; i < processes.size(); i++) {
            if (!done[i] && processes[i].arrival <= time && processes[i].burst < min_burst) {
                min_burst = processes[i].burst;
                idx = i;
            }
        }
        if (idx == -1) {
            time++;
            continue;
        }
        processes[idx].start = time;
        cout << "| " << processes[idx].id << " ";
        time += processes[idx].burst;
        processes[idx].completion = time;
        done[idx] = true;
        completed++;
    }
    cout << "|\n";
    calculateTimes(processes);
}

// Shortest-Remaining-Time (Preemptive SJF)
void SRT(vector<Process> processes) {
    int time = 0, completed = 0;
    int n = processes.size();
    for (auto &p : processes) p.remaining = p.burst;
    
    cout << "\nGantt Chart: ";
    while (completed < n) {
        int idx = -1, min_remain = 1e9;
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= time && processes[i].remaining > 0 && processes[i].remaining < min_remain) {
                min_remain = processes[i].remaining;
                idx = i;
            }
        }
        if (idx == -1) {
            time++;
            continue;
        }
        cout << "| " << processes[idx].id << " ";
        processes[idx].remaining--;
        time++;
        if (processes[idx].remaining == 0) {
            processes[idx].completion = time;
            completed++;
        }
    }
    cout << "|\n";
    calculateTimes(processes);
}

// Round Robin Scheduling
void RoundRobin(vector<Process> processes, int quantum) {
    queue<int> q;
    int time = 0;
    vector<bool> inQueue(processes.size(), false);
    
    for (int i = 0; i < processes.size(); i++) {
        processes[i].remaining = processes[i].burst;
        if (processes[i].arrival == 0) {
            q.push(i);
            inQueue[i] = true;
        }
    }
    
    cout << "\nGantt Chart: ";
    while (!q.empty()) {
        int i = q.front(); q.pop();
        if (processes[i].remaining > quantum) {
            processes[i].remaining -= quantum;
            time += quantum;
            cout << "| " << processes[i].id << " ";
            for (int j = 0; j < processes.size(); j++) {
                if (!inQueue[j] && processes[j].arrival <= time) {
                    q.push(j);
                    inQueue[j] = true;
                }
            }
            q.push(i);
        } else {
            time += processes[i].remaining;
            processes[i].completion = time;
            processes[i].remaining = 0;
            cout << "| " << processes[i].id << " ";
        }
    }
    cout << "|\n";
    calculateTimes(processes);
}

// Menu to choose scheduling algorithm
int main() {
    int choice;
    do {
        cout << "\nCPU Scheduling Algorithms:\n";
        cout << "1. FCFS\n2. SJF\n3. SRT\n4. Round Robin\n5. Exit\n";
        cout << "Choose an option: ";
        while (!(cin >> choice) || choice < 1 || choice > 5) {
            cout << "Invalid choice. Please enter a number between 1 and 5: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        if (choice == 5) break;
        
        int n;
        cout << "Enter number of processes: ";
        while (!(cin >> n) || n <= 0) {
            cout << "Invalid input. Enter a positive integer: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        vector<Process> processes(n);
        for (int i = 0; i < n; i++) {
            cout << "Enter Process ID, Arrival Time, Burst Time: ";
            while (!(cin >> processes[i].id >> processes[i].arrival >> processes[i].burst) || processes[i].arrival < 0 || processes[i].burst <= 0) {
                cout << "Invalid input. Enter valid Process ID, non-negative Arrival Time, and positive Burst Time: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
        
        if (choice == 1) FCFS(processes);
        else if (choice == 2) SJF(processes);
        else if (choice == 3) SRT(processes);
        else if (choice == 4) {
            int quantum;
            cout << "Enter Time Quantum: ";
            while (!(cin >> quantum) || quantum <= 0) {
                cout << "Invalid input. Enter a positive integer for Time Quantum: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            RoundRobin(processes, quantum);
        }
    } while (choice != 5);
    return 0;
}