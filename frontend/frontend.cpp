#include "frontend.h"
#include "backend.h"
#include "Booking/Booking.h"
#include <QPainter>
#include <QPixmap>
#include <QDialog>
#include <QString>
#include <QDateEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFile>
#include <QComboBox>
#include "Room/Room.h"
#include "customerwin.h"
#include "Repository/RoomRepository.h"
#include "Repository/CustomerRepository.h"

#include "Room/DerivedRooms.h"
#include "Room/TypeRoom.h"
#include "Room/RoomFactory.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedSize(1000, 800);
    setWindowTitle("Hotel Management System");
    setObjectName("MyMainWindow");

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QWidget *sidebar = new QWidget(this);
    sidebar->setFixedWidth(240); // Slightly wider for a premium feel
    sidebar->setObjectName("sidebar");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(15, 30, 15, 30);
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

    QList<QPushButton *> buttons = {button1, button2, button3, button4, button5,
                                    button6, button7, button8, button9, button10};

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

    QHBoxLayout *actionBarLayout = new QHBoxLayout();

    btnAdd = new QPushButton("Add", contentArea);
    btnUpdate = new QPushButton("Update", contentArea);
    btnDelete = new QPushButton("Delete", contentArea);
    btnFilter = new QPushButton("Filter", contentArea);

    actionBarLayout->addWidget(btnAdd);
    actionBarLayout->addWidget(btnUpdate);
    actionBarLayout->addWidget(btnDelete);
    actionBarLayout->addWidget(btnFilter);
    actionBarLayout->addStretch();

    contentLayout->addLayout(actionBarLayout);

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

    contentLayout->addWidget(stackedWidget);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea);

    // --- ENHANCED MAIN WINDOW STYLESHEET ---
    this->setStyleSheet(R"(
        #contentArea { 
            background-color: #f8fafc; 
        }
        #sidebar { 
            background-color: #1e293b; 
            border-right: 1px solid #0f172a; 
        }
        
        #sidebar QPushButton {
            background-color: transparent;
            color: #94a3b8;
            border: none;
            border-radius: 8px;
            padding: 12px 20px;
            font-size: 15px;
            font-family: 'Segoe UI', Arial, sans-serif;
            font-weight: 600;
            text-align: left;
        }
        #sidebar QPushButton:hover { 
            background-color: #334155; 
            color: #f8fafc; 
        }
        #sidebar QPushButton[active="true"] { 
            background-color: #3b82f6; 
            color: #ffffff; 
            border-left: 5px solid #93c5fd; 
            border-top-left-radius: 4px;
            border-bottom-left-radius: 4px;
        }

        #contentArea QPushButton {
            background-color: #ffffff;
            color: #1e293b;
            border: 1px solid #cbd5e1;
            border-radius: 6px;
            padding: 8px 20px;
            font-size: 14px;
            font-weight: bold;
            font-family: 'Segoe UI', Arial, sans-serif;
        }
        #contentArea QPushButton:hover { 
            background-color: #f1f5f9; 
            border-color: #94a3b8;
            color: #3b82f6;
        }
        
        /* --- UPGRADED TABLE STYLES --- */
        QTableWidget {
            background-color: #ffffff;
            alternate-background-color: #f8fafc; /* Crisp, subtle alternating rows */
            border: 1px solid #cbd5e1;
            border-radius: 8px;
            gridline-color: #e2e8f0; 
            font-size: 14px;
            color: #1e293b;
            selection-background-color: #3b82f6; /* Vibrant blue selection */
            selection-color: #ffffff;
            outline: none; /* Removes the ugly dotted focus rectangle */
        }
        
        QTableWidget::item {
            padding: 5px; /* Gives the text some breathing room */
        }

        QTableWidget::item:hover {
            background-color: #f1f5f9; /* Soft gray hover effect on rows */
            color: #0f172a;
        }
        
        /* --- BOLD, PREMIUM HEADERS --- */
        QHeaderView::section:horizontal {
            background-color: #1e293b; /* Dark slate to match the sidebar */
            color: #ffffff;
            font-weight: bold;
            font-size: 14px;
            padding: 12px;
            border: none;
            border-right: 1px solid #334155; /* Subtle separator between columns */
        }

        QHeaderView::section:horizontal:first {
            border-top-left-radius: 8px; /* Rounds the top corners of the table */
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
            border-right: 1px solid #cbd5e1;
        }
        
        QTableCornerButton::section {
            background-color: #1e293b; /* Matches the top-left corner box to the header */
            border: none;
        }
    )");

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
    Backend::loadTableData(tableBooking, "SELECT * FROM Bookings");
    btnAdd->disconnect();
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::showAddBookingDialog);
}

