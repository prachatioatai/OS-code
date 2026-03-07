#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstring> // เพิ่ม header นี้สำหรับ memcpy

using namespace std;
using hrc = chrono::high_resolution_clock;

// ■■ Metrics: โครงสร้างเก็บผลลัพธ์การทดสอบ ■■■■■■■■■■■■■■■
struct Metrics {
    string testName;
    double throughputMBps;
    double latencyMs;
    double iops;
    long long totalOps;
    long long totalBytes;

    void print() const {
        cout << left << setw(28) << testName
             << right << fixed << setprecision(2)
             << setw(10) << throughputMBps << " MB/s"
             << setw(10) << latencyMs << " ms"
             << setw(12) << (long)iops << " IOPS" << endl;
    }
};

// ■■ Simulated Disk: จำลองการอ่านข้อมูลจาก Disk ■■■■■■■■■■
class SimDisk {
    static const size_t SECT = 512;
    vector<vector<uint8_t>> sectors;
    size_t total, head = 0;
    double seekMsPerTrack; // มิลลิวินาที ต่อการเลื่อน 100 sectors

public:
    SimDisk(size_t n, double seekMs = 0.1)
        : sectors(n, vector<uint8_t>(SECT, 0)),
          total(n), seekMsPerTrack(seekMs) {}

    double sequentialRead(size_t count) {
        auto t0 = hrc::now();
        for (size_t i = 0; i < count && i < total; ++i) {
            head = i;
            volatile uint8_t x = sectors[i][0]; (void)x; // อ่านหลอกๆ
        }
        auto t1 = hrc::now();
        return chrono::duration<double>(t1 - t0).count();
    }

    double randomRead(size_t count) {
        srand(42);
        auto t0 = hrc::now();
        for (size_t i = 0; i < count; ++i) {
            size_t tgt = rand() % total;
            
            // คำนวณเวลาที่ใช้ในการเลื่อนหัวอ่าน (Seek Time)
            double seekSec = abs((long long)tgt - (long long)head) * (seekMsPerTrack / 100.0) / 1000.0;
            
            // จำลองการรอ (Scale ลงเพื่อให้โปรแกรมไม่ช้าเกินไป)
            auto deadline = hrc::now() + chrono::duration<double>(seekSec * 0.01); 
            while (hrc::now() < deadline);
            
            head = tgt;
            volatile uint8_t x = sectors[tgt][0]; (void)x;
        }
        auto t1 = hrc::now();
        return chrono::duration<double>(t1 - t0).count();
    }
};

// ■■ Buffer Benchmark: ทดสอบประสิทธิภาพของ Buffer Size ■■■■
class BufferBench {
public:
    static Metrics run(size_t bufSize, size_t totalBytes) {
        vector<uint8_t> src(totalBytes), dst(totalBytes);
        iota(src.begin(), src.end(), 0);

        auto t0 = hrc::now();
        long long ops = 0;
        for (size_t off = 0; off < totalBytes; off += bufSize) {
            size_t n = min(bufSize, totalBytes - off);
            memcpy(dst.data() + off, src.data() + off, n);
            ++ops;
        }
        auto t1 = hrc::now();
        double dur = chrono::duration<double>(t1 - t0).count();

        Metrics m;
        string unit = (bufSize >= 1024) ? " KB" : " B";
        size_t displaySize = (bufSize >= 1024) ? bufSize / 1024 : bufSize;
        m.testName = "Buffer " + to_string(displaySize) + unit;
        m.totalOps = ops;
        m.totalBytes = totalBytes;
        m.throughputMBps = (totalBytes / (1024.0 * 1024.0)) / dur;
        m.latencyMs = (dur / ops) * 1000.0;
        m.iops = ops / dur;
        return m;
    }
};

