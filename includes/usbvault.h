#pragma once

#include <thread>
#include <chrono>
#include <limits>
#include <algorithm>

#define TIMEOUT(x) std::this_thread::sleep_for(std::chrono::seconds(x));

#define VAULT_PATH "data/vault.dat"
#define SHADOW_PATH "data/shadow.dat"

inline void waitForEnter() {
    std::cout << "\nPress Enter to continue...";

    std::string line;
    std::getline(std::cin, line);
}

struct DatabaseStatistics {
    int entries;
    bool auth;
};

void loadBanner(DatabaseStatistics& stats) {
    std::cout << "###################################" << std::endl;
        std::cout << "         PASSWORD KEEPER"            << std::endl;
        std::cout << "  By: Jacob Dawes - Version 1.0.0"   << std::endl;
        std::cout << std::endl;
        std::cout << "  Entries    " << std::to_string(stats.entries) << std::endl;
        std::cout << "  Status     " << (stats.auth == true ? "DECRYPTED" : "ENCRYPTED") << std::endl;
        std::cout << "###################################" << std::endl;
}

inline bool checkNumber(const std::string& choice) {
    for (char c : choice) {
        if (!std::isdigit(c)) {
            std::cout << "*Invalid integer choice.*" << std::endl;
            TIMEOUT(1);
            return false;
        }
    }
    return true;
}
