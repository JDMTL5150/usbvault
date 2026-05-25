/*
    Main runner for the program that contains the
    main function.

    Author: Jacob Dawes
*/

#include <cstdlib>
#include <string>
#include <iostream>
#include <cctype>
#include <strings.h>

#include "../includes/usbvault.h"
#include "../includes/menu.h"
#include "../includes/passcontrol.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;

void flushcin() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int checkPassword(const string& password);
void createDatabase();
void createMasterKey();

int main() {
    signal(SIGINT, SIG_IGN);
    DatabaseStatistics stats;
    std::vector<PasswordEntity> database;
    bool isRunning = true;

    string envKey = getMasterPass();

    if (envKey.empty()) {
        cout << "MASTER_KEY not found in environment." << endl;
        cout << "### Create a master key ###" << endl;
        TIMEOUT(2);
        createMasterKey();
        envKey = getMasterPass();
    }

    // read into the file for password entries
    std::stringstream buff;
    std::ifstream inFile("data/vault.dat");

    // check if user has made a local database
    if(!inFile.is_open()) {
        cout << "\nNo database found in system." << endl;
        cout << "### Create your database ###" << endl;
        TIMEOUT(2);
        createDatabase();
    }

    buff << inFile.rdbuf();
    string content = buff.str();

    if (content.size() > 0) database = entityParser(content,envKey);

    stats.entries = database.size();

    // main loop of the program
    while (isRunning) {
        stats.entries = database.size();
        CLEAR_S;
        loadBanner(stats);
        cout << endl;

        if (!stats.auth) {
            cout << "Master Password: ";

            int check = checkPassword(envKey);

            if (check == 1) {
                cout << "* Invalid Password *" << endl;
                TIMEOUT(1);
                continue;
            }

            stats.auth = true;
        }

        CLEAR_S;

        loadBanner(stats);
        cout << endl;
        loadMenu();

        string choice;

        cout << "> ";
        getline(cin,choice);

        if (!checkNumber(choice)) {
            continue;
        }

        int selected = std::stoi(choice);

        switch (selected) {
            case 1: // View
                if(database.size() <= 0) {
                    cout << "[!] No Entries to view." << endl;
                    TIMEOUT(1);
                }
                else {
                    viewUserPasswords(database);
                    waitForEnter();
                }
                break;
            case 2: // db mod
                {
                    CLEAR_S;
                    loadBanner(stats);
                    cout << endl;
                    databaseModMenu();
                    int c = handleMenu();
                    bool isMod = true;

                    while (isMod) {

                        switch (c) {
                        case 1: // add entry
                            addUserPassword(database);
                            isMod = false;
                            break;
                        case 2: // modify entry
                            {
                                CLEAR_S;
                                if (database.size() <= 0) {
                                    cout << "* No entries found. *" << endl;
                                    TIMEOUT(1);
                                    isMod = false;
                                    break;
                                }
                                cout << "### Modify a entry ###" << endl;
                                TIMEOUT(1);
                                viewUserPasswords(database);
                                string id = "";
                                printf("Enter id to modify [back]: ");
                                std::getline(cin,id);

                                if (strcasecmp(id.c_str(),"back") == 0) {
                                    isMod = false;
                                    break;
                                }

                                if (!checkNumber(id)) {
                                    cout << "[!] Invalid formatting for this field." << endl;
                                    break;
                                }

                                int x = std::stoi(id);

                                if (x > database.size() || x < 0) {
                                    cout << "[!] Identifier entered is not found. - " << id << endl;
                                    break;
                                }

                                modifyDatabase(x,database);
                            }
                            break;
                        case 3: // remove entry
                            {
                                CLEAR_S;
                                if (database.size() <= 0) {
                                    cout << "* No entries found. *" << endl;
                                    TIMEOUT(1);
                                    isMod = false;
                                    break;
                                }
                                string id = "";
                                viewUserPasswords(database);
                                printf("Enter id to delete [back]: ");
                                std::getline(cin,id);

                                if (strcasecmp(id.c_str(),"back") == 0) {
                                    isMod = false;
                                    break;
                                }

                                if (!checkNumber(id)) {
                                    cout << "[!] Invalid formatting for this field." << endl;
                                    break;
                                }

                                int x = std::stoi(id);

                                if (x > database.size() || x < 0) {
                                    cout << "[!] Identifier entered is not found. - " << id << endl;
                                    break;
                                }

                                std::erase_if(database,[x](const PasswordEntity& e) {
                                    return e.id == x;
                                });

                                printf("[+] Entry Erased.");
                                TIMEOUT(1);

                                break;
                            }
                        case 99: // back
                            isMod = false;
                            break;
                        default:
                            cout << "* Invalid Choice *" << endl;
                            break;
                        }
                    }
                    break;
                }
            case 3: {
                CLEAR_S;
                if (database.size() <= 0) {
                    cout << "* No entries found. *" << endl;
                    TIMEOUT(1);
                    break;
                }
                std::vector<PasswordEntity> e;
                int c = searchPrompt();
                e = searchEntry(c,database);
                if (e.empty() || e[0].id == -1) {
                    cout << "[!] No entries found." << endl;
                    break;
                }
                else if (e[0].id == -99) {
                    break;
                }

                else {
                    viewUserPasswords(e);
                    waitForEnter();
                }
                break;
            }
            case 99:
                cout << "Locking Vault ..." << endl;
                TIMEOUT(2);
                databaseWriter(VAULT_PATH,database,envKey);
                stats.auth = false;
                break;
            default:
                cout << "*Invalid Choice.*" << endl;
                TIMEOUT(1);
                break;
        }
    }

    return 0;
}

int checkPassword(const string& password) {

    std::string pass;

    std::getline(cin,pass);

    if (password != sha256(pass)) {
        return 1;
    }

    return 0;
}

void createMasterKey() {
    CLEAR_S;
    cout << "Enter a master key: ";

    string key = "";
    getline(cin,key);

    std::ofstream env(SHADOW_PATH);

    env << sha256(key);
    env.close();
    cout << "Master Key created.  * DO NOT FORGET IT! *" << endl;
    cout << "this password cannot be changed later on." << endl;
    return;
}

void createDatabase() {
    CLEAR_S;
    std::cout << "Creating Database ..." << std::endl;

    TIMEOUT(2);

    std::ofstream out("data/vault.dat");
    out.close();

    std::cout << "[+] Database Created." << std::endl;
    return;
}
