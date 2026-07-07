// Hàm này dùng để display, thực hiện của backend trong UI
// Hàm này sẽ liên kết với các hàm Repository vì đây sẽ lấy dự liệu từ database

#include "frontend.h"
#include <QPainter>
#include <QPixmap>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include "Manager/DatabaseManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>

// Trong đây parent nó có ý nghĩa là khi mình đóng chương trình thì qt sẽ tự xoá widget của chính nó, không cần gọi delete
MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedSize(1000, 800);
    setWindowTitle("Hotel Management System");
    setObjectName("MyMainWindow");

    // Cái này dùng để lưu những thứ được hiện thị trong main
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Dùng để xếp các nút như sidebar
    QWidget *sidebar = new QWidget(this);
    sidebar->setFixedWidth(220);
    sidebar->setObjectName("sidebar");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(15, 30, 15, 30);
    sidebarLayout->setSpacing(5);

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

    QList<QPushButton *> buttons = {button1, button2, button3, button4, button5,
                                    button6, button7, button8, button9, button10};

    // Lưu bottons
    for (QPushButton *btn : buttons)
    {
        sidebarLayout->addWidget(btn);
        btn->setCursor(Qt::PointingHandCursor);
    }
    // Hàm này dùng để giản để lắp đầu khoảng chống nếu không sử dụng
    sidebarLayout->addStretch();

    QWidget *contentArea = new QWidget(this);
    contentArea->setObjectName("contentArea");
    QVBoxLayout *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(30, 30, 30, 30);
    contentLayout->setSpacing(15);

    QHBoxLayout *actionBarLayout = new QHBoxLayout();

    // Nút thực hiện các hàm add,Updaye,Delete,Filter
    QPushButton *btnAdd = new QPushButton("Add", contentArea);
    QPushButton *btnUpdate = new QPushButton("Update", contentArea);
    QPushButton *btnDelete = new QPushButton("Delete", contentArea);
    QPushButton *btnFilter = new QPushButton("Filter", contentArea);

    actionBarLayout->addWidget(btnAdd);
    actionBarLayout->addWidget(btnUpdate);
    actionBarLayout->addWidget(btnDelete);
    actionBarLayout->addWidget(btnFilter);
    actionBarLayout->addStretch();

    contentLayout->addLayout(actionBarLayout);

    stackedWidget = new QStackedWidget(this);

    tableBooking = new QTableWidget(100, 6, this);
    tableBookingItems = new QTableWidget(100, 6, this);
    tableCustomer = new QTableWidget(100, 5, this);
    tableFood = new QTableWidget(100, 4, this);
    tableInventory = new QTableWidget(100, 5, this);
    tableInventoryLog = new QTableWidget(100, 5, this);
    tableRoom = new QTableWidget(100, 6, this);
    tableRoomType = new QTableWidget(100, 2, this);
    tableService = new QTableWidget(100, 4, this);
    tableBill = new QTableWidget(100, 3, this);

    tableBooking->setHorizontalHeaderLabels({"Booking ID", "Customer Name", "Room Number", "Check-in", "Check-out", "Total Price"});
    tableBookingItems->setHorizontalHeaderLabels({"ID", "Booking id", "Item id", "Quantity", "Customer note", "Final price"});
    tableCustomer->setHorizontalHeaderLabels({"Customer ID", "Name", "Phone Number", "Type", "Point"});
    tableFood->setHorizontalHeaderLabels({"Food ID", "Type food", "Name", "Price"});
    tableInventory->setHorizontalHeaderLabels({"Item ID", "Name", "Type", "Quantity", "Price"});
    tableInventoryLog->setHorizontalHeaderLabels({"Log ID", "Item ID", "Quantity", "Action type", "Date"});
    tableRoom->setHorizontalHeaderLabels({"Room ID", "Room Number", "Type", "Status", "Price", "number people"});
    tableRoomType->setHorizontalHeaderLabels({"Type ", "Base Price"});
    tableService->setHorizontalHeaderLabels({"Service ID", "Name", "Category", "Price"});
    tableBill->setHorizontalHeaderLabels({"Bill ID", "Booking ID", "Total Amount"});

    // Danh sách chứa các table
    QList<QTableWidget *> tables = {tableBooking, tableBookingItems, tableCustomer, tableFood, tableInventory,
                                    tableInventoryLog, tableRoom, tableRoomType, tableService, tableBill};
    for (QTableWidget *tb : tables)
    {
        tb->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tb->verticalHeader()->setDefaultSectionSize(40);
        stackedWidget->addWidget(tb);
    }

    contentLayout->addWidget(stackedWidget);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea);

    // Set up CSS cho button SIDEBAR
    this->setStyleSheet(R"(
        #contentArea { background-color: #e8f0fa; }
        #sidebar { background-color: white; border-right: 1px solid #e2e8f0; }
        
        /* SIDEBAR BUTTONS */
        #sidebar QPushButton {
            background-color: transparent;
            color: #64748b;
            border: none;
            border-radius: 8px;
            padding: 10px 15px;
            font-size: 15px;
            font-weight: bold;
            text-align: left;
        }
        #sidebar QPushButton:hover { background-color: #f1f5f9; color: #2563eb; }
        #sidebar QPushButton[active="true"] { 
            background-color: #eff6ff; 
            color: #1d4ed8; 
            border-left: 4px solid #3b82f6; 
            border-top-left-radius: 4px;
            border-bottom-left-radius: 4px;
        }

        /* NEW: TOP ACTION BUTTONS (Add, Update, Delete) */
        #contentArea QPushButton {
            background-color: white;
            color: #333333;
            border: 1px solid #d1d5db;
            border-radius: 6px;
            padding: 8px 16px;
            font-size: 14px;
            font-weight: bold;
            text-align: center;
        }
        #contentArea QPushButton:hover { background-color: #f3f4f6; }
        
        /* TABLES */
        QTableWidget {
            background-color: white;
            border: none;
            border-radius: 8px;
            gridline-color: #f1f5f9;
            font-size: 14px;
            color: #333333;
            selection-background-color: #eff6ff;
            selection-color: #1e3a8a;
        }
        
        /* FIXED: Top Headers (Thick padding) */
        QHeaderView::section:horizontal {
            background-color: white;
            color: #64748b;
            font-weight: bold;
            padding: 12px;
            border: none;
            border-bottom: 2px solid #e2e8f0;
        }

        /* FIXED: Left Row Numbers (Thin padding so numbers don't get cut off) */
        QHeaderView::section:vertical {
            background-color: white;
            color: #64748b;
            font-weight: bold;
            padding: 4px; 
            border: none;
            border-right: 1px solid #e2e8f0;
        }
    )");

    // Đây dùng để liên kết hàm( trong đó khi ấn button1 thì sẽ thực hiện hàm. tương ứng)
    connect(button1, &QPushButton::clicked, this, &MainWindow::handleLogin_1);
    connect(button2, &QPushButton::clicked, this, &MainWindow::handleLogin_2);
    connect(button3, &QPushButton::clicked, this, &MainWindow::handleLogin_3);
    connect(button4, &QPushButton::clicked, this, &MainWindow::handleLogin_4);
    connect(button5, &QPushButton::clicked, this, &MainWindow::handleLogin_5);
    connect(button6, &QPushButton::clicked, this, &MainWindow::handleLogin_6);
    connect(button7, &QPushButton::clicked, this, &MainWindow::handleLogin_7);
    connect(button8, &QPushButton::clicked, this, &MainWindow::handleLogin_8);
    connect(button9, &QPushButton::clicked, this, &MainWindow::handleLogin_9);
    connect(button10, &QPushButton::clicked, this, &MainWindow::handleLogin_10);
}
void MainWindow::handleLogin_1()
{
    stackedWidget->setCurrentIndex(0);
    setActiveButton(button1);
    loadTableData(tableBooking, "SELECT * FROM Bookings");
}

