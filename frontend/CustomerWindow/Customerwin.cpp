// Đây là file để customer nhập thông tin và nhập phòng
// FIle này sẽ liên quan đến customer,room,booking . respotory
#include "customerwin.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include "Booking/Booking.h"
#include "Repository/BookingRepository.h"
#include <QDebug>
#include <QDate>
#include "Customer/Customer.h"
#include "Repository/CustomerRepository.h"
#include <iostream>
#include <memory>
#include "Manager/DatabaseManager.h"

using namespace std;

// Hàm này để chứa thông tin nhập customer infomation
CustomerInputWindow::CustomerInputWindow(QWidget *parent) : QWidget(parent)
{
    setFixedSize(800, 600);
    setWindowTitle("Customer Information");

    setStyleSheet(
        "QWidget { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:0.5 #e0f2fe, stop:1 #ffffff); color: #1e293b; font-family: 'Segoe UI', Arial, sans-serif; }"

        "QCalendarWidget QWidget { background-color: #ffffff; }"
        "QCalendarWidget QToolButton { "
        "   color: #4f46e5; font-weight: bold; font-size: 14px; background-color: #e0e7ff; border-radius: 6px; padding: 4px;"
        "}"
        "QCalendarWidget QToolButton:hover { background-color: #c7d2fe; }"
        "QCalendarWidget QMenu { background-color: white; color: #3730a3; }"
        "QCalendarWidget QSpinBox { background-color: #f0f9ff; color: #3730a3; border: 1px solid #bae6fd; border-radius: 4px; }"

        "QCalendarWidget QAbstractItemView:enabled { "
        "   color: #1e293b; background-color: #ffffff; "
        "   selection-background-color: #38bdf8; "
        "   selection-color: white; "
        "}"
        "QCalendarWidget QAbstractItemView:disabled { color: #cbd5e1; }");
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(50, 40, 50, 40);

    QLabel *titleLabel = new QLabel("Guest Registration", this);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #3730a3; margin-bottom: 20px; background: transparent;"); // Transparent background for label
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *form = new QFormLayout();
    form->setSpacing(20);

    QString inputStyle =
        "QLineEdit, QDateEdit, QSpinBox {"
        "   background-color: #ffffff; "
        "   border: 2px solid #38bdf8; "
        "   border-radius: 8px; "
        "   padding: 12px; "
        "   font-size: 15px; "
        "   color: #0f172a; "
        "}"
        "QLineEdit:hover, QDateEdit:hover, QSpinBox:hover { border: 2px solid #0284c7; }"
        "QLineEdit:focus, QDateEdit:focus, QSpinBox:focus { border: 2px solid #0369a1; background-color: #f0f9ff; }";

    // Style for the form labels
    QString labelStyle = "font-size: 15px; font-weight: 600; color: #475569; background: transparent;";

    // CHỗ nhập thông tin khách hàng
    //"-----------------------"

    txtName = new QLineEdit(this);
    txtName->setStyleSheet(inputStyle);
    txtName->setPlaceholderText("Enter full name");

    txtPhone = new QLineEdit(this);
    txtPhone->setStyleSheet(inputStyle);
    txtPhone->setPlaceholderText("Enter phone number");

    dateCheckIn = new QDateEdit(QDate::currentDate(), this);
    dateCheckIn->setCalendarPopup(true);
    dateCheckIn->setStyleSheet(inputStyle);

    datecheckout = new QDateEdit(QDate::currentDate().addDays(1), this); // Default to tomorrow
    datecheckout->setCalendarPopup(true);
    datecheckout->setStyleSheet(inputStyle);

    spinPeople = new QSpinBox(this);
    spinPeople->setMinimum(1);
    spinPeople->setMaximum(10);
    spinPeople->setStyleSheet(inputStyle);

    ID = new QLineEdit(this);
    ID->setStyleSheet(inputStyle);
    ID->setPlaceholderText("Enter ID or Passport number");

    //"-----------------------"

    QLabel *lblName = new QLabel("Your name:", this);
    lblName->setStyleSheet(labelStyle);
    QLabel *lblID = new QLabel("Your ID Card:", this);
    lblID->setStyleSheet(labelStyle);
    QLabel *lblPhone = new QLabel("Phone number:", this);
    lblPhone->setStyleSheet(labelStyle);
    QLabel *lblCheckIn = new QLabel("Check-in day:", this);
    lblCheckIn->setStyleSheet(labelStyle);
    QLabel *lblCheckOut = new QLabel("Check-out day:", this);
    lblCheckOut->setStyleSheet(labelStyle);
    QLabel *lblPeople = new QLabel("Number of people:", this);
    lblPeople->setStyleSheet(labelStyle);

    form->addRow(lblName, txtName);
    form->addRow(lblID, ID);
    form->addRow(lblPhone, txtPhone);
    form->addRow(lblCheckIn, dateCheckIn);
    form->addRow(lblCheckOut, datecheckout);
    form->addRow(lblPeople, spinPeople);

    layout->addLayout(form);

    layout->addStretch(); // Pushes the button to the bottom nicely

    btnNext = new QPushButton("Find Available Rooms", this);
    btnNext->setCursor(Qt::PointingHandCursor);

    // --- ĐỒNG BỘ NÚT VIBRANT GRADIENT ---
    btnNext->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366f1, stop:1 #8b5cf6); color: white; padding: 14px; font-size: 16px; font-weight: bold; border-radius: 8px; border: none; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4f46e5, stop:1 #7c3aed); }");
    layout->addWidget(btnNext);

    connect(btnNext, &QPushButton::clicked, this, &CustomerInputWindow::onNextClicked);
}

