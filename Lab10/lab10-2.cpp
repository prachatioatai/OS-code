#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;


int fcfs(int head, vector<int> requests) {
    int totalMovement = 0;
    for (int req : requests) {
        totalMovement += abs(req - head);
        head = req;
    }
    return totalMovement;
}


int main() {
    vector<int> req = {98, 183, 37, 122, 14, 124, 65, 67};
    cout << "FCFS Total Head Movement: " << fcfs(53, req) << endl;
    // Output: 640 cylinders
    return 0;
}