void MainWindow::handleLogin_2()
{
    stackedWidget->setCurrentIndex(1);
    setActiveButton(button2);
    loadTableData(tableBookingItems, "SELECT * FROM BookingServiceItems");
}

void MainWindow::handleLogin_3()
{
    stackedWidget->setCurrentIndex(2);
    setActiveButton(button3);
    loadTableData(tableCustomer, "SELECT * FROM Customer");
}

void MainWindow::handleLogin_4()
{
    stackedWidget->setCurrentIndex(3);
    setActiveButton(button4);
    loadTableData(tableFood, "SELECT * FROM FoodOptions");
}

void MainWindow::handleLogin_5()
{
    stackedWidget->setCurrentIndex(4);
    setActiveButton(button5);
    loadTableData(tableInventory, "SELECT * FROM Inventory");
}

void MainWindow::handleLogin_6()
{
    stackedWidget->setCurrentIndex(5);
    setActiveButton(button6);
    loadTableData(tableInventoryLog, "SELECT * FROM InventoryLog");
}

void MainWindow::handleLogin_7()
{
    stackedWidget->setCurrentIndex(6);
    setActiveButton(button7);
    loadTableData(tableRoom, "SELECT * FROM ListRooms");
}

void MainWindow::handleLogin_8()
{
    stackedWidget->setCurrentIndex(7);
    setActiveButton(button8);
    loadTableData(tableRoomType, "SELECT * FROM RoomTypeCatalog");
}

void MainWindow::handleLogin_9()
{
    stackedWidget->setCurrentIndex(8);
    setActiveButton(button9);
    loadTableData(tableService, "SELECT * FROM ListServiceItems");
}