// Hàm kiểm tra thông tin khách hàng ghi click

void CustomerInputWindow::onNextClicked()
{

    // Kiểm tra name và phone được fill chưa
    if (txtName->text().isEmpty() || txtPhone->text().isEmpty())
    {
        QMessageBox::warning(this, "Warning", "Please input name and phone");
        return;
    }

    // Kiểm tra điều kiện id
    if (ID->text().toStdString().size() != 10)
    {
        QMessageBox::warning(this, "Input Error", "Error: ID Card must be 10 digits long.");
        return;
    }

    for (char g : ID->text().toStdString())
    {
        if (!std::isdigit(g))
        {
            QMessageBox::warning(this, "Input Error", "Error:ID Card must contain only numbers.");
            return;
        }
    }

    // Kiểm tra điều kiện phone
    if (txtPhone->text().toStdString().size() != 10)
    {
        QMessageBox::warning(this, "Input Error", "Error: Phone Number must be 10 digits long.");
        return;
    }

    for (char g : txtPhone->text().toStdString())
    {
        if (!std::isdigit(g))
        {
            QMessageBox::warning(this, "Input Error", "Error: Phone number must contain only numbers.");
            return;
        }
    }

    // Lấy dữ liệu
    QString name = txtName->text();
    QString phone = txtPhone->text();
    QString id = ID->text();
    QString checkInDate = dateCheckIn->date().toString("yyyy-MM-dd");
    QString checkOutDate = datecheckout->date().toString("yyyy-MM-dd");
    int people = spinPeople->value();

    // Mở Window tìm phòng và truyền dữ liệu sang
    CustomerWindow *roomWindow = new CustomerWindow(name, phone, id, checkInDate, checkOutDate, people);
    roomWindow->show();

    // Đóng Window
    this->close();
}

