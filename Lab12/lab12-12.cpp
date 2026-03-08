#include <iostream>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>
#include <iomanip>
using namespace std;
namespace fs = std::filesystem;
 
string getPerms(fs::perms p) {
    auto check = [&](fs::perms bit, char c) {
        return (p & bit) != fs::perms::none ? c : '-';
    };
    string s;
    s += check(fs::perms::owner_read,  'r');
    s += check(fs::perms::owner_write, 'w');
    s += check(fs::perms::owner_exec,  'x');
    s += check(fs::perms::group_read,  'r');
    s += check(fs::perms::group_write, 'w');
    s += check(fs::perms::group_exec,  'x');
    s += check(fs::perms::others_read, 'r');
    s += check(fs::perms::others_write,'w');
    s += check(fs::perms::others_exec, 'x');
    return s;
}
 
// (a) List directory
void listDir(const string& path) {
    cout << left << setw(12) << "PERM" << setw(10) << "SIZE(B)"
         << "NAME" << endl << string(40, '-') << endl;
    for (auto& e : fs::directory_iterator(path)) {
        auto st = e.status();
        uintmax_t sz = e.is_regular_file() ? e.file_size() : 0;
        cout << setw(12) << getPerms(st.permissions())
             << setw(10) << sz
             << e.path().filename().string() << endl;
    }
}
 
// (b) Copy file with enforcement
void copyFile(const string& src, const string& dst) {
    struct stat info;
    stat(src.c_str(), &info);
    // (c) Enforce read-only check
    if (!(info.st_mode & S_IRUSR)) {
        cerr << "Error: No read permission on source." << endl;
        return;
    }
    try {
        fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
        cout << "Copied: " << src << " -> " << dst << endl;
    } catch (const fs::filesystem_error& e) {
        cerr << "Copy failed: " << e.what() << endl;
    }
}
 
int main() {
    // Create demo files
    ofstream("file_a.txt") << "Hello World";
    ofstream("file_b.txt") << "OSC Chapter 13";
    chmod("file_b.txt", 0444);  // Make read-only
 
    cout << "=== Directory Listing ===" << endl;
    listDir(".");
    cout << endl;
    copyFile("file_a.txt", "file_a_copy.txt");
    copyFile("file_b.txt", "file_b_copy.txt");
    return 0;
}
