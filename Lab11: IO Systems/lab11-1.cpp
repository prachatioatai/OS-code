#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <functional>
#include <mutex>
using namespace std;

// Status Register
struct StatusRegister
{
    bool busy = false;
    bool error = false;
    bool ready = true;
    bool transferComplete = false;
};

// I/O Port
class IOPort
{
public:
    uint8_t dataRegister;
    StatusRegister status;
    uint8_t controlRegister;

    IOPort()
    {
        dataRegister = 0;
        controlRegister = 0;
    }

    void writeData(uint8_t data)
    {
        if (status.busy)
        {
            cout << "Error: Device is busy!" << endl;
            status.error = true;
            return;
        }

        status.busy = true;
        status.ready = false;
        dataRegister = data;

        cout << "Writing data: " << (int)data << endl;

        // simulate transfer delay
        this_thread::sleep_for(chrono::milliseconds(1000));

        status.busy = false;
        status.ready = true;
        status.transferComplete = true;
    }

    uint8_t readData()
    {
        return dataRegister;
    }

    bool pollStatus()
    {
        return status.ready;
    }
};

// Device Controller
class DeviceController
{
private:
    IOPort port;
    string deviceName;
    queue<function<void()>> interruptQueue;

public:
    DeviceController(string name)
    {
        deviceName = name;
    }

    void pollingIO(uint8_t data)
    {
        cout << "[POLLING] Waiting for device ready..." << endl;

        while (!port.pollStatus())
        {
            this_thread::sleep_for(chrono::milliseconds(100));
        }

        port.writeData(data);
        cout << "[POLLING] Data sent successfully.\n";
    }

    void interruptDrivenIO(uint8_t data, function<void()> callback)
    {
        cout << "[INTERRUPT] Sending data..." << endl;

        thread t([=]() mutable
                 {
            port.writeData(data);
            interruptQueue.push(callback); });

        t.detach();
    }

    void processInterrupts()
    {
        while (!interruptQueue.empty())
        {
            auto handler = interruptQueue.front();
            interruptQueue.pop();
            handler();
        }
    }
};

int main()
{
    cout << "=== I/O Hardware Simulation ===" << endl;

    DeviceController keyboard("Keyboard");
    DeviceController disk("Disk Drive");

    cout << "\n--- Polling I/O Test ---" << endl;
    keyboard.pollingIO(65);

    cout << "\n--- Interrupt-driven I/O Test ---" << endl;
    disk.interruptDrivenIO(99, []()
                           { cout << "Interrupt: Transfer Complete!" << endl; });

    this_thread::sleep_for(chrono::milliseconds(1500));
    disk.processInterrupts();

    return 0;
}