void MainWindow::handleLogin_2()
{
    stackedWidget->setCurrentIndex(1);
    setActiveButton(button2);
    btnAdd->disconnect();
    Backend::loadTableData(tableBookingItems, "SELECT * FROM BookingServiceItems");
}

void MainWindow::handleLogin_3()
{
    stackedWidget->setCurrentIndex(2);
    setActiveButton(button3);
    Backend::loadTableData(tableCustomer, "SELECT * FROM Customer");
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::AddNewCustomerClicked);
    connect(btnUpdate, &QPushButton::clicked, this, &MainWindow::showUpdateCustomerDialog);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::showDeleteCustomerDialog);
    connect(btnFilter, &QPushButton::clicked, this, &MainWindow::showFilterCustomerDialog);
}

void MainWindow::handleLogin_4()
{
    stackedWidget->setCurrentIndex(3);
    setActiveButton(button4);
    Backend::loadTableData(tableFood, "SELECT * FROM FoodOptions");
    btnAdd->disconnect();
}

void MainWindow::handleLogin_5()
{
    stackedWidget->setCurrentIndex(4);
    setActiveButton(button5);
    Backend::loadTableData(tableInventory, "SELECT * FROM Inventory");
    btnAdd->disconnect();
}

void MainWindow::handleLogin_6()
{
    stackedWidget->setCurrentIndex(5);
    setActiveButton(button6);
    Backend::loadTableData(tableInventoryLog, "SELECT * FROM InventoryLog");
    btnAdd->disconnect();
}

void MainWindow::handleLogin_7()
{
    stackedWidget->setCurrentIndex(6);
    setActiveButton(button7);
    Backend::loadTableData(tableRoom, "SELECT * FROM ListRooms");

    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();

    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::showAddRoomDialog);
    connect(btnUpdate, &QPushButton::clicked, this, &MainWindow::showUpdateRoomDialog);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::showDeleteRoomDialog);
}

void MainWindow::handleLogin_8()
{
    stackedWidget->setCurrentIndex(7);
    setActiveButton(button8);
    Backend::loadTableData(tableRoomType, "SELECT * FROM RoomTypeCatalog");
    btnAdd->disconnect();
}

void MainWindow::handleLogin_9()
{
    stackedWidget->setCurrentIndex(8);
    setActiveButton(button9);
    Backend::loadTableData(tableService, "SELECT * FROM ListServiceItems");
    btnAdd->disconnect();
}