// ■■ I/O Scheduler: จำลองอัลกอริทึมจัดการหัวอ่าน ■■■■■■■■■■
class IOScheduler {
public:
    static long long fcfs(vector<int> reqs, int start) {
        long long dist = 0; int cur = start;
        for (int r : reqs) { dist += abs(r - cur); cur = r; }
        return dist;
    }

    static long long sstf(vector<int> reqs, int start) {
        long long dist = 0; int cur = start;
        vector<int> pending = reqs;
        while (!pending.empty()) {
            auto it = min_element(pending.begin(), pending.end(),
                [&](int a, int b) { return abs(a - cur) < abs(b - cur); });
            dist += abs(*it - cur);
            cur = *it;
            pending.erase(it);
        }
        return dist;
    }
};

// ■■ Performance Report: ฟังก์ชันพิมพ์รายงาน ■■■■■■■■■■■■■
void printReport(const vector<Metrics>& results) {
    cout << "\n" << string(70, '=') << "\n";
    cout << "              I/O PERFORMANCE REPORT\n";
    cout << string(70, '=') << "\n";
    cout << left << setw(28) << " Test"
         << right << setw(15) << "Throughput"
         << setw(13) << "Latency"
         << setw(13) << "IOPS" << "\n";
    cout << string(70, '-') << "\n";
    for (const auto& m : results) m.print();
    cout << string(70, '=') << "\n";

    auto best = max_element(results.begin(), results.end(),
        [](const Metrics& a, const Metrics& b) { return a.throughputMBps < b.throughputMBps; });
    
    cout << " Best Performance:  " << best->testName << " (" << fixed << setprecision(1) << best->throughputMBps << " MB/s)\n";
}

// ■■ main ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
int main() {
    vector<Metrics> results;
    const size_t TOTAL_DATA = 8 * 1024 * 1024; // 8 MB
    const size_t DISK_OPS = 1000;

    // 1. ทดสอบ Sequential vs Random Read
    SimDisk disk(20000);
    {
        double dur = disk.sequentialRead(DISK_OPS);
        Metrics m; m.testName = "Seq Read (Simulated Disk)";
        m.totalOps = DISK_OPS; m.totalBytes = DISK_OPS * 512;
        m.throughputMBps = (m.totalBytes / (1024.0 * 1024.0)) / dur;
        m.latencyMs = (dur / DISK_OPS) * 1000.0; m.iops = DISK_OPS / dur;
        results.push_back(m);
    }
    {
        double dur = disk.randomRead(DISK_OPS);
        Metrics m; m.testName = "Rand Read (Simulated Disk)";
        m.totalOps = DISK_OPS; m.totalBytes = DISK_OPS * 512;
        m.throughputMBps = (m.totalBytes / (1024.0 * 1024.0)) / dur;
        m.latencyMs = (dur / DISK_OPS) * 1000.0; m.iops = DISK_OPS / dur;
        results.push_back(m);
    }

    // 2. ทดสอบผลกระทบของ Buffer Size
    for (size_t bs : {512UL, 4096UL, 65536UL, 1048576UL}) {
        results.push_back(BufferBench::run(bs, TOTAL_DATA));
    }

    printReport(results);

    // 3. เปรียบเทียบ Disk Scheduling
    vector<int> requests = {98, 183, 37, 122, 14, 124, 65, 67};
    int startHead = 53;
    long long fcfsDist = IOScheduler::fcfs(requests, startHead);
    long long sstfDist = IOScheduler::sstf(requests, startHead);

    cout << "\n=== Disk Scheduling Analysis (Start Head=" << startHead << ") ===\n";
    cout << " Queue: 98, 183, 37, 122, 14, 124, 65, 67\n";
    cout << " FCFS Total Seek Distance: " << fcfsDist << " tracks\n";
    cout << " SSTF Total Seek Distance: " << sstfDist << " tracks\n";
    cout << " SSTF Improvement: " << fixed << setprecision(1) 
         << 100.0 * (fcfsDist - sstfDist) / fcfsDist << "% more efficient\n";

    return 0;
}
