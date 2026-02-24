#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;


int hammingDistance(vector<int>& a, vector<int>& b) {
    if (a.size() != b.size())
        throw invalid_argument("Vectors must be same length");
    int dist = 0;
    for (int i = 0; i < a.size(); i++)
        if (a[i] != b[i]) dist++;
    return dist;
}


// Example: a={1,0,1,1}, b={1,0,0,1} → distance = 1

int computeParity(vector<int>& bits) {
    int parity = 0;
    for (int b : bits) parity ^= b;
    return parity;
}


bool checkParity(vector<int>& data, int storedParity) {
    return computeParity(data) == storedParity;
}


int main() {
    vector<int> data = {1, 0, 1, 1, 0, 0, 1, 0};
    int parity = computeParity(data);
    cout << "Parity bit: " << parity << endl;


    // Simulate a 1-bit error
    data[3] = 1 - data[3];
    cout << "Error detected: " << !checkParity(data, parity) << endl;

    hammingDistance;
    return 0;
}
