#include <iostream>
#include <queue>
using namespace std;


struct NVMRequest {
    int lba;        // Logical Block Address
    string type;    // "read" or "write"
};


void processNVM(queue<NVMRequest>& q) {
    queue<NVMRequest> reads, writes;
    while (!q.empty()) {
        NVMRequest r = q.front(); q.pop();
        if (r.type == "read") reads.push(r);
        else                  writes.push(r);
    }
    // Process reads first
    while (!reads.empty()) {
        auto r = reads.front(); reads.pop();
        cout << "READ  LBA " << r.lba << " -> ~0.05ms" << endl;
    }
    // Then writes
    while (!writes.empty()) {
        auto r = writes.front(); writes.pop();
        cout << "WRITE LBA " << r.lba << " -> ~0.15ms" << endl;
    }
}



int main() {
    queue<NVMRequest> q;
    q.push({100, "read"});
    q.push({5,   "write"});
    q.push({999, "read"});
    processNVM(q);
    return 0;
}
