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
#include "Manager/DatabaseManager.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);

    setFixedSize(1000, 800);
    setWindowTitle("MyMainWindow");
    setObjectName("MyMainWindow");
    setStyleSheet(
        "#MyMainWindow {"
        "   border-image: url(images.jpeg);"
        "}");

    button1 = new QPushButton("Booking", this);
    button2 = new QPushButton("Customer", this);
    button3 = new QPushButton("Room", this);
    button4 = new QPushButton("Bill", this);

    button1->setGeometry(0, 100, 200, 150);
    button2->setGeometry(0, 250, 200, 150);
    button3->setGeometry(0, 400, 200, 150);
    button4->setGeometry(0, 550, 200, 150);

    button1->setStyleSheet("background-color: #cea71a; color: white; font-weight: bold; font-size: 18px;");
    button2->setStyleSheet("background-color: #cea71a; color: white; font-weight: bold; font-size: 18px;");
    button3->setStyleSheet("background-color: #cea71a; color: white; font-weight: bold; font-size: 18px;");
    button4->setStyleSheet("background-color: #cea71a; color: white; font-weight: bold; font-size: 18px;");

    stackedWidget = new QStackedWidget(this);
    stackedWidget->setGeometry(200, 100, 780, 680);

    QString tableStyle =
        "QTableWidget {"
        "   background-color: #ffffff; "
        "   alternate-background-color: #f2f2f2; "
        "   border: 1px solid #d3d3d3; "
        "   font-size: 14px; "
        "   color: #030000; "
        "}"
        "QHeaderView::section {"
        "   background-color: #f3f3f3; "
        "   color: black; "
        "   font-weight: bold; "
        "   padding: 4px; "
        "   border: 1px solid #d3d3d3; "
        "}";

    tableBooking = new QTableWidget(100, 6, this);
    tableBooking->setHorizontalHeaderLabels({"Booking ID", "Customer Name", "Room Number", "Check-in Date", "Check-out Date", "Total_price"});
    tableBooking->setStyleSheet(tableStyle);
    tableBooking->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tableCustomer = new QTableWidget(100, 5, this);
    tableCustomer->setHorizontalHeaderLabels({"Customer ID", "Name", "Phone Number", "Type", "Point"});
    tableCustomer->setStyleSheet(tableStyle);
    tableCustomer->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tableRoom = new QTableWidget(100, 5, this);
    tableRoom->setHorizontalHeaderLabels({"Room ID", "Room Number", "Type", "Price", "Status"});
    tableRoom->setStyleSheet(tableStyle);
    tableRoom->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tableBill = new QTableWidget(100, 3, this);
    tableBill->setHorizontalHeaderLabels({"Bill ID", "Booking ID", "Total Amount"});
    tableBill->setStyleSheet(tableStyle);
    tableBill->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    stackedWidget->addWidget(tableBooking);
    stackedWidget->addWidget(tableCustomer);
    stackedWidget->addWidget(tableRoom);
    stackedWidget->addWidget(tableBill);

    connect(button1, &QPushButton::clicked, this, &MainWindow::handleLogin_1);
    connect(button2, &QPushButton::clicked, this, &MainWindow::handleLogin_2);
    connect(button3, &QPushButton::clicked, this, &MainWindow::handleLogin_3);
    connect(button4, &QPushButton::clicked, this, &MainWindow::handleLogin_4);
}

