#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

#include "usbvault.h"
#include "menu.h"
#include "cypher.h"

#define MAX_BUFFER_LEN 1024

#define INVALID -1

#ifdef _WIN32
#define CLEAR_S system("cls");
#else
#define CLEAR_S system("clear");
#endif

using std::cout;
using std::endl;
using std::string;
using std::vector;

struct PasswordEntity {
    int id;
    string website;
    string user;
    string password;
};

inline void viewUserPasswords(std::vector<PasswordEntity>& database) {
    CLEAR_S;

    const int WIDTH = 20;

    cout << std::left;

    // header
    cout << "| "
         << std::setw(5)  << "ID"       << " | "
         << std::setw(WIDTH) << "WEBSITE" << " | "
         << std::setw(WIDTH) << "USER"    << " | "
         << std::setw(WIDTH) << "PASSWORD"<< " |"
         << endl;

    cout << string(75, '-') << endl;

    // rows
    for (const PasswordEntity& entry : database) {

        cout << "| "
             << std::setw(5)  << entry.id       << " | "
             << std::setw(WIDTH) << entry.website  << " | "
             << std::setw(WIDTH) << entry.user     << " | "
             << std::setw(WIDTH) << entry.password << " |"
             << endl;
    }
}

inline void addUserPassword(std::vector<PasswordEntity>& database) {
    string website,user,pass;
    int ident = database.size();
    PasswordEntity e;
    CLEAR_S;

    e.id = ident;
    std::cout << "Website: ";
    getline(std::cin,e.website);
    std::cout << "Username: ";
    getline(std::cin,e.user);
    std::cout << "Password: ";
    getline(std::cin,e.password);

    database.push_back(e);
    return;
}

/*
    Example format:

    1:google.com:jacob:mypassword
    2:github.com:user123:testpass
*/

inline vector<PasswordEntity> entityParser(const string& content,const string& password) {

    vector<PasswordEntity> entities;

    std::stringstream stream(decrypt(content,password));
    string line;

    while (std::getline(stream, line)) {

        if (line.empty()) {
            continue;
        }

        std::stringstream lineStream(line);

        PasswordEntity entity;

        string idStr;

        std::getline(lineStream, idStr, ':');
        std::getline(lineStream, entity.website, ':');
        std::getline(lineStream, entity.user, ':');
        std::getline(lineStream, entity.password, ':');

        entity.id = std::stoi(idStr);

        entities.push_back(entity);
    }

    return entities;
}

std::vector<PasswordEntity> searchEntry(int c, std::vector<PasswordEntity>& database) {
    int temp = 0;

    std::vector<PasswordEntity> searchResults;
    PasswordEntity e;

    e.id = -1;

    std::string input = "";

    std::cout << "Enter search: ";
    std::getline(std::cin, input);

    switch (c) {

        case 1: { // id

            searchResults.push_back(e);

            if (!checkNumber(input)) {
                std::cout << "* Invalid format for this field. *";
                return searchEntry(c, database);
            }

            temp = std::stoi(input);

            for (PasswordEntity ent : database) {
                if (ent.id == temp) {
                    searchResults.clear();
                    searchResults.push_back(ent);
                }
            }

            return searchResults;
        }

        case 2: { // website

            searchResults.clear();

            for (PasswordEntity ent : database) {
                if (ent.website == input) {
                    searchResults.push_back(ent);
                }
            }

            return searchResults;
        }

        case 3: { // username

            searchResults.clear();

            for (PasswordEntity ent : database) {
                if (ent.user == input) {
                    searchResults.push_back(ent);
                }
            }

            return searchResults;
        }

        case 99: {
            PasswordEntity e;
            e.id = -99;
            searchResults.clear();
            searchResults.push_back(e);
            return searchResults;
        }

        default: {
            return searchResults;
        }
    }
}

inline int databaseWriter(
    const string& link,std::vector<PasswordEntity>& db,
    const std::string& password
)
{
    string content = "";
    std::ofstream t_stream(link);
    string inter = "";

    if (!t_stream) return INVALID;
    
    for (PasswordEntity entity : db) {
        inter += std::to_string(entity.id) + ":";
        inter += entity.website + ":";
        inter += entity.user + ":";
        inter += entity.password + "\n";

        content += inter;
        inter = "";
    }

    // encryption
    std::vector<unsigned char> out = encrypt(content,password);
    content.clear();
    for (unsigned char c : out) {
        content += c;
    }

    t_stream << content;
    t_stream.close();

    return 0;
}

inline string databaseReader(const string& link,const string& password) {

    std::ifstream file(link);
    string content = "";

    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;

    buffer << file.rdbuf();
    content = buffer.str();

    file.close();

    return decrypt(content,password);
}

string getMasterPass() {
    std::ifstream ifile("data/shadow.dat");
    string key = "";
    if (!ifile.is_open()) return "";
    ifile >> key;
    return key;
}

void modifyDatabase(int id,std::vector<PasswordEntity>& db) {
    string web = "";
    string user = "";
    string pass = "";

    cout << "Enter new Website [skip]: ";
    std::getline(std::cin,web);
    cout << "Enter new User [skip]: ";
    std::getline(std::cin,user);
    cout << "Enter new password [skip]: ";
    std::getline(std::cin,pass);

    if(!web.empty()) db[id].website = web;
    if(!user.empty()) db[id].user = user;
    if(!pass.empty()) db[id].password = pass;

    cout << "[+] Entry modified. - " << id << endl;
    TIMEOUT(1);
    return;
}
