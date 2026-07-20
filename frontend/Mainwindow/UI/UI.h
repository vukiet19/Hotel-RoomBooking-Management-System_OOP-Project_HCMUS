#pragma once

#include <QWidget>
#include <QStackedWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include "../../Checkout/CheckoutPage.h"
#include <QString>

class MainWindowUi : public QWidget
{
public:
    MainWindowUi(QWidget *parent = nullptr);

protected:
    void setupUi();

    QWidget *actionBar;
    QPushButton *button1, *button2, *button3, *button4, *button5;
    QPushButton *button6, *button7, *button8, *button9, *button10;
    QPushButton *buttonCheckout;

    QPushButton *btnAdd;
    QPushButton *btnUpdate;
    QPushButton *btnDelete;
    QPushButton *btnFilter;

    QStackedWidget *stackedWidget;
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