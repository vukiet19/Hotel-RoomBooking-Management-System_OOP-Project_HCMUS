#pragma once

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <string>
#include "Manager/DatabaseManager.h"

class MainWindow;
class LoginWindow;

class Backend : public QObject
{
    Q_OBJECT
    friend class MainWindow;
    friend class LoginWindow;

public:
    explicit Backend(QObject *parent = nullptr);

    void setMainWindowUI(QStackedWidget *stacked, QTableWidget *table1, QTableWidget *table2,
                         QTableWidget *table3, QTableWidget *table4, QTableWidget *table5,
                         QTableWidget *table6, QTableWidget *table7, QTableWidget *table8,
                         QTableWidget *table9, QTableWidget *table10);
    void setLoginWindowUI(QLineEdit *user, QLineEdit *pass, QLabel *label);

public slots:
    void handleLogin_1();
    void handleLogin_2();
    void handleLogin_3();
    void handleLogin_4();
    void handleLogin_5();
    void handleLogin_6();
    void handleLogin_7();
    void handleLogin_8();
    void handleLogin_9();
    void handleLogin_10();
    void handleLogin();

    void loadTableData(QTableWidget *table, QString queryStr);
    bool check_valid_username(const std::string &g);
    bool check_valid_password(const std::string &g, const std::string &p);

private:
    QStackedWidget *m_stackedWidget = nullptr;
    QTableWidget *m_tableBooking = nullptr;
    QTableWidget *m_tableBookingItems = nullptr;
    QTableWidget *m_tableCustomer = nullptr;
    QTableWidget *m_tableFood = nullptr;
    QTableWidget *m_tableInventory = nullptr;
    QTableWidget *m_tableInventoryLog = nullptr;
    QTableWidget *m_tableRoom = nullptr;
    QTableWidget *m_tableRoomType = nullptr;
    QTableWidget *m_tableService = nullptr;
    QTableWidget *m_tableBill = nullptr;

    QLineEdit *m_inputBox_user = nullptr;
    QLineEdit *m_inputBox_pass = nullptr;
    QLabel *m_l1 = nullptr;

    MainWindow *m_mainWindow = nullptr;
};