#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;
 
void traverseDirectory(const fs::path& dirPath, int depth = 0) {
    std::string indent(depth * 2, ' ');
    try {
        for (const auto& entry : fs::directory_iterator(dirPath)) {
            if (entry.is_directory()) {
                cout << indent << "[DIR]  " << entry.path().filename().string() << "/" << endl;
                traverseDirectory(entry.path(), depth + 1);  // Recurse
            } else {
                cout << indent << "[FILE] " << entry.path().filename().string()
                     << " (" << entry.file_size() << " B)" << endl;
            }
        }
    } catch (const fs::filesystem_error& e) {
        cout << indent << "  [Permission denied: " << e.what() << "]" << endl;
    }
}
 
int main() {
    traverseDirectory(".");
    return 0;
}
