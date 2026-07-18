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
    void setActionBarVisible(bool visible);

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
    void handleCheckout();

    void showAddBookingDialog();

    void showAddRoomDialog();

    void showFilterCustomerDialog();
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
    QPushButton *buttonCheckout;

    QPushButton *btnAdd;
    QPushButton *btnUpdate;
    QPushButton *btnDelete;
    QPushButton *btnFilter;

    QStackedWidget *stackedWidget;
    QWidget *actionBar;
    class CheckoutPage *checkoutPage;

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