void MainWindow::handleLogin_10()
{
    stackedWidget->setCurrentIndex(9);
    setActiveButton(button10);
    loadTableData(tableBill, "SELECT * FROM Bills");
}
// dùng để load data, sẽ nhận là table (dạng table gì) và là query được yêu cầu làm
void MainWindow::loadTableData(QTableWidget *table, QString queryStr)
{
    // Dòng này để reset lại dòng ,nghĩa là sẽ chỉ còn lại 1 cái bảng với không có dòng nào
    table->setRowCount(0);

    if (!DatabaseManager::instance().open())
        return;

    QSqlQuery query(DatabaseManager::instance().database());
    if (query.exec(queryStr))
    {
        int row = 0;
        while (query.next())
        {
            // Thêm cột
            table->insertRow(row);
            for (int col = 0; col < table->columnCount(); ++col)
            {
                table->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
            }
            row++;
        }
    }
}

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedSize(1000, 800);
    setWindowTitle("Hotel_Management");
    setObjectName("MyMainWindow");

    setStyleSheet(
        "#MyMainWindow {"
        "   background-color: #e8f0fa;"
        "}");

    window2 = new MainWindow();

    label1 = new QLabel("Log in", this);
    QLabel *subLabel = new QLabel("Please log in to use services", this);

    inputBox_user = new QLineEdit(this);
    inputBox_pass = new QLineEdit(this);
    // Echomode là mode mà khi nhập mật khậu sẽ không hiện ra
    inputBox_pass->setEchoMode(QLineEdit::Password);

    button = new QPushButton("Log in", this);

    imageLabel = new QLabel(this);
    l1 = new QLabel(this);

    QPixmap myImage("NTH.png");
    imageLabel->setPixmap(myImage);
    imageLabel->setScaledContents(true);
    imageLabel->setGeometry(100, 50, 120, 120);

    label1->setGeometry(0, 200, 1000, 60);
    label1->setAlignment(Qt::AlignCenter);

    subLabel->setGeometry(0, 270, 1000, 30);
    subLabel->setAlignment(Qt::AlignCenter);

    inputBox_user->setGeometry(300, 370, 400, 50);
    inputBox_pass->setGeometry(300, 440, 400, 50);

    button->setGeometry(425, 520, 150, 45);

    l1->setGeometry(0, 630, 1000, 40);
    l1->setAlignment(Qt::AlignCenter);

    label1->setStyleSheet("font-size: 38px; color: #333333;");

    subLabel->setStyleSheet("font-size: 18px; color: #6b7280;");

    QString inputStyle =
        "background-color: white; "
        "border: none; "
        "border-radius: 2px; "
        "padding: 10px; "
        "font-size: 16px; "
        "color: #333333;";

    inputBox_user->setStyleSheet(inputStyle);
    inputBox_pass->setStyleSheet(inputStyle);

    inputBox_user->setPlaceholderText("Username");
    inputBox_pass->setPlaceholderText("Password");

    button->setStyleSheet(
        "QPushButton {"
        "   background-color: #3b82f6; "
        "   color: white; "
        "   border: none; "
        "   border-radius: 4px; "
        "   font-size: 18px; "
        "}"
        "QPushButton:hover {"
        "   background-color: #2563eb; "
        "}");

    connect(button, &QPushButton::clicked, this, &LoginWindow::handleLogin);
}

LoginWindow::~LoginWindow()
{
    delete window2;
}

bool LoginWindow::check_valid_username(const std::string &g)
{
    if (g.size() == 0)
        return false;
    for (size_t i = 0; i < g.size(); i++)
    {
        if (g[i] == ' ')
            return false;
    }
    return true;
}

bool LoginWindow::check_valid_password(const std::string &g, const std::string &p)
{
    return g == p;
}

void LoginWindow::handleLogin()
{
    QString user = inputBox_user->text();
    QString pass = inputBox_pass->text();

    if (check_valid_username(user.toStdString()) && check_valid_password(pass.toStdString(), "suuu"))
    {
        l1->setText("Correct");
        l1->setGeometry(325, 560, 400, 50);
        l1->setStyleSheet("font-size: 18px; font-weight: bold; color: green;");
        l1->show();

        window2->show();
        this->close();
    }
    else
    {
        l1->setText("Incorrect username or password");
        l1->setGeometry(325, 560, 400, 50);
        l1->setStyleSheet("font-size: 18px; font-weight: bold; color: RED;");
        l1->show();
    }
}

void MainWindow::setActiveButton(QPushButton *clickedButton)
{
    QList<QPushButton *> buttons = {
        button1, button2, button3, button4, button5,
        button6, button7, button8, button9, button10};

    for (QPushButton *btn : buttons)
    {
        if (btn == clickedButton)
        {
            btn->setProperty("active", true);
        }
        else
        {
            btn->setProperty("active", false);
        }

        btn->style()->unpolish(btn);
        btn->style()->polish(btn);
    }
}