void MainWindow::handleLogin_10()
{
    stackedWidget->setCurrentIndex(9);
    setActiveButton(button10);
    Backend::loadTableData(tableBill, "SELECT * FROM Bills");
    btnAdd->disconnect();
}

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedSize(1000, 800);
    setWindowTitle("Hotel_Management");
    setObjectName("MyMainWindow");

    // --- PREMIUM DARK GRADIENT BACKGROUND ---
    setStyleSheet(
        "#MyMainWindow {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #1e293b, stop:1 #0f172a);"
        "}");

    window2 = new MainWindow();

    label1 = new QLabel("Welcome Back", this);
    QLabel *subLabel = new QLabel("Please log in to manage your hotel", this);

    inputBox_user = new QLineEdit(this);
    inputBox_pass = new QLineEdit(this);
    inputBox_pass->setEchoMode(QLineEdit::Password);

    button = new QPushButton("Sign In", this);

    imageLabel = new QLabel(this);
    l1 = new QLabel("© 2026 Premium Hotel Management System", this);

    QPixmap myImage("NTH.png");
    imageLabel->setPixmap(myImage);
    imageLabel->setScaledContents(true);
    imageLabel->setGeometry(440, 80, 120, 120); // Centered logo above title

    label1->setGeometry(0, 230, 1000, 60);
    label1->setAlignment(Qt::AlignCenter);

    subLabel->setGeometry(0, 290, 1000, 30);
    subLabel->setAlignment(Qt::AlignCenter);

    inputBox_user->setGeometry(300, 370, 400, 55); // Slightly taller
    inputBox_pass->setGeometry(300, 445, 400, 55);

    button->setGeometry(300, 530, 400, 55); // Full width button to match inputs

    l1->setGeometry(0, 720, 1000, 40);
    l1->setAlignment(Qt::AlignCenter);

    // --- ELEGANT TYPOGRAPHY ---
    label1->setStyleSheet("font-size: 42px; font-weight: bold; color: #ffffff; font-family: 'Segoe UI', Arial;");
    subLabel->setStyleSheet("font-size: 18px; color: #94a3b8; font-family: 'Segoe UI', Arial;");
    l1->setStyleSheet("font-size: 13px; color: #475569;");

    // --- SLEEK INPUT STYLES ---
    QString inputStyle =
        "background-color: rgba(255, 255, 255, 0.05); "
        "border: 1px solid rgba(255, 255, 255, 0.2); "
        "border-radius: 27px; "
        "padding: 10px 25px; "
        "font-size: 16px; "
        "color: #ffffff;"
        "selection-background-color: #3b82f6;";

    inputBox_user->setStyleSheet(inputStyle + "QLineEdit:focus { border: 1px solid #60a5fa; background-color: rgba(255, 255, 255, 0.1); }");
    inputBox_pass->setStyleSheet(inputStyle + "QLineEdit:focus { border: 1px solid #60a5fa; background-color: rgba(255, 255, 255, 0.1); }");

    inputBox_user->setPlaceholderText("Username");
    inputBox_pass->setPlaceholderText("Password");

    // --- MODERN GRADIENT BUTTON ---
    button->setStyleSheet(
        "QPushButton {"
        "   background-color: #3b82f6; "
        "   color: white; "
        "   border: none; "
        "   border-radius: 27px; "
        "   font-size: 18px; "
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #2563eb; "
        "}"
        "QPushButton:pressed {"
        "   background-color: #1d4ed8; "
        "}");

    connect(button, &QPushButton::clicked, this, &LoginWindow::handleLogin);
}

LoginWindow::~LoginWindow()
{
    delete window2;
}

void LoginWindow::handleLogin()
{
    QString user = inputBox_user->text();
    QString pass = inputBox_pass->text();

    if (user == "admin" && pass == "admin123")
    {
        MainWindow *mainWindow = new MainWindow();
        mainWindow->show();
        this->close();
    }
    else if (user == "customer" && pass == "123456")
    {
        CustomerInputWindow *customerWindow = new CustomerInputWindow();
        customerWindow->show();
        this->close();
    }
    else
    {
        QMessageBox::warning(this, "Error", "Incorrect usename or password!");
    }
}

