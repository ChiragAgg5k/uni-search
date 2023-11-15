//
// Created by Chirag Aggarwal on 21/08/23.
//

#include "../include/auth.h"
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <sqlite3.h>
#include <regex>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"

void turnOffEcho() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void turnOnEcho() {
    struct termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    oldt.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

std::string generateUserID() {
    // Get the current time as a string
    time_t now = time(nullptr);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", localtime(&now));

    // Generate a random 4-digit number
    int randomNum = rand() % 10000;
    char randomStr[5];
    sprintf(randomStr, "%04d", randomNum);

    // Combine the timestamp and random number to create a unique ID
    std::string userID = timestamp + std::string(randomStr);
    return userID;
}

// Define a secret key for XOR encryption
const char xor_key = 0x42; // You can change this key

// Function to encrypt a string using XOR
std::string encrypt(const std::string &input) {
    std::string encrypted = input;
    for (char &c: encrypted) {
        c ^= xor_key;
    }
    return encrypted;
}

// Function to decrypt a string using XOR
std::string decrypt(const std::string &input) {
    return encrypt(input); // XOR decryption is the same as encryption
}

bool isValidEmail(const std::string &email) {
    // Regular expression pattern for a simple email validation
    const std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4})");

    return std::regex_match(email, pattern);
}

bool emailValidation(const std::string &email) {
    if (!isValidEmail(email)) {
        std::cout << RED << "Invalid email address. Please enter a valid email address" << RESET << std::endl;
        return false;
    }

    std::string domain = email.substr(email.find('@') + 1);
    if (domain != "bennett.edu.in") {
        std::cout << RED << "Only Bennett University email addresses are allowed" << RESET << std::endl;
        return false;
    }

    return true;
}

std::string createAccount(const std::string &email, std::string password) {

    if (password.length() < 6) {
        std::cout << RED << "Password must be at least 6 characters long" << RESET << std::endl;
        return "";
    }

    sqlite3 *db;
    int rc = sqlite3_open("../database.db", &db);

    if (rc) {
        throw std::runtime_error("Can't open database");
    }

    password = encrypt(password);

    // create table if not exists
    std::string sql = "CREATE TABLE IF NOT EXISTS users (id TEXT PRIMARY KEY, email TEXT UNIQUE, password TEXT);";
    char *zErrMsg = 0;
    rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        throw std::runtime_error("SQL error: " + std::string(zErrMsg));
    }

    std::string userID = generateUserID();

    std::string query = "INSERT INTO users (id, email, password) VALUES (?, ?, ?)";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, userID.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, password.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            if (strcmp(sqlite3_errmsg(db), "UNIQUE constraint failed: users.email") == 0) {
                std::cout << RED << "User already exists!" << RESET << std::endl;
            }
        } else {
            std::cout << GREEN << "Account created successfully!" << RESET << std::endl;
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return "";
}

std::string login(const std::string &email, const std::string &password) {
    sqlite3 *db;
    int rc = sqlite3_open("../database.db", &db);

    if (rc) {
        throw std::runtime_error("Can't open database");
    }

    std::string query = "SELECT * FROM users WHERE email = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string encryptedPassword = (char *) sqlite3_column_text(stmt, 2); // Password in Column 1
            std::string decryptedPassword = decrypt(encryptedPassword);

            if (decryptedPassword == password) {
                std::string user_id;
                user_id = (char *) sqlite3_column_text(stmt, 0); // User ID in Column 0

                sqlite3_finalize(stmt);
                sqlite3_close(db);

                return user_id;
            } else {
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                return "";
            }
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return "";
}