// Đây là file để customer nhập thông tin và nhập phòng
// FIle này sẽ liên quan đến customer,room,booking . respotory
#include "customerwin.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
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

    // Premium background and global font
    setStyleSheet("QWidget { background-color: #f8fafc; color: #1e293b; font-family: 'Segoe UI', Arial, sans-serif; }");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(50, 40, 50, 40); // Added breathing room around edges

    // Added an elegant title
    QLabel *titleLabel = new QLabel("Guest Registration", this);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #1e293b; margin-bottom: 20px;");
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *form = new QFormLayout();
    form->setSpacing(20); // Increased spacing for a cleaner look

    // Modernized input styling with focus effects
    QString inputStyle = "QLineEdit, QDateEdit, QSpinBox { border: 1px solid #cbd5e1; border-radius: 8px; padding: 12px; background-color: white; font-size: 15px; color: #1e293b; }"
                         "QLineEdit:focus, QDateEdit:focus, QSpinBox:focus { border: 2px solid #3b82f6; }";

    // Style for the form labels
    QString labelStyle = "font-size: 15px; font-weight: 600; color: #475569;";

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

    // Adding styled labels to the form
    QLabel *lblName = new QLabel("Your name:", this);
    lblName->setStyleSheet(labelStyle);
    QLabel *lblID = new QLabel("Your ID:", this);
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
    btnNext->setStyleSheet(
        "QPushButton { background-color: #3b82f6; color: white; padding: 14px; font-size: 16px; font-weight: bold; border-radius: 8px; border: none; }"
        "QPushButton:hover { background-color: #2563eb; }");
    layout->addWidget(btnNext);

    connect(btnNext, &QPushButton::clicked, this, &CustomerInputWindow::onNextClicked);
}

// Hàm kiểm tra thông tin khách hàng
void CustomerInputWindow::onNextClicked()
{
    if (txtName->text().isEmpty() || txtPhone->text().isEmpty())
    {
        QMessageBox::warning(this, "Warning", "Please input name and phone");
        return;
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

CustomerWindow::CustomerWindow(QString name, QString phone, QString id, QString date, QString dateout, int people, QWidget *parent)
    : QWidget(parent), customerName(name), ID(id), customerPhone(phone), checkInDate(date), datecheckout(dateout), numPeople(people)
{
    setFixedSize(850, 650); // Made slightly larger to accommodate the beautiful table
    setWindowTitle("Select a Room");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    // Added an elegant title
    QLabel *titleLabel = new QLabel("Available Rooms", this);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #1e293b; margin-bottom: 10px; font-family: 'Segoe UI', Arial, sans-serif;");
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    tableRoom = new QTableWidget(0, 5, this);
    tableRoom->setHorizontalHeaderLabels({"Room ID", "Room Number", "Type Room", "Price", "Capacity"});
    tableRoom->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableRoom->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableRoom->setSelectionMode(QAbstractItemView::SingleSelection);
    tableRoom->verticalHeader()->setDefaultSectionSize(45); // Taller rows for readability
    tableRoom->setAlternatingRowColors(true);

    // Applied the exact premium table stylesheet from the main window
    this->setStyleSheet(R"(
        QWidget { background-color: #f8fafc; color: #1e293b; }
        
        QTableWidget {
            background-color: #ffffff;
            alternate-background-color: #f8fafc;
            border: 1px solid #cbd5e1;
            border-radius: 8px;
            gridline-color: #e2e8f0; 
            font-size: 14px;
            color: #1e293b;
            selection-background-color: #3b82f6;
            selection-color: #ffffff;
            outline: none;
        }
        
        QTableWidget::item {
            padding: 5px;
        }

        QTableWidget::item:hover {
            background-color: #f1f5f9;
            color: #0f172a;
        }
        
        QHeaderView::section:horizontal {
            background-color: #1e293b;
            color: #ffffff;
            font-weight: bold;
            font-size: 14px;
            padding: 12px;
            border: none;
            border-right: 1px solid #334155;
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
            border-right: 1px solid #cbd5e1;
        }
    )");

    layout->addWidget(tableRoom);

    btnBook = new QPushButton("Confirm Booking", this);
    btnBook->setCursor(Qt::PointingHandCursor);
    btnBook->setStyleSheet(
        "QPushButton { background-color: #10b981; color: white; padding: 14px; font-size: 16px; font-weight: bold; border-radius: 8px; border: none; }"
        "QPushButton:hover { background-color: #059669; }"); // Darker green on hover
    layout->addWidget(btnBook);

    connect(btnBook, &QPushButton::clicked, this, &CustomerWindow::onBookRoomClicked);

    // load dữ liệu
    loadFilteredRooms();
}

void CustomerWindow::loadFilteredRooms()
{
    if (!DatabaseManager::instance().open())
    {
        qDebug() << "Error: Could not open database\n";
        return;
    }

    tableRoom->setRowCount(0);
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare("SELECT room_id, room_number, room_type, base_price, number_people "
                  "FROM ListRooms "
                  "WHERE status = 0 "
                  "  AND number_people >= ? "
                  "  AND room_number NOT IN ("
                  "      SELECT room_number FROM Bookings "
                  "      WHERE check_in_date <= ? AND check_out_date >= ?"
                  "  )");

    query.addBindValue(this->numPeople);

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

    // 1. Lưu dữ liệu vào bảng
    QString roomId = tableRoom->item(selectedRow, 0)->text();
    QString roomTypeStr = tableRoom->item(selectedRow, 2)->text();
    double price = tableRoom->item(selectedRow, 3)->text().toDouble();

    QSqlDatabase db = DatabaseManager::instance().database();

    QSqlQuery query(db);

    Customer a(customerName.toStdString(), ID.toStdString(), customerPhone.toStdString());

    a.setIdroom(roomId.toStdString());

    a.setIdroom(roomId.toStdString());

    CustomerRepository t;
    // Them customer
    t.add(a);

    BookingData bookingData;
    BookingRepository sp;

    bookingData.customerId = a.getId();
    bookingData.roomNumber = roomId;
    bookingData.checkInTime = checkInDate;
    bookingData.checkOutTime = checkInDate;
    bookingData.totalPrice = 1000000;

    // Them booking vao
    sp.add(bookingData);

    // Set status của list room là 1
    query.prepare("UPDATE ListRooms SET status = 1 WHERE room_id = ?");
    query.addBindValue(roomId);

    if (!query.exec())
    {
        qDebug() << "Lỗi Insert Booking:" << query.lastError().text();
    }
    else
    {
        qDebug() << "Booking successfully saved to database!";

        // Added a beautiful success message for the user before closing
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Success");
        msgBox.setText("Your room has been booked successfully!");
        msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #1e293b; font-size: 14px; } QPushButton { background-color: #3b82f6; color: white; padding: 6px 20px; border-radius: 4px; border: none; font-weight: bold; }");
        msgBox.exec();
    }

    this->close();
}