void MainWindow::AddNewCustomerClicked()
{
    QDialog *addDialog = new QDialog(this);

    addDialog->setStyleSheet(
        "QDialog { background-color: #f8fafc; }"
        "QLabel { color: #334155; font-weight: bold; font-size: 14px; }");
    addDialog->setWindowTitle("Add Customer");
    addDialog->setFixedSize(400, 420); // Slightly larger for breathing room

    QVBoxLayout *mainLayout = new QVBoxLayout(addDialog);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("Customer's Information", addDialog);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #1e293b; margin-bottom: 20px;");
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    QString inputStyle = "QLineEdit { border: 1px solid #cbd5e1; border-radius: 8px; padding: 10px; font-size: 14px; color: #1e293b; background-color: #ffffff; }"
                         "QLineEdit:focus { border: 2px solid #3b82f6; }";

    QLineEdit *txtName = new QLineEdit(addDialog);
    txtName->setPlaceholderText("Type your name...");
    txtName->setStyleSheet(inputStyle);

    QLineEdit *ID = new QLineEdit(addDialog);
    ID->setPlaceholderText("Type your ID:...");
    ID->setStyleSheet(inputStyle);

    QLineEdit *txtPhone = new QLineEdit(addDialog);
    txtPhone->setPlaceholderText("Type your Phone number...");
    txtPhone->setStyleSheet(inputStyle);

    QLineEdit *txtType = new QLineEdit(addDialog);
    txtType->setPlaceholderText("Type Customer...");
    txtType->setStyleSheet(inputStyle);

    QLineEdit *txtPoint = new QLineEdit(addDialog);
    txtPoint->setPlaceholderText("Type your point...");
    txtPoint->setStyleSheet(inputStyle);

    formLayout->addRow(new QLabel("Customer's name:", addDialog), txtName);
    formLayout->addRow(new QLabel("ID:", addDialog), ID);
    formLayout->addRow(new QLabel("Phone number:", addDialog), txtPhone);
    formLayout->addRow(new QLabel("Type:", addDialog), txtType);
    formLayout->addRow(new QLabel("Point:", addDialog), txtPoint);

    mainLayout->addLayout(formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnSave = new QPushButton("Save", addDialog);
    QPushButton *btnCancel = new QPushButton("Cancel", addDialog);

    btnSave->setStyleSheet("background-color: #10b981; color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #e2e8f0; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

    // Quick hover effects for dialog buttons
    btnSave->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);
    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, addDialog, &QDialog::reject);

    connect(btnSave, &QPushButton::clicked, [=]()
            {
            QString name = txtName->text();
            QString phone = txtPhone->text();
            QString IDcard = ID->text();
            QString type = txtType->text();
            QString point = txtPoint->text();
    
            if (name.isEmpty() || phone.isEmpty())
            {
                QMessageBox::warning(addDialog, "Error", "Please input your name or phone number");
                return;
            }
    
                    CustomerRepository re;
                    Customer a(name.toStdString(), phone.toStdString(),IDcard.toStdString());
                    a.setPoint(point.toInt());

                    bool success = re.add(a);
    
                    if (success) {
                        QMessageBox::information(addDialog, "Successfully", "Successfully add new customer");
                        addDialog->accept();
                        handleLogin_3();
                    } else {
                        QMessageBox::critical(addDialog, "Error", "Can not save into database");
                    } });

    addDialog->exec();
    addDialog->deleteLater();
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

void MainWindow::showAddBookingDialog()
{
    QDialog *addDialog = new QDialog(this);
    addDialog->setWindowTitle("Add Booking");
    // Increased height from 420 to 520 to fit the new Phone and ID fields
    addDialog->setFixedSize(450, 560);

    addDialog->setStyleSheet(
        "QDialog { background-color: #f8fafc; }"
        "QLabel { color: #334155; font-weight: bold; font-size: 14px; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(addDialog);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("Booking Information", addDialog);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #1e293b; margin-bottom: 20px;");
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    QString inputStyle = "QLineEdit, QDateEdit { border: 1px solid #cbd5e1; border-radius: 8px; padding: 10px; font-size: 14px; color: #1e293b; background-color: white; }"
                         "QLineEdit:focus, QDateEdit:focus { border: 2px solid #3b82f6; }";

    // --- NEW FIELDS: ID and Phone ---
    QLineEdit *txtId = new QLineEdit(addDialog);
    txtId->setPlaceholderText("Customer ID ...");
    txtId->setStyleSheet(inputStyle);

    QLineEdit *txtCustomer = new QLineEdit(addDialog);
    txtCustomer->setPlaceholderText("Customer Name...");
    txtCustomer->setStyleSheet(inputStyle);

    QLineEdit *txtPhone = new QLineEdit(addDialog);
    txtPhone->setPlaceholderText("Phone Number...");
    txtPhone->setStyleSheet(inputStyle);

    // --- EXISTING FIELDS ---
    QLineEdit *txtRoom = new QLineEdit(addDialog);
    txtRoom->setPlaceholderText("Room ID...");
    txtRoom->setStyleSheet(inputStyle);

    QDateEdit *dateCheckIn = new QDateEdit(QDate::currentDate(), addDialog);
    dateCheckIn->setCalendarPopup(true);
    dateCheckIn->setStyleSheet(inputStyle);

    QDateEdit *dateCheckOut = new QDateEdit(QDate::currentDate().addDays(1), addDialog);
    dateCheckOut->setCalendarPopup(true);
    dateCheckOut->setStyleSheet(inputStyle);

    QLineEdit *txtPrice = new QLineEdit(addDialog);
    txtPrice->setPlaceholderText("Total Price ($)...");
    txtPrice->setStyleSheet(inputStyle);

    formLayout->addRow(new QLabel("Customer ID:", addDialog), txtId);
    formLayout->addRow(new QLabel("Customer Name:", addDialog), txtCustomer);
    formLayout->addRow(new QLabel("Phone Number:", addDialog), txtPhone);
    formLayout->addRow(new QLabel("Room ID:", addDialog), txtRoom);
    formLayout->addRow(new QLabel("Check-In:", addDialog), dateCheckIn);
    formLayout->addRow(new QLabel("Check-Out:", addDialog), dateCheckOut);
    formLayout->addRow(new QLabel("Total Price:", addDialog), txtPrice);

    mainLayout->addLayout(formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnSave = new QPushButton("Save", addDialog);
    QPushButton *btnCancel = new QPushButton("Cancel", addDialog);

    btnSave->setStyleSheet("background-color: #3b82f6; color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #e2e8f0; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

    btnSave->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);
    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, addDialog, &QDialog::reject);

    connect(btnSave, &QPushButton::clicked, [=]()
            {
        QString id = txtId->text();
        QString customer = txtCustomer->text();
        QString phone = txtPhone->text();
        QString room = txtRoom->text();
        QString price = txtPrice->text();
        
        QString checkInDate = dateCheckIn->date().toString("yyyy-MM-dd");
        QString checkOutDate = dateCheckOut->date().toString("yyyy-MM-dd");

        if (id.isEmpty() || customer.isEmpty() || phone.isEmpty() || room.isEmpty()) {
            QMessageBox::warning(addDialog, "Error", "Please fill in all customer and room details!");
            return; 
        }

        CustomerRepository re;
       Customer a(customer.toStdString(), phone.toStdString(), id.toStdString());
        re.add(a);

        QSqlDatabase db = DatabaseManager::instance().database();
        QSqlQuery query(db);

        query.prepare("UPDATE ListRooms SET Status = 1 WHERE room_id = ?");
        query.addBindValue(room);
        
        if (!query.exec()) {
            QMessageBox::critical(addDialog, "Database Error", "Failed to update Room status:\n" + query.lastError().text());
            return;
        }

        // (Optional but Recommended) Insert into Bookings table if you have one
        /*
        query.prepare("INSERT INTO Bookings (CustomerID, RoomID, CheckInDate, Status) VALUES (?, ?, ?, 'Pending')");
        query.addBindValue(id);
        query.addBindValue(room);
        query.addBindValue(checkInDate);
        query.exec();
        */
       BookingRepository r;

        BookingData t;
        t.customerId = 100;
        t.roomNumber = room; 
        t.checkInTime = checkInDate;
        t.checkOutTime = checkOutDate;
        r.add(t);


        QMessageBox::information(addDialog, "Success", "Booking created and room status updated successfully!");
        addDialog->accept(); });

    addDialog->exec();
    addDialog->deleteLater();
}
void MainWindow::showAddRoomDialog()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Add room");
    dialog->setFixedSize(420, 500); // More height for neatness

    dialog->setStyleSheet(
        "QDialog { background-color: #f8fafc; }"
        "QLabel { color: #334155; font-weight: bold; font-size: 14px; }"
        "QMessageBox { background-color: white; }"
        "QMessageBox QLabel { color: #333333; font-size: 13px; font-weight: normal; }"
        "QMessageBox QPushButton { background-color: #e2e8f0; color: #1e293b; border: none; border-radius: 4px; padding: 6px 20px; font-weight: bold; }");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("Room Details", dialog);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #1e293b; margin-bottom: 15px;");
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *form = new QFormLayout();
    form->setSpacing(15);

    QString inputStyle = "QLineEdit, QComboBox { border: 1px solid #cbd5e1; border-radius: 8px; padding: 10px; background-color: white; color: #1e293b; font-size: 14px; }"
                         "QLineEdit:focus, QComboBox:focus { border: 2px solid #3b82f6; }";

    QLineEdit *txtId = new QLineEdit(dialog);
    txtId->setPlaceholderText("EX: R101, R102...");
    txtId->setStyleSheet(inputStyle);

    QLineEdit *txtNumber = new QLineEdit(dialog);
    txtNumber->setPlaceholderText("EX: 101, 102...");
    txtNumber->setStyleSheet(inputStyle);

    QComboBox *cbType = new QComboBox(dialog);
    cbType->addItems({"Standard", "VIP", "President"});
    cbType->setStyleSheet(inputStyle);

    QComboBox *cbStatus = new QComboBox(dialog);
    cbStatus->addItems({"Available", "Reserved", "Occupied", "Maintenance"});
    cbStatus->setStyleSheet(inputStyle);

    QLineEdit *txtPrice = new QLineEdit(dialog);
    txtPrice->setPlaceholderText("EX: 500000");
    txtPrice->setStyleSheet(inputStyle);

    QLineEdit *txtPeople = new QLineEdit(dialog);
    txtPeople->setPlaceholderText("EX: 2, 4...");
    txtPeople->setStyleSheet(inputStyle);

    form->addRow("Room ID:", txtId);
    form->addRow("Room number:", txtNumber);
    form->addRow("Type Room:", cbType);
    form->addRow("Status:", cbStatus);
    form->addRow("Base price:", txtPrice);
    form->addRow("Capacity:", txtPeople);

    layout->addLayout(form);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnSave = new QPushButton("Save", dialog);
    QPushButton *btnCancel = new QPushButton("Cancel", dialog);

    btnSave->setStyleSheet("background-color: #10b981; color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #e2e8f0; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

    btnSave->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);
    layout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);

    connect(btnSave, &QPushButton::clicked, [=]()
            {
        if (txtId->text().isEmpty() || txtNumber->text().isEmpty() || txtPrice->text().isEmpty() || txtPeople->text().isEmpty()) {
            QMessageBox::warning(dialog, "Error", "Please fill up information");
            return;
        }

        StandardRoom* newRoom = new StandardRoom();        
        newRoom->setRoomNumber(txtNumber->text().toStdString());

        QString statusText = cbStatus->currentText();
        RoomStatus statusEnum;

        if (statusText == "Available") {
            statusEnum = RoomStatus::Available; 
        } 
        else if (statusText == "Reserved") {
            statusEnum = RoomStatus::Maintenance; 
        } 
        else if (statusText == "Occupied") {
            statusEnum = RoomStatus::Occupied; 
        } 
        else {
            statusEnum = RoomStatus::Maintenance;
        }
        newRoom->setStatus(statusEnum); 
        newRoom->setBasePrice(txtPrice->text().toInt());
        
        newRoom->setNumberPeople(txtPeople->text().toInt());


        
        RoomRepository repo;
        bool success = repo.add(newRoom);

        delete newRoom;

        if (success) {
            QMessageBox::information(dialog, "Successfully", "Successfully add new room");
            dialog->accept();
            
            handleLogin_7(); 
        } else {
            QMessageBox::critical(dialog, "Error", "Can save into room id");
        } });

    dialog->exec();
    dialog->deleteLater();
}

