//
// Created by Chirag Aggarwal on 13/11/23.
//

#ifndef UNISEARCH_DASHBOARD_H
#define UNISEARCH_DASHBOARD_H

#include <string>
#include <sqlite3.h>

void showDashboard(const std::string &userID);

void showFinanceDashboard(const std::string &userID);

void showAcademicDashboard(const std::string &userID);

void showMedicalDashboard(const std::string &userID);

void printFile(const std::string &fileName);

#endif //UNISEARCH_DASHBOARD_H