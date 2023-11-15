//
// Created by Chirag Aggarwal on 11/11/23.
//

#include "../include/dashboard.h"
#include <iostream>
#include <sqlite3.h>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <vector>


namespace fs = std::filesystem;

#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"

sqlite3 *db;

int base_amount = 120000;

void clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

// return (bool, int) for (status, amount_paid)
std::vector<int> checkFeeStatus(const std::string &userID) {
    std::string query = "SELECT * FROM fees WHERE user_id = ?";
    sqlite3_stmt *stmt;

    int amount_paid = 0;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, userID.c_str(), -1, SQLITE_STATIC);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            amount_paid += sqlite3_column_int(stmt, 2);
        }

        std::cout << "You have paid Rs. " << amount_paid << " till now." << std::endl;

        if (amount_paid >= base_amount) {
            std::cout << GREEN << "You have already paid your fees for this semester." << RESET << std::endl;
            std::cout << "--------------------------" << std::endl;
            return {true, amount_paid};
        }

        std::cout << "You have to pay Rs. " << base_amount - amount_paid << " more." << std::endl;
    }

    return {false, amount_paid};
}

bool payFees(const std::string &userID) {
    std::vector<int> status = checkFeeStatus(userID);
    bool paid = status[0];
    int amount_paid = status[1];

    if (paid) {
        return false;
    }

    int curr_payment = 0;
    std::cout << "Enter amount to pay: ";
    std::cin >> curr_payment;

    if (curr_payment < 0) {
        std::cout << "Invalid amount." << std::endl;
        return false;
    }

    if (curr_payment + amount_paid > base_amount) {
        std::cout << RED << "You can only pay Rs. " << base_amount - amount_paid << " more." << RESET << std::endl;
        std::cout << "--------------------------" << std::endl;
        return false;
    }

    std::string query = "INSERT INTO fees (user_id, amount_paid, time) VALUES (?, ?, ?)";
    sqlite3_stmt *stmt;

    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    std::string date = std::to_string(now->tm_year + 1900) + "-" + std::to_string(now->tm_mon + 1) + "-" +
                       std::to_string(now->tm_mday);

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, userID.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, curr_payment);
        sqlite3_bind_text(stmt, 3, date.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cout << "Error paying fees." << std::endl;
            return false;
        }
    }

    sqlite3_finalize(stmt);

    std::cout << GREEN << "Payment successful!" << RESET << std::endl;
    std::cout << "--------------------------" << std::endl;
    return true;
}

void printReceipt(const std::string &userID) {
    clearScreen();

    std::cout << YELLOW << "--------------------------" << std::endl;
    std::cout << "UniSearch - Fee Receipt" << std::endl;
    std::cout << "--------------------------" << RESET << std::endl;

    std::string query = "SELECT * FROM fees WHERE user_id = ?";
    sqlite3_stmt *stmt;

    int total_amount_paid = 0;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, userID.c_str(), -1, SQLITE_STATIC);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string date = (char *) sqlite3_column_text(stmt, 3);
            int amount_paid = sqlite3_column_int(stmt, 2);

            total_amount_paid += amount_paid;

            std::cout << "Date: " << date << std::endl;
            std::cout << "Amount Paid: " << amount_paid << std::endl;
            std::cout << "--------------------------" << std::endl;
        }
    }

    std::cout << "Total Amount Paid: " << total_amount_paid << std::endl;
    std::cout << GREEN << "--------------------------" << RESET << std::endl;

    clearScreen();
}

void showDashboard(const std::string &userID) {
    clearScreen();

    int rc = sqlite3_open("../database.db", &db);

    if (rc) {
        throw std::runtime_error("Can't open database");
    }

    int choice = 0;
    while (choice != 4) {
        std::cout << std::endl;
        std::cout << YELLOW "--------------------------" << std::endl;
        std::cout << "  UniSearch - Dashboard" << std::endl;
        std::cout << "--------------------------" << RESET << std::endl;
        std::cout << std::endl;

        std::cout << "Choose from the following options:" << std::endl;
        std::cout << "1. Finance Department" << std::endl;
        std::cout << "2. Academics Department" << std::endl;
        std::cout << "3. Medical Department" << std::endl;
        std::cout << "4. Exit\n" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cout << std::endl;

        switch (choice) {
            case 1:
                showFinanceDashboard(userID);
                break;
            case 2:
                showAcademicDashboard(userID);
                break;
            case 3:
                showMedicalDashboard(userID);
                break;
            case 4:
                std::cout << std::endl;
                std::cout << "--------------------------------" << std::endl;
                std::cout << GREEN << "Thank you for using UniSearch!" << RESET << std::endl;
                std::cout << "--------------------------------" << std::endl;
                break;
            default:
                std::cout << RED << "Invalid choice... try again." << RESET << std::endl;
                break;
        }
    }
}

void printAllMedicines() {
    std::cout << std::endl;
    std::cout << GREEN << "--------------------------" << std::endl;
    std::cout << "   UniSearch - Medicines" << std::endl;
    std::cout << "--------------------------" << RESET << std::endl;
    std::cout << std::endl;

    std::string query = "SELECT * FROM medicines";
    sqlite3_stmt *stmt;

    std::cout << "ID\tName\t\t\tTreatment\tPrice\t\tIn Stock" << std::endl;
    std::cout << "------------------------------------------------------------------------------------" << std::endl;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string mid = (char *) sqlite3_column_text(stmt, 0);
            std::string name = (char *) sqlite3_column_text(stmt, 1);
            std::string treatment = (char *) sqlite3_column_text(stmt, 2);

            int price = sqlite3_column_int(stmt, 3);
            std::string in_stock = (char *) sqlite3_column_text(stmt, 4);

            std::cout << mid << "\t" << name << "\t\t\t" << treatment << "\t" << price << "\t\t" << in_stock
                      << std::endl;
        }

        std::cout << "------------------------------------------------------------------------------------"
                  << std::endl;
    }

    sqlite3_finalize(stmt);

    std::cout << "\n\n\n" << std::endl;
}