void MainWindow::showUpdateCustomerDialog()
{
    handleLogin_3();
}

void MainWindow::showDeleteCustomerDialog()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Delete Customer");
    dialog->setFixedSize(380, 220);

    dialog->setStyleSheet(
        "QDialog { background-color: #f8fafc; }"
        "QLabel { color: #334155; font-weight: bold; font-size: 14px;}");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(30, 20, 30, 20);

    QLabel *titleLabel = new QLabel("Remove Customer", dialog);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #e11d48; margin-bottom: 10px;");
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *form = new QFormLayout();

    QLineEdit *txtId = new QLineEdit(dialog);
    txtId->setPlaceholderText("Enter Customer ID...");
    txtId->setStyleSheet("border: 1px solid #cbd5e1; border-radius: 8px; padding: 10px; font-size: 14px; background: white; color: #1e293b;");

    form->addRow("Customer ID:", txtId);
    layout->addLayout(form);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnDelete = new QPushButton("Delete", dialog);
    QPushButton *btnCancel = new QPushButton("Cancel", dialog);

    btnDelete->setStyleSheet("background-color: #e11d48; color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 14px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #e2e8f0; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 14px; font-weight: bold;");

    btnDelete->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnDelete);
    layout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);

    connect(btnDelete, &QPushButton::clicked, [=]()
            {
        if (txtId->text().isEmpty()) {
            QMessageBox::warning(dialog, "Error", "Please enter a Customer ID!");
            return;
        }

        QMessageBox::StandardButton reply = QMessageBox::question(dialog, "Confirm", "Are you sure you want to delete this customer?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) return;

        CustomerRepository repo;
        bool success = repo.remove(txtId->text().toStdString());

        if (success) {
            QMessageBox::information(dialog, "Success", "Customer deleted successfully!");
            dialog->accept();
            handleLogin_3();
        } else {
            QMessageBox::critical(dialog, "Error", "Cannot delete customer. It may not exist.");
        } });

    dialog->exec();
    dialog->deleteLater();
}

