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
    setStyleSheet("QWidget { background-color: white; color: #333333; }");

    QVBoxLayout *layout = new QVBoxLayout(this);
    QFormLayout *form = new QFormLayout();
    form->setSpacing(15);

    QString inputStyle = "QLineEdit, QDateEdit, QSpinBox { border: 1px solid #cbd5e1; border-radius: 4px; padding: 6px; }";

    txtName = new QLineEdit(this);
    txtName->setStyleSheet(inputStyle);

    txtPhone = new QLineEdit(this);
    txtPhone->setStyleSheet(inputStyle);

    dateCheckIn = new QDateEdit(QDate::currentDate(), this);
    dateCheckIn->setCalendarPopup(true);
    dateCheckIn->setStyleSheet(inputStyle);

    spinPeople = new QSpinBox(this);
    spinPeople->setMinimum(1);
    spinPeople->setMaximum(10);
    spinPeople->setStyleSheet(inputStyle);

    ID = new QLineEdit(this);
    ID->setStyleSheet(inputStyle);

    form->addRow("Your name", txtName);
    form->addRow("Your ID", ID);
    form->addRow("Your phone number", txtPhone);
    form->addRow("Check-in day", dateCheckIn);
    form->addRow("Number people", spinPeople);

    layout->addLayout(form);

    btnNext = new QPushButton("Find Available Rooms", this);
    btnNext->setStyleSheet("background-color: #3b82f6; color: white; padding: 10px; font-weight: bold; border-radius: 6px;");
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
    int people = spinPeople->value();

    // Mở Window tìm phòng và truyền dữ liệu sang
    CustomerWindow *roomWindow = new CustomerWindow(name, phone, id, checkInDate, people);
    roomWindow->show();

    // Đóng Window
    this->close();
}

CustomerWindow::CustomerWindow(QString name, QString phone, QString id, QString date, int people, QWidget *parent)
    : QWidget(parent), customerName(name), ID(id), customerPhone(phone), checkInDate(date), numPeople(people)
{
    setFixedSize(800, 600);
    setWindowTitle("Select a Room");
    setStyleSheet("QWidget { background-color: #f8fafc; color: #333333; }");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);

    tableRoom = new QTableWidget(0, 5, this);
    tableRoom->setHorizontalHeaderLabels({"Room ID", "Room Number", "Type Room", "Price", "Capacity"});
    tableRoom->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableRoom->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableRoom->setSelectionMode(QAbstractItemView::SingleSelection);
    tableRoom->setStyleSheet("QTableWidget { background-color: white; color: #333333; }");
    layout->addWidget(tableRoom);

    btnBook = new QPushButton("Confirm Booking", this);
    btnBook->setStyleSheet("background-color: #10b981; color: white; padding: 12px; font-weight: bold; border-radius: 6px;");
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
                  "FROM ListRooms WHERE status = 0 AND number_people >= ?");
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

    CustomerRepository t;
    // Them customer
    t.add(a);

    BookingData bookingData;
    BookingRepository sp;

    bookingData.customerId = newCustomerId;
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
    }

    this->close();
}