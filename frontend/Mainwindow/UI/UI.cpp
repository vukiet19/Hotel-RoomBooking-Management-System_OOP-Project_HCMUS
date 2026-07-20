#include "UI.h"
#include "../../Checkout/CheckoutPage.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>

MainWindowUi::MainWindowUi(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedSize(1000, 800);
    setWindowTitle("Hotel Management System");
    setObjectName("MyMainWindow");
}

void MainWindowUi::setupUi()
{
    QWidget *sidebar = new QWidget(this);
    sidebar->setFixedWidth(240);
    sidebar->setObjectName("sidebar");

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(15, 30, 0, 30);
    sidebarLayout->setSpacing(8);

    button1 = new QPushButton("Booking", sidebar);
    button2 = new QPushButton("Booking Services", sidebar);
    button3 = new QPushButton("Customer", sidebar);
    button4 = new QPushButton("Food Options", sidebar);
    button5 = new QPushButton("Inventory", sidebar);
    button6 = new QPushButton("Inventory Log", sidebar);
    button7 = new QPushButton("Room", sidebar);
    button8 = new QPushButton("Room Type", sidebar);
    button9 = new QPushButton("Service", sidebar);
    button10 = new QPushButton("Bill", sidebar);
    buttonCheckout = new QPushButton("Checkout", sidebar);

    QList<QPushButton *> buttons = {button1, button2, button3, button4, button5,
                                    button6, button7, button8, button9, button10, buttonCheckout};

    for (QPushButton *btn : buttons)
    {
        sidebarLayout->addWidget(btn);
        btn->setCursor(Qt::PointingHandCursor);
    }

    sidebarLayout->addStretch();

    QWidget *contentArea = new QWidget(this);
    contentArea->setObjectName("contentArea");
    QVBoxLayout *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(40, 40, 40, 40);
    contentLayout->setSpacing(20);

    // --- FIX: Bọc ActionBarLayout vào một QWidget (actionBar) ---
    actionBar = new QWidget(contentArea);
    actionBar->setObjectName("actionBar");
    QHBoxLayout *actionBarLayout = new QHBoxLayout(actionBar);
    actionBarLayout->setContentsMargins(0, 0, 0, 0);

    btnAdd = new QPushButton("Add", actionBar);
    btnUpdate = new QPushButton("Update", actionBar);
    btnDelete = new QPushButton("Delete", actionBar);
    btnFilter = new QPushButton("Filter", actionBar);

    actionBarLayout->addWidget(btnAdd);
    actionBarLayout->addWidget(btnUpdate);
    actionBarLayout->addWidget(btnDelete);
    actionBarLayout->addWidget(btnFilter);
    actionBarLayout->addStretch();

    contentLayout->addWidget(actionBar);
    // -------------------------------------------------------------

    stackedWidget = new QStackedWidget(this);

    tableBooking = new QTableWidget(100, 6, this);
    tableBookingItems = new QTableWidget(100, 6, this);
    tableCustomer = new QTableWidget(100, 6, this);
    tableFood = new QTableWidget(100, 4, this);
    tableInventory = new QTableWidget(100, 5, this);
    tableInventoryLog = new QTableWidget(100, 5, this);
    tableRoom = new QTableWidget(100, 6, this);
    tableRoomType = new QTableWidget(100, 2, this);
    tableService = new QTableWidget(100, 4, this);
    tableBill = new QTableWidget(100, 3, this);

    tableBooking->setHorizontalHeaderLabels({"Booking ID", "Customer ID", "Room Number", "Check-in", "Check-out", "Total Price"});
    tableBookingItems->setHorizontalHeaderLabels({"ID", "Booking id", "Item id", "Quantity", "Customer note", "Final price"});
    tableCustomer->setHorizontalHeaderLabels({"Customer ID", "ID number", "Name", "Phone Number", "Type", "Point"});
    tableFood->setHorizontalHeaderLabels({"Food ID", "Type food", "Name", "Price"});
    tableInventory->setHorizontalHeaderLabels({"Item ID", "Name", "Type", "Quantity", "Price"});
    tableInventoryLog->setHorizontalHeaderLabels({"Log ID", "Item ID", "Quantity", "Action type", "Date"});
    tableRoom->setHorizontalHeaderLabels({"Room ID", "Room Number", "Type", "Status", "Price", "number people"});
    tableRoomType->setHorizontalHeaderLabels({"Type ", "Base Price"});
    tableService->setHorizontalHeaderLabels({"Service ID", "Name", "Category", "Price"});
    tableBill->setHorizontalHeaderLabels({"Bill ID", "Booking ID", "Total Amount"});

    QList<QTableWidget *> tables = {tableBooking, tableBookingItems, tableCustomer, tableFood, tableInventory,
                                    tableInventoryLog, tableRoom, tableRoomType, tableService, tableBill};
    for (QTableWidget *tb : tables)
    {
        tb->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tb->verticalHeader()->setDefaultSectionSize(45);
        tb->setAlternatingRowColors(true); // Ensures the alternate colors work
        stackedWidget->addWidget(tb);
    }

    checkoutPage = new CheckoutPage(this);
    stackedWidget->addWidget(checkoutPage);

    contentLayout->addWidget(stackedWidget);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea);

    this->setStyleSheet(R"(
        #contentArea { 
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:0.5 #e0f2fe, stop:1 #ffffff); 
        }
        /* --- PREMIUM SIDEBAR (TONE TÍM/INDIGO SÁNG HƠN) --- */
        #sidebar { 
            background-color: #47456d; /* Màu Tím/Indigo sáng hơn màu bảng cũ */
            border-right: 1px solid #312e81; /* Viền phân cách nhẹ nhàng */
        }
        
        #sidebar QPushButton {
            background-color: transparent;
            color: #e0e7ff; /* Chữ màu xanh tím nhạt cực êm mắt */
            border: none;
            border-radius: 10px; /* Bo góc tròn hiện đại */
            padding: 12px 20px;
            margin: 4px 12px; /* Khoảng cách thở cho các nút */
            font-size: 15px;
            font-family: 'Segoe UI', Arial, sans-serif;
            font-weight: 600;
            text-align: left;
        }
        
        #sidebar QPushButton:hover { 
            background-color: #818cf8; /* Màu tím pastel sáng bừng, tách biệt hoàn toàn với nền */
            color: #ffffff; 
        }
        
        #sidebar QPushButton[active="true"] { 
            /* Gradient Tím - Xanh hòa hợp tuyệt đối với bảng bên phải */
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0a7ed0);
            color: #ffffff; 
            border-radius: 10px; 
            font-weight: bold;
        }

        #contentArea QPushButton {
            background-color: #ffffff;
            color: #3730a3; /* Deep vibrant text */
            border: 2px solid #a5b4fc;
            border-radius: 18px; /* Bo viền tròn trịa */
            padding: 8px 20px;
            font-size: 14px;
            font-weight: bold;
            font-family: 'Segoe UI', Arial, sans-serif;
        }
        #contentArea QPushButton:hover { 
            background-color: #e0e7ff; 
            border-color: #6366f1;
            color: #312e81;
        }
        
        /* --- VIBRANT TABLE STYLES --- */
        QTableWidget {
            background-color: #ffffff;
            alternate-background-color: #f0f9ff; /* Xanh lơ nhạt */
            border: 1px solid #bae6fd;
            border-radius: 8px;
            gridline-color: #e0f2fe; 
            font-size: 14px;
            color: #0f172a;
            selection-background-color: #38bdf8; /* Sky blue selection */
            selection-color: #ffffff;
            outline: none; 
        }
        
        QTableWidget::item {
            padding: 5px; 
        }

        QTableWidget::item:hover {
            background-color: #e0f2fe; 
            color: #0f172a;
        }
        
        /* --- BOLD, PREMIUM HEADERS --- */
        QHeaderView::section:horizontal {
            background-color: #312e81; /* Deep Indigo Header */
            color: #ffffff;
            font-weight: bold;
            font-size: 14px;
            padding: 12px;
            border: none;
            border-right: 1px solid #1e1b4b; 
        }

        QHeaderView::section:horizontal:first {
            border-top-left-radius: 8px; 
        }

        QHeaderView::section:horizontal:last {
            border-top-right-radius: 8px;
            border-right: none;
        }

        QHeaderView::section:vertical {
            background-color: #f8fafc;
            color: #64748b;
            font-weight: bold;
            padding: 4px; 
            border: none;
            border-right: 1px solid #bae6fd;
        }
        
        QTableCornerButton::section {
            background-color: #312e81; 
            border: none;
        }
    )");
}