void MainWindow::showFilterCustomerDialog()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Filter Customers");
    dialog->setFixedSize(420, 250);

    dialog->setStyleSheet(
        "QDialog { background-color: #f8fafc; }"
        "QLabel { color: #334155; font-weight: bold; font-size: 14px; }");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(30, 20, 30, 20);

    QLabel *titleLabel = new QLabel("Advanced Search", dialog);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #1e293b; margin-bottom: 15px;");
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *form = new QFormLayout();

    QString inputStyle = "QLineEdit, QComboBox { border: 1px solid #cbd5e1; border-radius: 8px; padding: 10px; font-size: 14px; background: white; color: #1e293b; }";

    QComboBox *cbCriteria = new QComboBox(dialog);
    cbCriteria->addItem("Customer Name", "full_name");
    cbCriteria->addItem("Customer ID", "id_customer");
    cbCriteria->addItem("Phone Number", "phone_number");
    cbCriteria->addItem("Room ID", "id_room");
    cbCriteria->setStyleSheet(inputStyle);

    QLineEdit *txtValue = new QLineEdit(dialog);
    txtValue->setPlaceholderText("Enter search value...");
    txtValue->setStyleSheet(inputStyle);

    form->addRow("Search By:", cbCriteria);
    form->addRow("Keyword:", txtValue);
    layout->addLayout(form);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnSearch = new QPushButton("Search", dialog);
    QPushButton *btnCancel = new QPushButton("Cancel", dialog);

    btnSearch->setStyleSheet("background-color: #3b82f6; color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 14px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #e2e8f0; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 14px; font-weight: bold;");

    btnSearch->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSearch);
    layout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);

    connect(btnSearch, &QPushButton::clicked, [=]()
            {
        if (txtValue->text().isEmpty())
        {
            QMessageBox::warning(dialog, "Error", "Please enter a value to search!");
            return;
        }

        QString columnToSearch = cbCriteria->currentData().toString();
        QString searchValue = txtValue->text();

        CustomerRepository repo;
        std::vector<Customer> filteredCustomers = repo.filter(columnToSearch, searchValue);

        if (filteredCustomers.empty())
        {
            QMessageBox::information(dialog, "Result", "No customers found matching your criteria.");
        }
        else
        {
            tableCustomer->setRowCount(0);

            int row = 0;
            for (auto &cus : filteredCustomers)
            {
                tableCustomer->insertRow(row);

                tableCustomer->setItem(row, 0, new QTableWidgetItem(cus.getId()));
                 tableCustomer->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(cus.getIdcard())));
                
                tableCustomer->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(cus.getFullname())));
                tableCustomer->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(cus.getPhone())));
                
                
                tableCustomer->setItem(row, 4, new QTableWidgetItem(cus.getTier() ));
                tableCustomer->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(cus.getIdRoom())));

                row++;
            }

            QMessageBox::information(dialog, "Success", QString("Found %1 customers!").arg(filteredCustomers.size()));
            dialog->accept();
        } });

    dialog->exec();
    dialog->deleteLater();
}

