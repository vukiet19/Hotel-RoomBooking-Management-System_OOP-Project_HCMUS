#pragma once

#include <QWidget>
#include <QStackedWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QString>

class MainWindow : public QWidget
{
public:
    MainWindow(QWidget *parent = nullptr);
    void setActiveButton(QPushButton *clickedButton);

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

    //dashboard 
    void handleDashboardTab();

    void showAddBookingDialog();

    void showAddRoomDialog();

    //nhấp vào sidebar & button filter trong dash
    void showFilterCustomerDialog();
    void showFilterDashboardDialog();
    //

    void showDeleteCustomerDialog();
    void AddNewCustomerClicked();
    void showUpdateCustomerDialog();
    void showUpdateRoomDialog();
    void showDeleteRoomDialog();

private:
    void loadTableData(QTableWidget *table, QString queryStr);

    QPushButton *button1;
    QPushButton *button2;
    QPushButton *button3;
    QPushButton *button4;
    QPushButton *button5;
    QPushButton *button6;
    QPushButton *button7;
    QPushButton *button8;
    QPushButton *button9;
    QPushButton *button10;

    //newly add button thành button 11 cx đc
    QPushButton *buttonDashboard;

    QPushButton *btnAdd;
    QPushButton *btnUpdate;
    QPushButton *btnDelete;
    QPushButton *btnFilter;

    QStackedWidget *stackedWidget;

    QTableWidget *tableBooking;
    QTableWidget *tableBookingItems;
    QTableWidget *tableCustomer;
    QTableWidget *tableFood;
    QTableWidget *tableInventory;
    QTableWidget *tableInventoryLog;
    QTableWidget *tableRoom;
    QTableWidget *tableRoomType;
    QTableWidget *tableService;
    QTableWidget *tableBill;
    //dashboard & 4 thông số revenue
    QTableWidget *tableDashboard;
    QLabel *lblTodayBookings;
    QLabel *lblDailyRevenue;
    QLabel *lblMonthlyRevenue;
    QLabel *lblYearlyRevenue;
};

class LoginWindow : public QWidget
{

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private:
    MainWindow *window2;

    QLabel *label1;
    QLabel *l1;
    QLabel *imageLabel;
    QLineEdit *inputBox_user;
    QLineEdit *inputBox_pass;
    QPushButton *button;
    void handleLogin();
};