#pragma once

#include <QWidget>

#include "frontend/UI/ManagerWindow/Booking/BookingPage.h"
#include "frontend/UI/ManagerWindow/Bill/BillPage.h"
#include "frontend/UI/ManagerWindow/Checkout/CheckoutPage.h"
#include "frontend/UI/ManagerWindow/Customer/CustomerPage.h"
#include "frontend/UI/ManagerWindow/Dashboard/DashboardPage.h"
#include "frontend/UI/ManagerWindow/Inventory/InventoryPage.h"
#include "frontend/UI/ManagerWindow/Room/RoomPage.h"
#include "frontend/UI/ManagerWindow/Service/ServicePage.h"

class QStackedWidget;
class QTableWidget;
class QPushButton;
class QLabel;

class MainWindowUi : public QWidget
{
public:
    MainWindowUi(QWidget *parent = nullptr);

protected:
    enum PageIndex
    {
        DashboardIndex,
        BookingIndex,
        CustomerIndex,
        RoomIndex,
        ServiceIndex,
        InventoryIndex,
        BillIndex,
        CheckoutIndex
    };

    void setupUi();

    QWidget *actionBar;
    QPushButton *buttonDashboard;
    QPushButton *buttonBooking;
    QPushButton *buttonCustomer;
    QPushButton *buttonRoom;
    QPushButton *buttonService;
    QPushButton *buttonInventory;
    QPushButton *buttonBill;
    QPushButton *buttonCheckout;

    QPushButton *btnAdd;
    QPushButton *btnUpdate;
    QPushButton *btnDelete;
    QPushButton *btnFilter;

    QStackedWidget *stackedWidget;
    BookingPage *bookingPage;
    CustomerPage *customerPage;
    RoomPage *roomPage;
    ServicePage *servicePage;
    InventoryPage *inventoryPage;
    BillPage *billPage;
    DashboardPage *dashboardPage;
    CheckoutPage *checkoutPage;

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