void MainWindow::showDeleteRoomDialog()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Delete Room");
    dialog->setFixedSize(380, 220);

    dialog->setStyleSheet(
        "QDialog { background-color: #f8fafc; }"
        "QLabel { color: #334155; font-weight: bold; font-size: 14px; }");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(30, 20, 30, 20);

    QLabel *titleLabel = new QLabel("Remove Room", dialog);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #e11d48; margin-bottom: 10px;");
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *form = new QFormLayout();

    QLineEdit *txtId = new QLineEdit(dialog);
    txtId->setPlaceholderText("Enter Room Number...");
    txtId->setStyleSheet("border: 1px solid #cbd5e1; border-radius: 8px; padding: 10px; font-size: 14px; background: white; color: #1e293b;");

    form->addRow("Room Number:", txtId);
    layout->addLayout(form);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnDelete = new QPushButton("Delete", dialog);
    QPushButton *btnCancel = new QPushButton("Cancel", dialog);

    btnDelete->setStyleSheet("background-color: #e11d48; color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 14px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #e2e8f0; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 14px; font-weight: bold;");

    btnDelete->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnDelete);
    layout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);

    connect(btnDelete, &QPushButton::clicked, dialog, [=]()
            {
        if (txtId->text().isEmpty()) {
            QMessageBox::warning(dialog, "Error", "Please enter a Room Number!");
            return;
        }

        QMessageBox::StandardButton reply = QMessageBox::question(dialog, "Confirm", "Are you sure you want to delete this room?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) return;

        RoomRepository repo;
        
        bool success = repo.remove(txtId->text().toStdString());

        if (success) {
            QMessageBox::information(dialog, "Success", "Room deleted successfully!");
            dialog->accept();
            handleLogin_7(); 
        } else {
            QMessageBox::critical(dialog, "Error", "Cannot delete room. It may not exist or is currently booked.");
        } });

    dialog->exec();
    dialog->deleteLater();
}

