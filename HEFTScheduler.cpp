#include "bits/stdc++.h"
using namespace std;

vector<vector<int>> dag;
vector<vector<int>> cost;
vector<double> ranks;
int n, m;
vector<set<pair<int,int>>> procSchedule;
vector<int> AST, assignedProcessor;

double calculateAvgRunningTime(int task) {
    double totalTime = 0;
    for(int proc = 0; proc < m; proc++) totalTime += cost[task][proc];
    totalTime /= m;
    return totalTime;
}

void calculateRank(int task) {
    double maxNext = 0;
    for(int next = 0; next < n; next++) {
        if(dag[task][next] == -1) continue;
        if(ranks[next] == -1) calculateRank(next);
        maxNext = max(maxNext, dag[task][next] + ranks[next]);
    }
    ranks[task] = calculateAvgRunningTime(task) + maxNext;
}

int sortByRank(int a, int b) { return (ranks[a] < ranks[b]); }

int calculateEFT(int task, int proc) {
    int EST = 0;
    for(int pred = 0; pred < n; pred++) {
        if(dag[pred][task] == -1) continue;
        EST = max(EST, AST[pred] + cost[pred][assignedProcessor[pred]] + ((assignedProcessor[pred] == proc)?0:dag[pred][task]));
    }

    if(procSchedule[proc].empty()) return (EST + cost[task][proc]);
    auto itr = prev(procSchedule[proc].lower_bound({EST, -1}));
    int EFT = 1e9;
    while(itr != procSchedule[proc].end()) {
        auto [curTime, curTask] = *itr;
        if(next(itr) == procSchedule[proc].end()) {
            EFT = max(EST + cost[task][proc], curTime + cost[curTask][proc] + cost[task][proc]);
            break;
        }
        else {
            auto [nextTime, nextTask] = *next(itr);
            if(max(EST + cost[task][proc], curTime + cost[curTask][proc] + cost[task][proc]) > nextTime) { itr++; continue; }
            EFT = max(EST + cost[task][proc], curTime + cost[curTask][proc] + cost[task][proc]);
            break;
        }
    }
    return EFT;
}

void schedule(int task) {
    int curProc = 0, curEFT = calculateEFT(task, 0);
    for(int proc = 1; proc < m; proc++) {
        int nextEFT = calculateEFT(task, proc);
        if(nextEFT < curEFT) { curEFT = nextEFT; curProc = proc; }
    }

    int ast = curEFT - cost[task][curProc];
    procSchedule[curProc].insert({ast, task});
    AST[task] = ast;
    assignedProcessor[task] = curProc;
}

int main() {
    cin>>n>>m; //No. of tasks, No. of processors
    dag.resize(n);
    for(int i = 0; i < n; i++) {
        dag[i].resize(n);
        for(int j = 0; j < n; j++) cin>>dag[i][j]; //dag[i][j] = amount of input task j needs from i
    }
    cost.resize(n);
    for(int i = 0; i < n; i++) {
        cost[i].resize(n);
        for(int j = 0; j < m; j++) cin>>cost[i][j]; //cost[i][j] = time taken for task i to finish on processor j
    }
    ranks.resize(n, -1);
    for(int i = 0; i < n; i++) if(ranks[i] == -1) calculateRank(i);
    vector<int> rankOrder(n); iota(rankOrder.begin(), rankOrder.end(), 0);
    sort(rankOrder.begin(), rankOrder.end(), sortByRank);
    AST.resize(n); 
    assignedProcessor.resize(n);
    procSchedule.resize(m);

    //for(int i = 0; i < rankOrder.size(); i++) cout << rankOrder[i] << " "; cout << endl;
    while(rankOrder.size()) {
        int curTask = rankOrder.back();
        schedule(curTask);
        rankOrder.pop_back();
    }

    cout << "Task\tProc\tAST\t\tAFT\n";
    int makespan = 0;
    for(int i = 0; i < n; i++) {
        cout << i << " \t\t " << assignedProcessor[i] << " \t\t " << AST[i] << " \t " << AST[i] + cost[i][assignedProcessor[i]] << "\n";
        makespan = max(makespan, AST[i] + cost[i][assignedProcessor[i]]);
    } 
    cout << "\nMakespan = " << makespan << "\n";
}