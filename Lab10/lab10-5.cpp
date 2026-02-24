#include <iostream>
#include <vector>
#include <bitset>
using namespace std;

const int TOTAL_BLOCKS = 16;

class DiskManager
{
    bitset<TOTAL_BLOCKS> freeBlocks;

public:
    DiskManager() { freeBlocks.set(); } // all free

    int allocate()
    {
        for (int i = 0; i < TOTAL_BLOCKS; i++)
            if (freeBlocks[i])
            {
                freeBlocks[i] = 0;
                return i;
            }
        throw runtime_error("Disk full: no free blocks available");
        // OR return -1 and always check: if (b == -1) { handle error; }
    }

    void release(int block) { freeBlocks[block] = 1; }

    void status()
    {
        cout << "Free blocks: " << freeBlocks.count() << "/" << TOTAL_BLOCKS << endl;
    }
};

int main()
{
    DiskManager dm;
    int b1 = dm.allocate();
    int b2 = dm.allocate();
    dm.status();
    dm.release(b1);
    dm.status();
    try
    {
        int b = dm.allocate();
        cout << "Allocated block " << b << endl;
    }
    catch (const runtime_error &e)
    {
        cerr << "Allocation error: " << e.what() << endl;
    }
    return 0;
}