void MainWindow::showUpdateRoomDialog()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Update Room");
    dialog->setFixedSize(420, 420);

    dialog->setStyleSheet(
        "QDialog { background-color: #f8fafc; }"
        "QLabel { color: #334155; font-weight: bold; font-size: 14px; }"
        "QMessageBox { background-color: white; }"
        "QMessageBox QLabel { color: #333333; font-size: 13px; font-weight: normal; }"
        "QMessageBox QPushButton { background-color: #e2e8f0; color: #1e293b; border: none; border-radius: 4px; padding: 6px 20px; font-weight: bold; }"
        "QMessageBox QPushButton:hover { background-color: #cbd5e1; }");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(30, 20, 30, 20);

    QLabel *titleLabel = new QLabel("Update Room Details", dialog);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #3b82f6; margin-bottom: 15px;");
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *form = new QFormLayout();
    form->setSpacing(15);

    QString inputStyle = "QLineEdit, QComboBox { border: 1px solid #cbd5e1; border-radius: 8px; padding: 10px; background-color: white; color: #1e293b; font-size: 14px; }"
                         "QLineEdit:focus, QComboBox:focus { border: 2px solid #3b82f6; }";

    QLineEdit *txtNumber = new QLineEdit(dialog);
    txtNumber->setPlaceholderText("Target Room Number...");
    txtNumber->setStyleSheet(inputStyle);

    QComboBox *cbStatus = new QComboBox(dialog);
    cbStatus->addItems({"Available", "Reserved", "Occupied", "Maintenance"});
    cbStatus->setStyleSheet(inputStyle);

    QLineEdit *txtPrice = new QLineEdit(dialog);
    txtPrice->setPlaceholderText("New Base Price...");
    txtPrice->setStyleSheet(inputStyle);

    QLineEdit *txtPeople = new QLineEdit(dialog);
    txtPeople->setPlaceholderText("New Capacity...");
    txtPeople->setStyleSheet(inputStyle);

    form->addRow("Room Number:", txtNumber);
    form->addRow("New Status:", cbStatus);
    form->addRow("New Base price:", txtPrice);
    form->addRow("New Capacity:", txtPeople);

    layout->addLayout(form);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnSave = new QPushButton("Update", dialog);
    QPushButton *btnCancel = new QPushButton("Cancel", dialog);

    btnSave->setStyleSheet("background-color: #3b82f6; color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 14px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #e2e8f0; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 14px; font-weight: bold;");

    btnSave->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);
    layout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);

    connect(btnSave, &QPushButton::clicked, dialog, [=]()
            {
        if (txtNumber->text().isEmpty() || txtPrice->text().isEmpty() || txtPeople->text().isEmpty()) {
            QMessageBox::warning(dialog, "Error", "Please fill up all information");
            return;
        }

        StandardRoom* updatedRoom = new StandardRoom();
        updatedRoom->setRoomNumber(txtNumber->text().toStdString());

        QString statusText = cbStatus->currentText();
        RoomStatus statusEnum;

        if (statusText == "Available") {
            statusEnum = RoomStatus::Available; 
        } 
        else if (statusText == "Reserved") {
            statusEnum = RoomStatus::Maintenance; 
        } 
        else if (statusText == "Occupied") {
            statusEnum = RoomStatus::Occupied; 
        } 
        else {
            statusEnum = RoomStatus::Maintenance;
        }
        
        updatedRoom->setStatus(statusEnum); 
        updatedRoom->setBasePrice(txtPrice->text().toInt());
        updatedRoom->setNumberPeople(txtPeople->text().toInt());

        RoomRepository repo;
        bool success = repo.update(updatedRoom);

        delete updatedRoom;
        if (success) {
            QMessageBox::information(dialog, "Successfully", "Successfully updated the room");
            dialog->accept();
            handleLogin_7();
        } else {
            QMessageBox::critical(dialog, "Error", "Cannot update room. It may not exist.");
        } });

    dialog->exec();
    dialog->deleteLater();
}