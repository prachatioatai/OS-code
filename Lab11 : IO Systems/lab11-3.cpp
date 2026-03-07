#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <memory>
#include <mutex>
#include <future>
#include <functional>
#include <stdexcept>
#include <vector>
#include <thread>
#include <chrono>
using namespace std;
using ms = chrono::milliseconds;
// ■■ BlockDevice ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ class BlockDevice {
class BlockDevice
{
    static const size_t BS = 512;
    vector<vector<uint8_t>> sectors;
    bool open_ = false;

public:
    explicit BlockDevice(size_t n) : sectors(n, vector<uint8_t>(BS, 0)) {}
    void open()
    {
        open_ = true;
        std::cout << "[BLK] opened\n";
    }
    void close()
    {
        open_ = false;
        std::cout << "[BLK] closed\n";
    }
    vector<uint8_t> readSector(size_t i)
    {
        if (!open_)
            throw runtime_error("not open");
        std::cout << "[BLK] read sector " << i << "\n";
        return sectors.at(i);
    }
    void writeSector(size_t i, const vector<uint8_t> &d)
    {
        if (!open_)
            throw runtime_error("not open");
        sectors.at(i) = d;
        std::cout << "[BLK] wrote " << d.size() << " bytes to sector " << i << "\n";
    }
};
// ■■ CharDevice ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ class CharDevice {
class CharDevice
{
    queue<char> buf;
    mutex mtx;
    bool open_ = false;

public:
    void open()
    {
        open_ = true;
        std::cout << "[CHR] opened\n";
    }
    void close()
    {
        open_ = false;
        std::cout << "[CHR] closed\n";
    }
    void inject(const string &s)
    {
        lock_guard<mutex> lk(mtx);
        for (char c : s)
            buf.push(c);
    }
    char getChar()
    {
        lock_guard<mutex> lk(mtx);
        if (buf.empty())
            return '\0';
        char c = buf.front();
        buf.pop();
        std::cout << "[CHR] getChar='" << c << "'\n";
        return c;
    }
    bool putChar(char c)
    {
        if (!open_)
            return false;
        std::cout << "[CHR] putChar='" << c << "'\n";
        return true;
    }
};
// ■■ NetworkSocket ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ class NetworkSocket {
class NetworkSocket
{
    string ip;
    int port;
    queue<string> recvQ;
    mutex mtx;
    bool open_ = false;
    int latencyMs = 5;

public:
    NetworkSocket(string ip_, int p) : ip(move(ip_)), port(p) {}
    void open()
    {
        open_ = true;
        std::cout << "[NET] connected to " << ip << ":" << port << "\n";
    }
    void close()
    {
        open_ = false;
        std::cout << "[NET] disconnected\n";
    }
    void injectPacket(const string &pkt)
    {
        lock_guard<mutex> lk(mtx);
        recvQ.push(pkt);
    }
    // MODE 1: Blocking — waits until data arrives
    string blockingRead()
    {
        std::cout << "[NET/BLOCK] waiting for data...\n";
        while (true)
        {
            lock_guard<mutex> lk(mtx);
            if (!recvQ.empty())
            {
                string s = recvQ.front();
                recvQ.pop();
                std::cout << "[NET/BLOCK] got: '" << s << "'\n";
                return s;
            }
        }
    }
    // MODE 2: Non-blocking — returns empty string if no data
    string nonBlockingRead()
    {
        lock_guard<mutex> lk(mtx);
        if (recvQ.empty())
        {
            cout << "[NET/NB] EAGAIN — no data available\n";
            return "";
        }
        string s = recvQ.front();
        recvQ.pop();
        cout << "[NET/NB] got: '" << s << "'\n";
        return s;
    }
    // MODE 3: Asynchronous — callback invoked on completion
    void asyncRead(function<void(string)> callback)
    {
        cout << "[NET/ASYNC] I/O submitted — CPU free\n";
        async(launch::async, [this, cb = move(callback)]()
              { 
this_thread::sleep_for(ms(latencyMs)); // simulate latency 
lock_guard<mutex> lk(mtx);
string data = recvQ.empty() ? "" : (recvQ.front(), recvQ.pop(), recvQ.empty() ? "" : ""); // simplified: inject and immediately callback 
cb("async-response"); })
            .wait();
    }
    bool send(const string &data)
    {
        if (!open_)
            return false;
        cout << "[NET] sent: '" << data << "'\n";
        return true;
    }
};
// ■■ main ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ int main() {
int main()
{
    std::cout << "=== Application I/O Interface Demo ===\n\n";
    // Block device
    BlockDevice disk(64);
    disk.open();
    vector<uint8_t> sector(512, 0xAB);
    disk.writeSector(0, sector);
    auto rd = disk.readSector(0);
    std::cout << " First byte: 0x" << std::hex << (int)rd[0] << std::dec << "\n\n";
    // Char device
    CharDevice kbd;
    kbd.open();
    kbd.inject("Hello!");
    for (int i = 0; i < 6; i++)
        kbd.getChar();
    cout << "\n";
    // Network — three I/O modes
    NetworkSocket sock("10.0.0.1", 443);
    sock.open();
    // non-blocking (empty)
    sock.nonBlockingRead();
    // inject then blocking
    sock.injectPacket("HTTP/1.1 200 OK");
    sock.blockingRead();
    // async
    sock.asyncRead([](const string &s)
                   { cout << "[NET/ASYNC] callback: '" << s << "'\n"; });
    sock.close();
    kbd.close();
    disk.close();
    return 0;
}
