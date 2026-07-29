#include "UI.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

MainWindowUi::MainWindowUi(QWidget *parent) : QWidget(parent) {
  setAttribute(Qt::WA_StyledBackground, true);
  setFixedSize(1000, 800);
  setWindowTitle("Hotel Management System");
  setObjectName("MyMainWindow");
}

void MainWindowUi::setupUi() {
  auto *mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  auto *sidebar = new QWidget(this);
  sidebar->setObjectName("sidebar");
  sidebar->setFixedWidth(240);
  auto *sidebarLayout = new QVBoxLayout(sidebar);
  sidebarLayout->setContentsMargins(15, 30, 0, 30);
  sidebarLayout->setSpacing(8);

  buttonDashboard = new QPushButton("Dashboard", sidebar);
  buttonBooking = new QPushButton("Booking", sidebar);
  buttonCustomer = new QPushButton("Customer", sidebar);
  buttonRoom = new QPushButton("Room", sidebar);
  buttonService = new QPushButton("Service", sidebar);
  buttonInventory = new QPushButton("Inventory", sidebar);
  buttonBill = new QPushButton("Bill", sidebar);
  buttonCheckout = new QPushButton("Checkout", sidebar);

  for (auto *button :
       {buttonDashboard, buttonBooking, buttonCustomer, buttonRoom,
        buttonService, buttonInventory, buttonBill, buttonCheckout}) {
    button->setCursor(Qt::PointingHandCursor);
    sidebarLayout->addWidget(button);
  }
  sidebarLayout->addStretch();

  auto *contentArea = new QWidget(this);
  contentArea->setObjectName("contentArea");
  auto *contentLayout = new QVBoxLayout(contentArea);
  contentLayout->setContentsMargins(40, 40, 40, 40);
  contentLayout->setSpacing(20);

  actionBar = new QWidget(contentArea);
  actionBar->setObjectName("actionBar");
  auto *actionBarLayout = new QHBoxLayout(actionBar);
  actionBarLayout->setContentsMargins(0, 0, 0, 0);
  btnAdd = new QPushButton("Add", actionBar);
  btnUpdate = new QPushButton("Update", actionBar);
  btnDelete = new QPushButton("Delete", actionBar);
  btnFilter = new QPushButton("Filter", actionBar);
  for (auto *button : {btnAdd, btnUpdate, btnDelete, btnFilter})
    actionBarLayout->addWidget(button);
  actionBarLayout->addStretch();

  stackedWidget = new QStackedWidget(contentArea);
  dashboardPage = new DashboardPage(stackedWidget);
  bookingPage = new BookingPage(stackedWidget);
  customerPage = new CustomerPage(stackedWidget);
  roomPage = new RoomPage(stackedWidget);
  servicePage = new ServicePage(stackedWidget);
  inventoryPage = new InventoryPage(stackedWidget);
  billPage = new BillPage(stackedWidget);
  checkoutPage = new CheckoutPage(stackedWidget);

  stackedWidget->addWidget(dashboardPage);
  stackedWidget->addWidget(bookingPage);
  stackedWidget->addWidget(customerPage);
  stackedWidget->addWidget(roomPage);
  stackedWidget->addWidget(servicePage);
  stackedWidget->addWidget(inventoryPage);
  stackedWidget->addWidget(billPage);
  stackedWidget->addWidget(checkoutPage);

  tableBooking = bookingPage->bookingTable();
  tableBookingItems = bookingPage->bookingServicesTable();
  tableCustomer = customerPage->table();
  tableRoom = roomPage->roomTable();
  tableRoomType = roomPage->roomTypeTable();
  tableFood = servicePage->foodTable();
  tableService = servicePage->serviceTable();
  tableInventory = inventoryPage->inventoryTable();
  tableInventoryLog = inventoryPage->inventoryLogTable();
  tableBill = billPage->table();
  tableDashboard = dashboardPage->table();
  lblTodayBookings = dashboardPage->todayBookingsLabel();
  lblDailyRevenue = dashboardPage->dailyRevenueLabel();
  lblMonthlyRevenue = dashboardPage->monthlyRevenueLabel();
  lblYearlyRevenue = dashboardPage->yearlyRevenueLabel();

  contentLayout->addWidget(actionBar);
  contentLayout->addWidget(stackedWidget, 1);
  mainLayout->addWidget(sidebar);
  mainLayout->addWidget(contentArea, 1);

  setStyleSheet(R"(
        #contentArea { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:0.5 #e0f2fe, stop:1 #ffffff); }
        #sidebar { background-color: #47456d; border-right: 1px solid #312e81; }
        #sidebar QPushButton { background: transparent; color: #e0e7ff; border: none; border-radius: 10px; padding: 12px 20px; margin: 4px 12px; font-size: 15px; font-family: 'Segoe UI', Arial, sans-serif; font-weight: 600; text-align: left; }
        #sidebar QPushButton:hover { background-color: #818cf8; color: #ffffff; }
        #sidebar QPushButton[active="true"] { background-color: #0a7ed0; color: #ffffff; font-weight: bold; }
        #actionBar QPushButton { background-color: #ffffff; color: #3730a3; border: 2px solid #a5b4fc; border-radius: 18px; padding: 8px 20px; font-size: 14px; font-weight: 600; }
        #actionBar QPushButton:hover { background-color: #eef2ff; }
        QTableWidget { background-color: #ffffff; alternate-background-color: #f5f3ff; border: 2px solid #bae6fd; border-radius: 8px; gridline-color: #dbeafe; color: #1e293b; selection-background-color: #6366f1; selection-color: #ffffff; }
        QHeaderView::section { background-color: #3730a3; color: #ffffff; font-weight: bold; padding: 10px; border: none; }
    )");
}