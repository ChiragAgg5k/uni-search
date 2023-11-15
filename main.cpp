#include <iostream>
#include "include/auth.h"
#include "include/trie.h"
#include "include/dashboard.h"
#include <unistd.h>

#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"

std::string authenticate() {
    std::cout << "Do you have an account? (y/n): ";
    char answer;
    std::cin >> answer;

    if (answer != 'y' && answer != 'n') {
        std::cout << RED << "Invalid input... try again." << RESET << std::endl;
        std::cout << "--------------------------" << std::endl;
        return "";
    }

    std::string email;
    std::cout << "Enter email: ";
    std::cin >> email;

    if (!emailValidation(email)) {
        std::cout << "--------------------------" << std::endl;
        return "";
    }

    turnOffEcho();

    std::string password;
    std::cout << "Enter password: ";
    std::cin >> password;
    std::cout << std::endl;

    turnOnEcho();

    if (answer == 'y') {
        std::string userID = login(email, password);
        if (userID.empty()) {
            std::cout << "Invalid email or password" << std::endl;
        } else {
            std::cout << GREEN << "Login successful!" << RESET << std::endl;
            sleep(1);
            std::cout << "\n\n\n" << std::endl;
            return userID;
        }
    } else {
        std::string userID = createAccount(email, password);
        return userID;
    }

    return "";
}

int main() {

    system("clear");
    std::cout << std::endl;
    std::cout << YELLOW << "--------------------------" << RESET << std::endl;
    std::cout << YELLOW << "  Welcome to UniSearch!" << RESET << std::endl;
    std::cout << YELLOW << "--------------------------" << RESET << std::endl;
    std::cout << std::endl;

    std::string userID;
    while (userID.empty()) {
        userID = authenticate();
    }

    if (userID.empty()) {
        std::cout << YELLOW << "--------------------------" << RESET << std::endl;
        return 0;
    }

    showDashboard(userID);

    return 0;
}