// Hàm này là constructor của UI chọn phòng( nhận input là các thông tin customer)
CustomerWindow::CustomerWindow(QString name, QString phone, QString id, QString date, QString dateout, int people, QWidget *parent)
    : QWidget(parent), customerName(name), ID(id), customerPhone(phone), checkInDate(date), datecheckout(dateout), numPeople(people)
{
    setFixedSize(850, 650);
    setWindowTitle("Select a Room");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    QLabel *titleLabel = new QLabel("Rooms", this);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #3730a3; margin-bottom: 10px; font-family: 'Segoe UI', Arial, sans-serif; background: transparent;");
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    tableRoom = new QTableWidget(0, 5, this);
    // Display những thông tin trên UI
    tableRoom->setHorizontalHeaderLabels({"Room ID", "Room Number", "Type Room", "Price", "Capacity"});
    tableRoom->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableRoom->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableRoom->setSelectionMode(QAbstractItemView::SingleSelection);
    tableRoom->verticalHeader()->setDefaultSectionSize(45);
    tableRoom->setAlternatingRowColors(true);

    this->setStyleSheet(R"(
        QWidget { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:0.5 #e0f2fe, stop:1 #ffffff); color: #1e293b; }
        QLabel { background: transparent; }
        
        QTableWidget {
            background-color: #ffffff;
            alternate-background-color: #f0f9ff;
            border: 1px solid #bae6fd;
            border-radius: 8px;
            gridline-color: #e0f2fe; 
            font-size: 14px;
            color: #0f172a;
            selection-background-color: #38bdf8;
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

    layout->addWidget(tableRoom);

    btnBook = new QPushButton("Confirm Booking", this);
    btnBook->setCursor(Qt::PointingHandCursor);

    btnBook->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366f1, stop:1 #8b5cf6); color: white; padding: 14px; font-size: 16px; font-weight: bold; border-radius: 8px; border: none; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4f46e5, stop:1 #7c3aed); }");
    layout->addWidget(btnBook);

    connect(btnBook, &QPushButton::clicked, this, &CustomerWindow::onBookRoomClicked);

    // filter room
    loadFilteredRooms();
}

// Hàm này filter room ở cửa sổ cusyomer
void CustomerWindow::loadFilteredRooms()
{
    if (!DatabaseManager::instance().open())
    {
        qDebug() << "Error: Could not open database\n";
        return;
    }

    tableRoom->setRowCount(0);
    QSqlQuery query(DatabaseManager::instance().database());

    // Điều kiện nếu status = là Avaliable và số người lớn hơn
    query.prepare("SELECT room_id, room_number, room_type, base_price, number_people "
                  "FROM ListRooms "
                  "WHERE status IN (0, 'Available') "
                  "  AND number_people >= ?");
    query.addBindValue(numPeople);

    if (!query.exec())
    {
        qDebug() << "Error executing query:" << query.lastError().text();
        return;
    }

    int row = 0;
    while (query.next())
    {
        tableRoom->insertRow(row);
        tableRoom->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        tableRoom->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        tableRoom->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
        tableRoom->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
        tableRoom->setItem(row, 4, new QTableWidgetItem(query.value(4).toString()));
        row++;
    }
}

void CustomerWindow::onBookRoomClicked()
{
    int selectedRow = tableRoom->currentRow();
    if (selectedRow < 0)
    {
        QMessageBox::warning(this, "Warning", "Please choose at least one room for booking");
        return;
    }

    // Lấy roomid
    QString roomId = tableRoom->item(selectedRow, 0)->text();
    double price = tableRoom->item(selectedRow, 3)->text().toDouble();

    QSqlDatabase db = DatabaseManager::instance().database();
    qDebug() << db.databaseName() << '\n';

    if (!db.isOpen())
    {
        QMessageBox::critical(this, "Database Error", "Database is not open!");
        return;
    }

    db.transaction();

    // Lưu biến để lưu thông tin customer

    QString finalCustomerId = ID;
    bool isExistingCustomer = false;
    int currentPoints = 0;
    QSqlQuery checkCustomer(db);

    // CUstomer repository
    CustomerRepository customerRepo;
    Customer newCustomer;

    // Tạo query kiểm tra query có tồn tại trong database chưa( có rồi thì copy point)
    QString sqlString = QString("SELECT id_customer, points FROM Customer WHERE id_customer = '%1'").arg(ID);

    // Kiểm tra xem có tồn tại không
    if (checkCustomer.next())
    {
        finalCustomerId = checkCustomer.value(":id_customer").toString();
        currentPoints = checkCustomer.value("points").toInt();
        isExistingCustomer = true;
    }

    newCustomer.setIdcard(finalCustomerId.toStdString());
    newCustomer.setFullname(customerName.toStdString());
    newCustomer.setPhone(customerPhone.toStdString());
    newCustomer.setPoint(currentPoints);
    newCustomer.setIdroom(roomId.toStdString());

    // add vào database
    if (!customerRepo.add(newCustomer))
    {
        db.rollback();
        QMessageBox::critical(this, "Database Error", "Failed to create new customer via Repository!");
        return;
    }
    currentPoints = 0;

    BookingData bookingData;
    BookingRepository sp;

    bookingData.customerId = finalCustomerId;
    bookingData.roomNumber = roomId;
    bookingData.checkInTime = checkInDate;
    bookingData.checkOutTime = datecheckout;
    // Khúc này lấy tiền theo type room
    bookingData.totalPrice = (price > 0) ? price : 1000000;

    if (!sp.add(bookingData))
    {
        db.rollback();
        QMessageBox::critical(this, "Database Error", "Failed to create Booking record!");
        return;
    }

    // update trạng thái phòng
    QSqlQuery updateRoom(db);
    updateRoom.prepare("UPDATE ListRooms SET status = 1 WHERE room_id = ?");
    updateRoom.addBindValue(roomId);

    if (!updateRoom.exec())
    {
        db.rollback();
        QMessageBox::critical(this, "Database Error", "Failed to update Room status:\n" + updateRoom.lastError().text());
        return;
    }

    // xác nhận booking
    if (db.commit())
    {
        QString infoText = isExistingCustomer
                               ? QString("Welcome back %1!\nYour booking is complete.")
                                     .arg(customerName)
                                     .arg(currentPoints)
                               : QString("Thank you %1!\nBooking created successfully.\n")
                                     .arg(customerName);

        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Success");
        msgBox.setText(infoText);
        msgBox.setStyleSheet(
            "QMessageBox { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); } "
            "QLabel { color: #1e293b; font-size: 14px; background: transparent; } "
            "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366f1, stop:1 #8b5cf6); color: white; padding: 6px 20px; border-radius: 4px; border: none; font-weight: bold; }");
        msgBox.exec();

        this->close();
    }
    else
    {
        db.rollback();
        QMessageBox::critical(this, "Database Error",
                              "Transaction commit failed:\n" + db.lastError().text());
    }
}