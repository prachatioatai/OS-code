
#include <iostream>
#include <string>
using namespace std;


struct HDD {
    int cylinders;
    int tracks_per_cylinder;
    int sectors_per_track;
    int bytes_per_sector;
    int rpm;  // e.g., 7200 or 15000


    long long totalCapacity() {
        return (long long)cylinders * tracks_per_cylinder
               * sectors_per_track * bytes_per_sector;
    }


    double rotationalLatency() {
        // Average = half rotation, in milliseconds
        return (60.0 / rpm) / 2.0 * 1000.0;
    }
};



int main() {
    HDD disk = {1024, 16, 63, 512};
    cout << "Total Capacity: " << disk.totalCapacity() << " bytes" << endl;
    return 0;
}
