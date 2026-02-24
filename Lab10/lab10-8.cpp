#include <iostream>
#include <vector>
#include <string>
using namespace std;



class RAID {
    int numDisks;
    vector<vector<string>> disks;
public:
    RAID(int n) : numDisks(n), disks(n) {}


    // RAID-0: stripe data across disks
    void writeRAID0(string data) {
        for (int i = 0; i < data.size(); i++)
            disks[i % numDisks].push_back(string(1, data[i]));
    }


    // RAID-1: mirror data to all disks
    void writeRAID1(string data) {
        for (auto& disk : disks)
            disk.push_back(data);
    }


    void status() {
        for (int i = 0; i < numDisks; i++) {
            cout << "Disk " << i << ": ";
            for (auto& b : disks[i]) cout << b << " ";
            cout << endl;
        }
    }
    void writeRAID5(string blockA, string blockB) {
    // Compute parity: XOR all characters
    string parity = "";
    for (int i = 0; i < min(blockA.size(), blockB.size()); i++)
        parity += char(blockA[i] ^ blockB[i]);


    // Stripe 0: parity on disk 2
    disks[0].push_back(blockA);
    disks[1].push_back(blockB);
    disks[2].push_back(parity); // P on disk 2


    // Next stripe: rotate parity to disk 1
    // disks[0].push_back(blockC);
    // disks[1].push_back(parity2); // P on disk 1
    // disks[2].push_back(blockD);
}


// Recovery example: if disk 0 fails, restore from disk 1 XOR disk 2 (parity)

};


int main() {
    RAID r0(3);
    r0.writeRAID0("ABCDEF");
    cout << "=== RAID-0 ===" << endl; r0.status();


    RAID r1(3);
    r1.writeRAID1("BACKUP");
    cout << "=== RAID-1 ===" << endl; r1.status();


    RAID r5(3);
    r5.writeRAID5("","");
    cout << "== RAID-5 ===" << endl; r5.status();

    return 0;

}
