#pragma once

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

void loadMenu() {
    printf(" 1: View Passwords\n");
    printf(" 2: Modify Database\n");
    printf(" 3: Search a entry\n");
    printf("99: Lock Vault\n");
}

void databaseModMenu() {
    printf(" 1: Add a entry\n");
    printf(" 2: Modify a entry\n");
    printf(" 3: Remove a entry\n");
    printf("99: Back\n");
}

int handleMenu() {
    printf("> ");

    string choice;
    std::getline(std::cin,choice);

    for (char c : choice) {
        if (!isdigit(c)) {
            return -99;
        }
    }

    return std::stoi(choice);
}

inline int searchPrompt() {
    printf("### Choose your search field ###\n\n");
    printf("[1] Identifier [2] Website [3] Username [99] Back\n");
    return handleMenu();
}