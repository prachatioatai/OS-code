#include <iostream>
#include <array>
#include <algorithm>
using namespace std;


const int BLOCKS = 4;
array<int,BLOCKS> writeCounts = {0,0,0,0};
array<int,BLOCKS> logicalToPhysical = {0,1,2,3}; // initial mapping


int wearLevelWrite(int logicalAddr, const string& data) {
    // Find physical block with minimum write count
    int minBlock = min_element(writeCounts.begin(), writeCounts.end())
                  - writeCounts.begin();
    writeCounts[minBlock]++;
    logicalToPhysical[logicalAddr % BLOCKS] = minBlock;
    cout << "Write to logical " << logicalAddr
         << " -> physical block " << minBlock
         << " (writes: " << writeCounts[minBlock] << ")" << endl;
    return minBlock;
}


int main() {
    for (int i = 0; i < 8; i++) wearLevelWrite(i % 4, "data");
    return 0;
}
