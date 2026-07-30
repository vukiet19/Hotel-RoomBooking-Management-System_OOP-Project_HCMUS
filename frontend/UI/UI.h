#pragma once

#include <QWidget>
#include <QStackedWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include "frontend/Checkout/CheckoutPage.h"
#include <QString>

class MainWindowUi : public QWidget
{
public:
    MainWindowUi(QWidget *parent = nullptr);

protected:
    void setupUi();

    QWidget *actionBar;
    QPushButton *btnNavBooking, *btnNavBookingServices, *btnNavCustomer, *btnNavFood, *btnNavInventory;
    QPushButton *btnNavInventoryLog, *btnNavRoom, *btnNavRoomType, *btnNavService, *btnNavBill;
    QPushButton *buttonCheckout;
    QPushButton *buttonDashboard;


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
    QTableWidget *tableDashboard;

    QLabel *lblTodayBookings;
    QLabel *lblDailyRevenue;
    QLabel *lblMonthlyRevenue;
    QLabel *lblYearlyRevenue;
};
