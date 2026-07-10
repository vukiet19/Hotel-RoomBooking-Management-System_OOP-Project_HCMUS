#pragma once

#include <string>
#include <QString>

class QTableWidget;

class Backend
{
public:
    static bool checkValidUsername(const std::string &g);
    static bool checkValidPassword(const std::string &g, const std::string &p);
    static void loadTableData(QTableWidget *table, const QString &queryStr);
    static bool addCustomer(const QString &name, const QString &phone, int point);
    static bool addRoom(const QString &roomNumber, const QString &type, const QString &status, double price, int numberPeople);
};
