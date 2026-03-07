#include <iostream>
#include <functional>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>
using namespace std;
// ■■ StatusRegister ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ struct StatusRegister {
class StatusRegister
{
public:
    bool busy = false, ready = true, error = false, done = false;
};
// ■■ IOPort ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ class IOPort {
class IOPort
{
public:
    uint8_t data = 0;
    StatusRegister status;
    uint8_t control = 0;
    void writeData(uint8_t v)
    {
        if (status.busy)
        {
            status.error = true;
            return;
        }
        status.busy = true;
        status.ready = false;
        data = v;
        // simulate short transfer
        this_thread::sleep_for(chrono::microseconds(1));
        status.busy = false;
        status.ready = true;
        status.done = true;
    }
    uint8_t readData() { return data; }
    bool pollStatus() { return status.ready; }
};
// ■■ DeviceController ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ class DeviceController {
class DeviceController
{
    IOPort port;
    queue<function<void()>> irqQueue;
    mutex mtx;

public:
    // POLLING: CPU busy-waits for each byte
    long pollingIO(uint8_t d)
    {
        long cycles = 0;
        while (!port.pollStatus())
        {
            ++cycles;
        } // busy-wait
        port.writeData(d);
        cycles += 2; // write + check
        return cycles;
    }
    // INTERRUPT-DRIVEN: CPU programs device, registers callback, is freed long interruptDrivenIO(uint8_t d, function<void()> cb) {
    long interruptDrivenIO(uint8_t d, function<void()> cb)
    {
        long cycles = 1; // 1 cycle to issue command
        port.writeData(d);
        {
            lock_guard<mutex> lk(mtx);
            irqQueue.push(cb);
        }
        processIRQ();
        cycles += 1; // 1 cycle for ISR invocation
        return cycles;
    }
    // DMA: CPU sets up once, DMA engine transfers entire buffer autonomously
    long dmaTransfer(vector<uint8_t> &dst,
                     const vector<uint8_t> &src)
    {
        long cycles = 1; // 1 cycle: program DMA registers
        std::cout << " [DMA] CPU programs DMA: src=buffer, dst=RAM, count=" << src.size() << "\n";
        std::cout << " [DMA] CPU is FREE during transfer...\n";
        dst = src; // DMA engine copies (no CPU per-byte cost)
        std::cout << " [DMA] Transfer complete — IRQ raised\n";
        cycles += 1; // 1 cycle: completion interrupt
        return cycles;
    }
    void processIRQ()
    {
        lock_guard<mutex> lk(mtx);
        while (!irqQueue.empty())
        {
            irqQueue.front()();
            irqQueue.pop();
        }
    }
};
// ■■ Benchmark ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ int main() {
int main()
{
    const size_t N = 1024; // 1 KB
    vector<uint8_t> srcBuf(N), dstBuf;
    iota(srcBuf.begin(), srcBuf.end(), 0);
    DeviceController ctrl;
    long totalPoll = 0, totalIRQ = 0;
    std::cout << "=== I/O Hardware Mechanisms (1 KB transfer) ===\n\n"; // ■■ Polling
    std::cout << "[POLLING] Transferring " << N << " bytes...\n";
    for (auto b : srcBuf)
        totalPoll += ctrl.pollingIO(b);
    std::cout << " CPU cycles used: " << totalPoll << "\n\n";
    // ■■ Interrupt-driven
    std::cout << "[INTERRUPT] Transferring " << N << " bytes...\n";
    for (auto b : srcBuf)
        totalIRQ += ctrl.interruptDrivenIO(b, [&]
                                           { 
/* ISR: wake blocked process */ });
    std::cout << " CPU cycles used: " << totalIRQ << "\n\n";
    // ■■ DMA
    std::cout << "[DMA] Transferring " << N << " bytes...\n";
    long dmaC = ctrl.dmaTransfer(dstBuf, srcBuf);
    std::cout << " CPU cycles used: " << dmaC << "\n\n";
    // ■■ Summary
    std::cout << "=== CPU Cycles Comparison ===\n";
    std::cout << " Polling: " << totalPoll << " cycles\n";
    std::cout << " Interrupt-driven: " << totalIRQ << " cycles\n";
    std::cout << " DMA: " << dmaC << " cycles\n";
    std::cout << " DMA speedup over polling: "
              << totalPoll / (double)dmaC << "x\n";
    return 0;
}
