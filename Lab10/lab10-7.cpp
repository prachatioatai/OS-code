#include <iostream>
#include <string>
using namespace std;

class StorageDevice
{
public:
    virtual string getInterface() = 0;
    virtual double getBandwidth() = 0; // MB/s
    virtual void read(int lba)
    {
        cout << getInterface() << ": Reading LBA " << lba
             << " at " << getBandwidth() << " MB/s" << endl;
    }
    virtual ~StorageDevice() {}
};

class SATADrive : public StorageDevice
{
public:
    string getInterface() override { return "SATA"; }
    double getBandwidth() override { return 600.0; }
};

class NVMeDrive : public StorageDevice
{
public:
    string getInterface() override { return "NVMe"; }
    double getBandwidth() override { return 7000.0; }
};

class USBDrive : public StorageDevice
{
public:
    string getInterface() override { return "USB 3.0"; }
    double getBandwidth() override { return 480.0; }
};

int main()
{
    StorageDevice *d1 = new SATADrive();
    StorageDevice *d2 = new NVMeDrive();
    // Usage in main():
    StorageDevice *d3 = new USBDrive();
    d3->read(200);
    // Output: USB 3.0: Reading LBA 200 at 480 MB/s
    delete d3;

    d1->read(100);
    d2->read(100);
    delete d1;
    delete d2;
    return 0;
}
