//
// Created by Chirag Aggarwal on 21/08/23.
//

#ifndef UNISEARCH_AUTH_H
#define UNISEARCH_AUTH_H

#include <string>

void turnOffEcho();

void turnOnEcho();

bool emailValidation(const std::string &email);

std::string createAccount(const std::string &email, std::string password);

std::string login(const std::string &email, const std::string &password);


#endif //UNISEARCH_AUTH_H