void MainWindow::handleLogin_1()
{
    button1->setStyleSheet("background-color: #ed1b1b; color: white; font-weight: bold; font-size: 18px;");
    button2->setStyleSheet("background-color: #cea71a; color: white; font-weight: bold; font-size: 18px;");
    button3->setStyleSheet("background-color: #cea71a; color: white; font-weight: bold; font-size: 18px;");
    button4->setStyleSheet("background-color: #cea71a; color: white; font-weight: bold; font-size: 18px;");

    stackedWidget->setCurrentIndex(0);

    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    if (!query.exec("SELECT id, customer_id, room_number, check_in_time, check_out_time, total_price FROM Bookings"))
    {
        qDebug() << "Query Error:" << query.lastError().text();
        return;
    }

    // 1. Force the table to have 6 columns so your data has a place to go
    tableRoom->setColumnCount(6);

    // Set headers (Optional, but highly recommended)
    tableRoom->setHorizontalHeaderLabels({"ID", "Customer", "Room", "Check-In", "Check-Out", "Price"});

    // 2. Clear out any old rows
    tableRoom->setRowCount(0);

    int i = 0;
    while (query.next())
    {
        // Print to the console so we KNOW the loop is running!
        qDebug() << "Loading row" << i << "from database...";

        // 3. Create the physical row in the UI
        tableRoom->insertRow(i);

        QString id = query.value(0).toString();
        tableRoom->setItem(i, 0, new QTableWidgetItem(id));

        QString cus = query.value(1).toString();
        tableRoom->setItem(i, 1, new QTableWidgetItem(cus));

        QString room = query.value(2).toString();
        tableRoom->setItem(i, 2, new QTableWidgetItem(room));

        QString checkin = query.value(3).toString();
        tableRoom->setItem(i, 3, new QTableWidgetItem(checkin));

        QString checkout = query.value(4).toString();
        tableRoom->setItem(i, 4, new QTableWidgetItem(checkout));

        QString price = query.value(5).toString();
        tableRoom->setItem(i, 5, new QTableWidgetItem(price));

        i++;
    }

    // Final check
    qDebug() << "Total rows loaded into tableRoom:" << i;
}

void MainWindow::handleLogin_2()
{
    button2->setStyleSheet("background-color: #ed1b1b; color: white; font-weight: bold; font-size: 18px;");
    button1->setStyleSheet("background-color: #cea71a; color: white; font-weight: bold; font-size: 18px;");
    button4->setStyleSheet("background-color: #cea71a; color: white; font-weight: bold; font-size: 18px;");
    button3->setStyleSheet("background-color: #cea71a; color: white; font-weight: bold; font-size: 18px;");

    stackedWidget->setCurrentIndex(1);
}

void MainWindow::handleLogin_3()
{
    button3->setStyleSheet("background-color: #ed1b1b; color: white; font-weight: bold; font-size: 18px;");
    button1->setStyleSheet("background-color: #cea71a; color: white; font-weight: bold; font-size: 18px;");
    button2->setStyleSheet("background-color: #cea71a; color: white; font-weight: bold; font-size: 18px;");
    button4->setStyleSheet("background-color: #cea71a; color: white; font-weight: bold; font-size: 18px;");

    stackedWidget->setCurrentIndex(2);
}

void MainWindow::handleLogin_4()
{
    button4->setStyleSheet("background-color: #ed1b1b; color: white; font-weight: bold; font-size: 18px;");
    button1->setStyleSheet("background-color: #cea71a; color: white; font-weight: bold; font-size: 18px;");
    button2->setStyleSheet("background-color: #cea71a; color: white; font-weight: bold; font-size: 18px;");
    button3->setStyleSheet("background-color: #cea71a; color: white; font-weight: bold; font-size: 18px;");

    stackedWidget->setCurrentIndex(3);
}

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent)
{

    setAttribute(Qt::WA_StyledBackground, true);

    setFixedSize(1000, 800);
    setWindowTitle("MyMainWindow");
    setObjectName("MyMainWindow");
    setStyleSheet(
        "#MyMainWindow {"
        "   border-image: url(images.jpeg);"
        "}");

    window2 = new MainWindow();

    label1 = new QLabel("Admin's log in", this);
    label2 = new QLabel("Username", this);
    label3 = new QLabel("Password", this);
    inputBox_user = new QLineEdit(this);
    inputBox_pass = new QLineEdit(this);
    button = new QPushButton("Log in", this);
    imageLabel = new QLabel(this);
    l1 = new QLabel(this);

    label1->setGeometry(350, 0, 1000, 260);
    label2->setGeometry(325, 330, 400, 100);
    label3->setGeometry(325, 440, 400, 100);
    inputBox_user->setGeometry(325, 400, 400, 50);
    inputBox_pass->setGeometry(325, 510, 400, 50);
    button->setGeometry(325, 600, 400, 50);

    QPixmap myImage("NTH.png");
    imageLabel->setPixmap(myImage);
    imageLabel->setScaledContents(true);
    int imageWidth = 200;
    int imageHeight = 200;
    int windowWidth = 1000;
    int centerX = (windowWidth / 2) - (imageWidth / 2);
    imageLabel->setGeometry(centerX, 165, imageWidth, imageHeight);

    label1->setStyleSheet("font-size: 48px; font-weight: bold; color: blue;");
    label2->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    label3->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    inputBox_user->setPlaceholderText("Enter your user name: ");
    inputBox_pass->setPlaceholderText("Enter your password:");

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