void requestMedicine(const std::string &userID) {
    std::string query = "INSERT INTO requests (user_id, mid, time) VALUES (?, ?, ?)";
    sqlite3_stmt *stmt;

    std::string medicineID;
    std::cout << "Enter medicine ID (or 0 to go back): ";
    std::cin >> medicineID;

    if (medicineID == "0") {
        return;
    }

    // check if mid exists
    std::string query2 = "SELECT * FROM medicines WHERE mid = ?";
    sqlite3_stmt *stmt2;

    if (sqlite3_prepare_v2(db, query2.c_str(), -1, &stmt2, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt2, 1, medicineID.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt2) != SQLITE_ROW) {
            std::cout << "Invalid medicine ID." << std::endl;
            return;
        }
    }

    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    std::string date = std::to_string(now->tm_year + 1900) + "-" + std::to_string(now->tm_mon + 1) + "-" +
                       std::to_string(now->tm_mday);

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, userID.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, medicineID.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, date.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cout << "Error requesting medicine." << std::endl;
            return;
        }
    }

    sqlite3_finalize(stmt);

    std::cout << std::endl;
    std::cout << "--------------------------" << std::endl;
    std::cout << GREEN << "Medicine requested successfully!" << RESET << std::endl;
    std::cout << "--------------------------" << std::endl;

}

void showMedicalDashboard(const std::string &userID) {
    clearScreen();

    int choice = -1;
    while (choice != 3) {

        std::cout << std::endl;
        std::cout << YELLOW << "---------------------------------------" << std::endl;
        std::cout << "   UniSearch - Medical Department" << std::endl;
        std::cout << "---------------------------------------" << RESET << std::endl;
        std::cout << std::endl;

        std::cout << "Choose from the following options:" << std::endl;
        std::cout << "1. View all medicines" << std::endl;
        std::cout << "2. Request a medicine" << std::endl;
        std::cout << "3. Back" << std::endl;
        std::cout << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cout << std::endl;

        switch (choice) {
            case 1:
                printAllMedicines();
                break;
            case 2:
                requestMedicine(userID);
                break;
            case 3:
                clearScreen();
                return;
            default:
                std::cout << RED << "Invalid choice... try again." << RESET << std::endl;
                break;
        }
    }
}

void showAcademicDashboard(const std::string &userID) {
    clearScreen();


    std::cout << YELLOW << "--------------------------" << std::endl;
    std::cout << "UniSearch - Academics Department" << std::endl;
    std::cout << "--------------------------" << RESET << std::endl;

    std::string path = "../data/";
    std::cout << "Choose a subject from the following:" << std::endl;
    int i = 1;
    for (const auto &entry: fs::directory_iterator(path)) {
        std::string subject = entry.path().filename();
        subject = subject.substr(0, subject.length() - 4);
        std::cout << i << ". " << subject << std::endl;
        i++;
    }
    std::cout << "0. Back" << std::endl;
    std::cout << std::endl;

    int choice = -1;
    while (choice != 0) {
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cout << std::endl;

        switch (choice) {
            case 1:
                printFile("../data/CSET240 - Probability.txt");
                break;
            case 2:
                printFile("../data/CSET205 - Software.txt");
                break;
            case 3:
                printFile("../data/CSET214 - Data Science.txt");
                break;
            case 4:
                printFile("../data/CSET203 - Micro.txt");
                break;
            case 5:
                printFile("../data/CSET202 - C++.txt");
                break;
            case 6:
                printFile("../data/CSET201 - IMS.txt");
                break;
            case 0:
                clearScreen();
                return;
            default:
                std::cout << RED << "Invalid choice... try again." << RESET << std::endl;
                break;
        }
    }
}

void printFile(const std::string &fileName) {
    std::cout << GREEN << "--------------------------" << std::endl;
    std::ifstream file(fileName);
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }
    std::cout << GREEN << "--------------------------" << std::endl;
    std::cout << RESET << std::endl;
}

void showFinanceDashboard(const std::string &userID) {
    clearScreen();

    int choice = 0;
    std::cout << "Choose from the following options:" << std::endl;
    while (choice != 4) {
        std::cout << YELLOW << "--------------------------" << std::endl;
        std::cout << "UniSearch - Finance Department" << std::endl;
        std::cout << "--------------------------" << RESET << std::endl;
        std::cout << "1. Pay Fees" << std::endl;
        std::cout << "2. View Fee Receipt" << std::endl;
        std::cout << "3. View Fee Status" << std::endl;
        std::cout << "4. Back\n" << std::endl;

        std::cout << "Enter your choice: ";

        std::cin >> choice;
        std::cout << std::endl;

        switch (choice) {
            case 1:
                payFees(userID);
                break;
            case 2:
                printReceipt(userID);
                break;
            case 3:
                checkFeeStatus(userID);
                break;
            case 4:
                return;
            default:
                std::cout << RED << "Invalid choice... try again." << RESET << std::endl;
                break;
        }
    }
}