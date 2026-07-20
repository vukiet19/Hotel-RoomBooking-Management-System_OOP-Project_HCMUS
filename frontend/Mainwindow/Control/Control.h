#pragma once

#include "../Ui/UI.h"
#include <QString>

class MainWindowController : public MainWindowUi
{
public:
    MainWindowController(QWidget *parent = nullptr);

private:
    void initConnections();
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
};