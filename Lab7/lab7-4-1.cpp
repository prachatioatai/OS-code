#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <random>

class BankAccount {
private:
    double balance;
    std::mutex mtx;
    int accountId;
    
public:
    BankAccount(int id, double initial) 
        : accountId(id), balance(initial) {}
    
    // TODO: Implement safe transfer method
    // Use lock ordering or std::lock to prevent deadlock
    
    static bool transfer(BankAccount& from, BankAccount& to, double amount) {
        // Your code here
        
        
        
        
        return true;
    }
    
    double getBalance() {
        std::lock_guard<std::mutex> lock(mtx);
        return balance;
    }
    
    int getId() const { return accountId; }
};

int main() {
    // Create accounts
    std::vector<BankAccount> accounts;
    for (int i = 0; i < 5; i++) {
        accounts.emplace_back(i, 1000.0);
    }
    
    // Create threads that perform random transfers
    // TODO: Implement this
    
    return 0;
}
