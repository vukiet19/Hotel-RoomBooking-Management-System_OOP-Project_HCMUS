// Hàm này liên quan đến control lên UI( như button , chắc năng khi ấn nút,...)
// Hàm này liên quan đến UI( nơi trình chiếu), booking.h( khi chọn add lên frontend),DerivedRooms(Nơi chọn Type room)
// Checkoutpage(Page chọn checkout), Các hàm của Manager để link với database

#include "UX.h"
#include "../UI.h"
#include "frontend/Observers/QtHotelObserver.h"
#include "frontend/usercheck/backend.h"
#include "backend/Repository/CustomerRepository.h"
#include "backend/Repository/RoomRepository.h"
#include "frontend/UI/ManagerWindow/Checkout/CheckoutPage.h"
#include "backend/Manager/DatabaseManager.h"
#include "backend/Manager/DashboardService.h"
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QComboBox>
#include <QDateEdit>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QLineEdit>
#include <QUuid>
#include "backend/Repository/FoodRepository.h"

#include <QSqlQuery>
#include <QSqlError>
#include "cores/Booking/Booking.h"
#include "cores/Room/DerivedRooms.h"

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>

MainWindowController::MainWindowController(QWidget *parent) : MainWindowUi(parent)
{

    // Gọi UI
    MainWindowUi::setupUi();
    initConnections();
}

// Hàm link kết nối
void MainWindowController::initConnections()
{
    connect(buttonBooking, &QPushButton::clicked, this, &MainWindowController::handleLogin_1);
    connect(buttonCustomer, &QPushButton::clicked, this, &MainWindowController::handleLogin_3);
    connect(buttonRoom, &QPushButton::clicked, this, &MainWindowController::handleLogin_7);
    connect(buttonService, &QPushButton::clicked, this, &MainWindowController::handleLogin_4);
    connect(buttonInventory, &QPushButton::clicked, this, &MainWindowController::handleLogin_5);
    connect(buttonBill, &QPushButton::clicked, this, &MainWindowController::handleLogin_10);
    connect(buttonCheckout, &QPushButton::clicked, this, &MainWindowController::handleCheckout);
    connect(buttonDashboard, &QPushButton::clicked, this, &MainWindowController::handleDashboardTab);

    connect(bookingPage->bookingTabButton(), &QPushButton::clicked, this, &MainWindowController::handleLogin_1);
    connect(bookingPage->servicesTabButton(), &QPushButton::clicked, this, &MainWindowController::handleLogin_2);
    connect(roomPage->roomTabButton(), &QPushButton::clicked, this, &MainWindowController::handleLogin_7);
    connect(roomPage->roomTypeTabButton(), &QPushButton::clicked, this, &MainWindowController::handleLogin_8);
    connect(servicePage->foodTabButton(), &QPushButton::clicked, this, &MainWindowController::handleLogin_4);
    connect(servicePage->serviceTabButton(), &QPushButton::clicked, this, &MainWindowController::handleLogin_9);
    connect(inventoryPage->inventoryTabButton(), &QPushButton::clicked, this, &MainWindowController::handleLogin_5);
    connect(inventoryPage->inventoryLogTabButton(), &QPushButton::clicked, this, &MainWindowController::handleLogin_6);

    connect(&QtHotelObserver::instance(), &QtHotelObserver::roomStatusChanged,
            this, [this](const QString &, int, const QString &)
            {
                if (tableRoom)
                    Backend::loadTableData(tableRoom, "SELECT * FROM ListRooms");
            });

    connect(&QtHotelObserver::instance(), &QtHotelObserver::bookingStatusChanged,
            this, [this](int, const QString &, const QString &, int, double, const QString &)
            {
                if (tableBooking)
                    Backend::loadTableData(tableBooking, "SELECT * FROM Bookings");
            });
}

// Button booking
void MainWindowController::handleLogin_1()
{
    setActionBarVisible(true);
    stackedWidget->setCurrentIndex(BookingIndex);
    bookingPage->setSection(0);
    setActiveButton(buttonBooking);
    Backend::loadTableData(tableBooking, "SELECT * FROM Bookings");
    // disconnect là ngắt kết nối chức năng hàm đó, để cho các hàm này không lập lại
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
    connect(btnAdd, &QPushButton::clicked, this, &MainWindowController::showAddBookingDialog);
}

// Button BookingServiceItems

void MainWindowController::handleLogin_2()
{
    stackedWidget->setCurrentIndex(BookingIndex);
    bookingPage->setSection(1);
    setActionBarVisible(true);
    setActiveButton(buttonBooking);
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
    Backend::loadTableData(tableBookingItems, "SELECT * FROM BookingServiceItems");
}

// Button Customer

void MainWindowController::handleLogin_3()
{
    stackedWidget->setCurrentIndex(CustomerIndex);
    setActionBarVisible(true);
    setActiveButton(buttonCustomer);
    Backend::loadTableData(tableCustomer, "SELECT * FROM Customer");
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
    connect(btnAdd, &QPushButton::clicked, this, &MainWindowController::AddNewCustomerClicked);
    connect(btnUpdate, &QPushButton::clicked, this, &MainWindowController::showUpdateCustomerDialog);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindowController::showDeleteCustomerDialog);
    connect(btnFilter, &QPushButton::clicked, this, &MainWindowController::showFilterCustomerDialog);
}

// Button Food

void MainWindowController::handleLogin_4()
{
    stackedWidget->setCurrentIndex(ServiceIndex);
    servicePage->setSection(0);
    setActionBarVisible(true);
    setActiveButton(buttonService);

    Backend::loadTableData(tableFood, "SELECT * FROM FoodOptions");

    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();

    connect(btnAdd, &QPushButton::clicked, this, &MainWindowController::AddNewFoodClick);
    connect(btnUpdate, &QPushButton::clicked, this, &MainWindowController::UpdateFoodClick);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindowController::DeleteFoodClick);
    connect(btnFilter, &QPushButton::clicked, this, &MainWindowController::FilterFoodClick);
}

// Button Inventory

void MainWindowController::handleLogin_5()
{
    stackedWidget->setCurrentIndex(InventoryIndex);
    inventoryPage->setSection(0);
    setActionBarVisible(true);
    setActiveButton(buttonInventory);
    Backend::loadTableData(tableInventory, "SELECT * FROM Inventory");
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
}

// Button InventoryLog

void MainWindowController::handleLogin_6()
{
    stackedWidget->setCurrentIndex(InventoryIndex);
    inventoryPage->setSection(1);
    setActionBarVisible(true);
    setActiveButton(buttonInventory);
    Backend::loadTableData(tableInventoryLog, "SELECT * FROM InventoryLog");
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
}

// Button ListRooms

void MainWindowController::handleLogin_7()
{
    stackedWidget->setCurrentIndex(RoomIndex);
    roomPage->setSection(0);
    setActionBarVisible(true);
    setActiveButton(buttonRoom);
    Backend::loadTableData(tableRoom, "SELECT * FROM ListRooms");

    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();

    connect(btnAdd, &QPushButton::clicked, this, &MainWindowController::showAddRoomDialog);
    connect(btnUpdate, &QPushButton::clicked, this, &MainWindowController::showUpdateRoomDialog);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindowController::showDeleteRoomDialog);
}

// Button RoomTypeCatalog

void MainWindowController::handleLogin_8()
{
    stackedWidget->setCurrentIndex(RoomIndex);
    roomPage->setSection(1);
    setActionBarVisible(true);
    setActiveButton(buttonRoom);
    Backend::loadTableData(tableRoomType, "SELECT * FROM RoomTypeCatalog");
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
}

// Button ListServiceItems

void MainWindowController::handleLogin_9()
{
    stackedWidget->setCurrentIndex(ServiceIndex);
    servicePage->setSection(1);
    setActionBarVisible(true);
    setActiveButton(buttonService);
    Backend::loadTableData(tableService, "SELECT * FROM ListServiceItems");
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
}

// Button Bills

void MainWindowController::handleLogin_10()
{
    stackedWidget->setCurrentIndex(BillIndex);
    setActionBarVisible(true);
    setActiveButton(buttonBill);
    Backend::loadTableData(tableBill, "SELECT * FROM Bills");
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
}

// Hàm add new customer
void MainWindowController::AddNewCustomerClicked()
{
    QDialog *addDialog = new QDialog(this);
    addDialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
        "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");
    addDialog->setWindowTitle("Add Customer");
    addDialog->setFixedSize(400, 420);

    QVBoxLayout *mainLayout = new QVBoxLayout(addDialog);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("Customer's Information", addDialog);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #3730a3; margin-bottom: 20px;");
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    QString inputStyle =
        "QLineEdit {"
        "   background-color: #ffffff; "
        "   border: 2px solid #38bdf8; "
        "   border-radius: 8px; "
        "   padding: 10px; "
        "   font-size: 14px; "
        "   color: #0f172a; "
        "}"
        "QLineEdit:hover { border: 2px solid #0284c7; }"
        "QLineEdit:focus { border: 2px solid #0369a1; background-color: #f0f9ff; }";

    // Điền thông tin customer
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

    btnSave->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366f1, stop:1 #8b5cf6); color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4f46e5, stop:1 #7c3aed); }");
    btnCancel->setStyleSheet("background-color: #cbd5e1; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

    btnSave->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);
    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, addDialog, &QDialog::reject);

    // Hàm sử lý chức năng khi click
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
    

        //Lưu thông tin khách hàng vào database
        CustomerRepository re;
        Customer a(name.toStdString(), phone.toStdString(), IDcard.toStdString());
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

// Hàm này sẽ thể hiện nút mà đang hiện
void MainWindowController::setActiveButton(QPushButton *clickedButton)
{
    QList<QPushButton *> buttons = {
        buttonDashboard, buttonBooking, buttonCustomer, buttonRoom,
        buttonService, buttonInventory, buttonBill, buttonCheckout};

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

// Add booking (như add customer )
void MainWindowController::showAddBookingDialog()
{
    QDialog *addDialog = new QDialog(this);
    addDialog->setWindowTitle("Add Booking");
    addDialog->setFixedSize(450, 560);

    addDialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
        "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(addDialog);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("Booking Information", addDialog);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #3730a3; margin-bottom: 20px;");
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    QString inputStyle =
        "QLineEdit, QDateEdit {"
        "   background-color: #ffffff; "
        "   border: 2px solid #38bdf8; "
        "   border-radius: 8px; "
        "   padding: 10px; "
        "   font-size: 14px; "
        "   color: #0f172a; "
        "}"
        "QLineEdit:hover, QDateEdit:hover { border: 2px solid #0284c7; }"
        "QLineEdit:focus, QDateEdit:focus { border: 2px solid #0369a1; background-color: #f0f9ff; }";

    QLineEdit *txtId = new QLineEdit(addDialog);
    txtId->setPlaceholderText("Customer ID ...");
    txtId->setStyleSheet(inputStyle);

    QLineEdit *txtCustomer = new QLineEdit(addDialog);
    txtCustomer->setPlaceholderText("Customer Name...");
    txtCustomer->setStyleSheet(inputStyle);

    QLineEdit *txtPhone = new QLineEdit(addDialog);
    txtPhone->setPlaceholderText("Phone Number...");
    txtPhone->setStyleSheet(inputStyle);

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

    btnSave->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366f1, stop:1 #8b5cf6); color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4f46e5, stop:1 #7c3aed); }");
    btnCancel->setStyleSheet("background-color: #cbd5e1; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

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


         if (id.toStdString().size() != 10)
    {
        QMessageBox::warning(this, "Input Error", "Error: ID Card must be 10 digits long.");
        return;
    }

    for (char g : id.toStdString())
    {
        if (!std::isdigit(g))
        {
            QMessageBox::warning(this, "Input Error", "Error:ID Card must contain only numbers.");
            return;
        }
    }

     if (phone.toStdString().size() != 10)
    {
        QMessageBox::warning(this, "Input Error", "Error: Phone Number must be 10 digits long.");
        return;
    }

    for (char g : phone.toStdString())
    {
        if (!std::isdigit(g))
        {
            QMessageBox::warning(this, "Input Error", "Error: Phone number must contain only numbers.");
            return;
        }
    }
        
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

        BookingRepository r;
        BookingData t;
        t.customerId = id.toInt();
        t.roomNumber = room; 
        t.checkInTime = checkInDate;
        t.checkOutTime = checkOutDate;
        r.add(t);

        QMessageBox::information(addDialog, "Success", "Booking created and room status updated successfully!");
        addDialog->accept(); });

    addDialog->exec();
    addDialog->deleteLater();
}

// Add room (như add customer )

void MainWindowController::showAddRoomDialog()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Add room");
    dialog->setFixedSize(420, 500);

    dialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
        "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }"
        "QMessageBox { background-color: white; }"
        "QMessageBox QLabel { color: #333333; font-size: 13px; font-weight: normal; }"
        "QMessageBox QPushButton { background-color: #cbd5e1; color: #1e293b; border: none; border-radius: 4px; padding: 6px 20px; font-weight: bold; }");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("Room Details", dialog);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #3730a3; margin-bottom: 15px;");
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *form = new QFormLayout();
    form->setSpacing(15);

    QString inputStyle =
        "QLineEdit, QComboBox {"
        "   background-color: #ffffff; "
        "   border: 2px solid #38bdf8; "
        "   border-radius: 8px; "
        "   padding: 10px; "
        "   font-size: 14px; "
        "   color: #0f172a; "
        "}"
        "QLineEdit:hover, QComboBox:hover { border: 2px solid #0284c7; }"
        "QLineEdit:focus, QComboBox:focus { border: 2px solid #0369a1; background-color: #f0f9ff; }";

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

    btnSave->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366f1, stop:1 #8b5cf6); color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4f46e5, stop:1 #7c3aed); }");
    btnCancel->setStyleSheet("background-color: #cbd5e1; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

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

// update customer

void MainWindowController::showUpdateCustomerDialog()
{
    handleLogin_3();
}

// Delete theo customer id
void MainWindowController::showDeleteCustomerDialog()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Delete Customer");
    dialog->setFixedSize(380, 220);

    dialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
        "QLabel { color: #1e293b; font-weight: bold; font-size: 14px;}");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(30, 20, 30, 20);

    QLabel *titleLabel = new QLabel("Remove Customer", dialog);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #e11d48; margin-bottom: 10px;");
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *form = new QFormLayout();

    QString inputStyle =
        "QLineEdit {"
        "   background-color: #ffffff; "
        "   border: 2px solid #38bdf8; "
        "   border-radius: 8px; "
        "   padding: 10px; "
        "   font-size: 14px; "
        "   color: #0f172a; "
        "}"
        "QLineEdit:hover { border: 2px solid #0284c7; }"
        "QLineEdit:focus { border: 2px solid #0369a1; background-color: #f0f9ff; }";

    QLineEdit *txtId = new QLineEdit(dialog);
    txtId->setPlaceholderText("Enter Customer ID...");
    txtId->setStyleSheet(inputStyle);

    form->addRow("Customer ID:", txtId);
    layout->addLayout(form);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnDelete = new QPushButton("Delete", dialog);
    QPushButton *btnCancel = new QPushButton("Cancel", dialog);

    btnDelete->setStyleSheet("background-color: #e11d48; color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 14px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #cbd5e1; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 14px; font-weight: bold;");

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

// FIlter customer theo
void MainWindowController::showFilterCustomerDialog()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Filter Customers");
    dialog->setFixedSize(420, 250);

    dialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
        "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(30, 20, 30, 20);

    QLabel *titleLabel = new QLabel("Advanced Search", dialog);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #3730a3; margin-bottom: 15px;");
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *form = new QFormLayout();

    QString inputStyle =
        "QLineEdit, QComboBox {"
        "   background-color: #ffffff; "
        "   border: 2px solid #38bdf8; "
        "   border-radius: 8px; "
        "   padding: 10px; "
        "   font-size: 14px; "
        "   color: #312e81; "
        "}"
        "QLineEdit:hover, QComboBox:hover { border: 2px solid #6366f1; }"
        "QLineEdit:focus, QComboBox:focus { border: 2px solid #8b5cf6; background-color: #f0f9ff; }"
        "QComboBox::drop-down { border: none; width: 25px; }"
        "QComboBox::down-arrow { image: none; }"
        "QComboBox QAbstractItemView, QComboBox QListView {"
        "   background-color: #ffffff; "
        "   color: #312e81; "
        "   border: 2px solid #a5b4fc; "
        "   border-radius: 4px; "
        "   selection-background-color: #e0e7ff; "
        "   selection-color: #312e81; "
        "   outline: none;"
        "}";

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

    btnSearch->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366f1, stop:1 #8b5cf6); color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4f46e5, stop:1 #7c3aed); }");
    btnCancel->setStyleSheet("background-color: #cbd5e1; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 14px; font-weight: bold;");

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
                tableCustomer->setItem(row, 4, new QTableWidgetItem(cus.getTier()));
                tableCustomer->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(cus.getIdRoom())));
                row++;
            }

            QMessageBox::information(dialog, "Success", QString("Found %1 customers!").arg(filteredCustomers.size()));
            dialog->accept();
        } });

    dialog->exec();
    dialog->deleteLater();
}

// Delete room , theo room number
void MainWindowController::showDeleteRoomDialog()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Delete Room");
    dialog->setFixedSize(380, 220);

    dialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
        "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(30, 20, 30, 20);

    QLabel *titleLabel = new QLabel("Remove Room", dialog);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #e11d48; margin-bottom: 10px;");
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *form = new QFormLayout();

    QString inputStyle =
        "QLineEdit {"
        "   background-color: #ffffff; "
        "   border: 2px solid #38bdf8; "
        "   border-radius: 8px; "
        "   padding: 10px; "
        "   font-size: 14px; "
        "   color: #0f172a; "
        "}"
        "QLineEdit:hover { border: 2px solid #0284c7; }"
        "QLineEdit:focus { border: 2px solid #0369a1; background-color: #f0f9ff; }";

    QLineEdit *txtId = new QLineEdit(dialog);
    txtId->setPlaceholderText("Enter Room Number...");
    txtId->setStyleSheet(inputStyle);

    form->addRow("Room Number:", txtId);
    layout->addLayout(form);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnDelete = new QPushButton("Delete", dialog);
    QPushButton *btnCancel = new QPushButton("Cancel", dialog);

    btnDelete->setStyleSheet("background-color: #e11d48; color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 14px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #cbd5e1; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 14px; font-weight: bold;");

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

// Update status room
void MainWindowController::showUpdateRoomDialog()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Update Room");
    dialog->setFixedSize(420, 420);

    dialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
        "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }"
        "QMessageBox { background-color: white; }"
        "QMessageBox QLabel { color: #333333; font-size: 13px; font-weight: normal; }"
        "QMessageBox QPushButton { background-color: #cbd5e1; color: #1e293b; border: none; border-radius: 4px; padding: 6px 20px; font-weight: bold; }");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(30, 20, 30, 20);

    QLabel *titleLabel = new QLabel("Update Room Details", dialog);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #3730a3; margin-bottom: 15px;");
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *form = new QFormLayout();
    form->setSpacing(15);

    QString inputStyle =
        "QLineEdit, QComboBox {"
        "   background-color: #ffffff; "
        "   border: 2px solid #38bdf8; "
        "   border-radius: 8px; "
        "   padding: 10px; "
        "   font-size: 14px; "
        "   color: #0f172a; "
        "}"
        "QLineEdit:hover, QComboBox:hover { border: 2px solid #0284c7; }"
        "QLineEdit:focus, QComboBox:focus { border: 2px solid #0369a1; background-color: #f0f9ff; }";

    QLineEdit *txtNumber = new QLineEdit(dialog);
    txtNumber->setPlaceholderText("Target Room Number...");
    txtNumber->setStyleSheet(inputStyle);

    QComboBox *cbStatus = new QComboBox(dialog);
    cbStatus->addItems({"Available", "Reserved", "Occupied", "Maintenance"});
    cbStatus->setStyleSheet(inputStyle);

    form->addRow("Room Number:", txtNumber);
    form->addRow("New Status:", cbStatus);

    layout->addLayout(form);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnSave = new QPushButton("Update", dialog);
    QPushButton *btnCancel = new QPushButton("Cancel", dialog);

    btnSave->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366f1, stop:1 #8b5cf6); color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4f46e5, stop:1 #7c3aed); }");
    btnCancel->setStyleSheet("background-color: #cbd5e1; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 14px; font-weight: bold;");

    btnSave->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);
    layout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);

    connect(btnSave, &QPushButton::clicked, dialog, [=]()
            {

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

// Hàm checkout
void MainWindowController::handleCheckout()
{
    setActionBarVisible(false);
    stackedWidget->setCurrentWidget(checkoutPage);
    setActiveButton(buttonCheckout);
}

void MainWindowController::setActionBarVisible(bool visible)
{
    if (actionBar)
    {
        actionBar->setVisible(visible);
    }
}

// Dashboard
void MainWindowController::handleDashboardTab()
{
    qDebug() << "[DEBUG] handleDashboardTab - Switched to Dashboard tab";
    stackedWidget->setCurrentIndex(DashboardIndex);
    setActiveButton(buttonDashboard);
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();

    // link btnFilter vs hàm
    connect(btnFilter, &QPushButton::clicked, this, &MainWindowController::showFilterDashboardDialog);

    DashboardService ds;

    // lấy thông số
    int todayBookings = ds.getTodayBookings();
    double dailyRevenue = ds.getRevenue("day");
    double monthlyRevenue = ds.getRevenue("month");
    double yearlyRevenue = ds.getRevenue("year");

    qDebug() << "[DEBUG] Today's Bookings:" << todayBookings
             << "| Daily:" << dailyRevenue
             << "| Monthly:" << monthlyRevenue
             << "| Yearly:" << yearlyRevenue;

    // ép vào UI
    lblTodayBookings->setText(QString::number(todayBookings));
    lblDailyRevenue->setText(QString::number(dailyRevenue, 'f', 2) + " VND");
    lblMonthlyRevenue->setText(QString::number(monthlyRevenue, 'f', 2) + " VND");
    lblYearlyRevenue->setText(QString::number(yearlyRevenue, 'f', 2) + " VND");

    // lấy booking
    std::vector<BookingRevenue> data = ds.getBookingRevenues("2026-01-01", "2026-12-31");
    qDebug() << "[DEBUG] Revenues table row count:" << data.size();

    tableDashboard->setRowCount(0);
    int row = 0;
    for (const auto &record : data)
    {
        tableDashboard->insertRow(row);
        tableDashboard->setItem(row, 0, new QTableWidgetItem(QString::number(record.bookingId)));
        tableDashboard->setItem(row, 1, new QTableWidgetItem(record.customerName));
        tableDashboard->setItem(row, 2, new QTableWidgetItem(QString::number(record.revenue, 'f', 2)));
        tableDashboard->setItem(row, 3, new QTableWidgetItem(record.checkIn));
        row++;
    }
}

void MainWindowController::showFilterDashboardDialog()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Filter Dashboard Revenues");
    dialog->setFixedSize(400, 220);

    dialog->setStyleSheet(
        "QDialog { background-color: white; }"
        "QLabel { color: #333333; font-weight: bold; }");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    QFormLayout *form = new QFormLayout();

    QString inputStyle = "QLineEdit { border: 1px solid #cbd5e1; border-radius: 4px; padding: 6px; }";

    QLineEdit *txtStartDate = new QLineEdit(dialog);
    txtStartDate->setPlaceholderText("YYYY-MM-DD");
    txtStartDate->setText("2026-01-01");
    txtStartDate->setStyleSheet(inputStyle);

    QLineEdit *txtEndDate = new QLineEdit(dialog);
    txtEndDate->setPlaceholderText("YYYY-MM-DD");
    txtEndDate->setText("2026-12-31");
    txtEndDate->setStyleSheet(inputStyle);

    form->addRow("Start Date:", txtStartDate);
    form->addRow("End Date:", txtEndDate);
    layout->addLayout(form);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *btnApply = new QPushButton("Apply", dialog);
    QPushButton *btnCancel = new QPushButton("Cancel", dialog);

    btnApply->setStyleSheet("background-color: #3b82f6; color: white; border-radius: 6px; padding: 10px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #94a3b8; color: white; border-radius: 6px; padding: 10px; font-weight: bold;");

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnApply);
    layout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);

    connect(btnApply, &QPushButton::clicked, [=]()
            {
        qDebug() << "[DEBUG] Filter Dialog - Apply clicked";
        QString startStr = txtStartDate->text();
        QString endStr = txtEndDate->text();

        if (startStr.isEmpty() || endStr.isEmpty())
        {
            QMessageBox::warning(this, "Error", "Please enter both Start Date and End Date!");
            return;
        }

        if (!DatabaseManager::instance().open())
        {
            QMessageBox::warning(this, "Error", "Could not open database connection!");
            return;
        }

        DashboardService ds;
        qDebug() << "[DEBUG] Filter Dialog - Querying from" << startStr << "to" << endStr;
        std::vector<BookingRevenue> data = ds.getBookingRevenues(startStr, endStr);
        qDebug() << "[DEBUG] Filter Dialog - Query returned" << data.size() << "records";

        tableDashboard->setRowCount(0);
        int row = 0;
        for (const auto& record : data) {
            tableDashboard->insertRow(row);
            tableDashboard->setItem(row, 0, new QTableWidgetItem(QString::number(record.bookingId)));
            tableDashboard->setItem(row, 1, new QTableWidgetItem(record.customerName));
            tableDashboard->setItem(row, 2, new QTableWidgetItem(QString::number(record.revenue, 'f', 2)));
            tableDashboard->setItem(row, 3, new QTableWidgetItem(record.checkIn));
            row++;
        }
        qDebug() << "[DEBUG] Filter Dialog - Table populated with" << row << "rows";

        QMessageBox::information(this, "Success", QString("Filtered and found %1 booking records!").arg(data.size()));
        qDebug() << "[DEBUG] Filter Dialog - Accepting dialog";
        dialog->accept(); });

    dialog->exec();
    dialog->deleteLater();
}

// Hàm add food

void MainWindowController::AddNewFoodClick()
{
    QDialog *addDialog = new QDialog(this);
    addDialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
        "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");
    addDialog->setWindowTitle("Add Food");
    addDialog->setFixedSize(400, 480);

    QVBoxLayout *mainLayout = new QVBoxLayout(addDialog);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("Food's Information", addDialog);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #3730a3; margin-bottom: 20px;");
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    QString inputStyle =
        "QLineEdit {"
        "   background-color: #ffffff; "
        "   border: 2px solid #38bdf8; "
        "   border-radius: 8px; "
        "   padding: 10px; "
        "   font-size: 14px; "
        "   color: #0f172a; "
        "}"
        "QLineEdit:hover { border: 2px solid #0284c7; }"
        "QLineEdit:focus { border: 2px solid #0369a1; background-color: #f0f9ff; }";

    // 1. Nhập hoặc chọn Food ID (Tự động sinh mã F_xxx hoặc cho phép tự nhập)
    QLineEdit *txtFoodId = new QLineEdit(addDialog);
    txtFoodId->setPlaceholderText("e.g. F001");
    // Tạo ID ngẫu nhiên mặc định
    txtFoodId->setText("F_" + QString::number(QDateTime::currentMSecsSinceEpoch() % 10000));
    txtFoodId->setStyleSheet(inputStyle);

    // 2. Nhập thông tin Food Name
    QLineEdit *txtFoodName = new QLineEdit(addDialog);
    txtFoodName->setPlaceholderText("Type name of food..");
    txtFoodName->setStyleSheet(inputStyle);

    // 3. Nhập Category
    QLineEdit *txtCato = new QLineEdit(addDialog);
    txtCato->setPlaceholderText("Type Category...");
    txtCato->setStyleSheet(inputStyle);

    // 4. Nhập Price
    QLineEdit *txtPrice = new QLineEdit(addDialog);
    txtPrice->setPlaceholderText("Type price...");
    txtPrice->setStyleSheet(inputStyle);

    formLayout->addRow(new QLabel("Food ID:", addDialog), txtFoodId);
    formLayout->addRow(new QLabel("Food Name:", addDialog), txtFoodName);
    formLayout->addRow(new QLabel("Category:", addDialog), txtCato);
    formLayout->addRow(new QLabel("Base Price:", addDialog), txtPrice);

    mainLayout->addLayout(formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnSave = new QPushButton("Save", addDialog);
    QPushButton *btnCancel = new QPushButton("Cancel", addDialog);

    btnSave->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366f1, stop:1 #8b5cf6); color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4f46e5, stop:1 #7c3aed); }");
    btnCancel->setStyleSheet("background-color: #cbd5e1; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

    btnSave->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);
    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, addDialog, &QDialog::reject);

    connect(btnSave, &QPushButton::clicked, [=]()
            {
        QString foodId = txtFoodId->text().trimmed();
        QString name = txtFoodName->text().trimmed();
        QString cato = txtCato->text().trimmed();
        QString priceStr = txtPrice->text().trimmed();

        if (foodId.isEmpty() || name.isEmpty() || cato.isEmpty() || priceStr.isEmpty())
        {
            QMessageBox::warning(addDialog, "Validation Error", "Please fill in all fields!");
            return;
        }

        bool okPrice = false;
        double price = priceStr.toDouble(&okPrice);
        if (!okPrice || price < 0)
        {
            QMessageBox::warning(addDialog, "Validation Error", "Please enter a valid price!");
            return;
        }

        FoodRepository repo(foodId, name, cato, price);

        if (repo.add()) {
            QMessageBox::information(addDialog, "Success", "Successfully added new Food!");
            addDialog->accept();
            
            handleLogin_4(); 
        } else {
            QMessageBox::critical(addDialog, "Database Error", "Failed to save food into database!");
        } });

    addDialog->exec();
    addDialog->deleteLater();
}

void MainWindowController::UpdateFoodClick()
{
    QDialog *updateDialog = new QDialog(this);
    updateDialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
        "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");
    updateDialog->setWindowTitle("Update Food Details");
    updateDialog->setFixedSize(400, 420);

    QVBoxLayout *mainLayout = new QVBoxLayout(updateDialog);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("Update Food Info", updateDialog);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #3730a3; margin-bottom: 20px;");
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    QString inputStyle =
        "QLineEdit {"
        "   background-color: #ffffff; "
        "   border: 2px solid #38bdf8; "
        "   border-radius: 8px; "
        "   padding: 10px; "
        "   font-size: 14px; "
        "   color: #0f172a; "
        "}"
        "QLineEdit:hover { border: 2px solid #0284c7; }"
        "QLineEdit:focus { border: 2px solid #0369a1; background-color: #f0f9ff; }";

    // 1. Booking ID
    QLineEdit *txtBookingId = new QLineEdit(updateDialog);
    txtBookingId->setPlaceholderText("Booking ID (e.g. BK-1001)...");
    txtBookingId->setStyleSheet(inputStyle);

    // 2. Food ID
    QLineEdit *txtFoodId = new QLineEdit(updateDialog);
    txtFoodId->setPlaceholderText("Food ID (e.g. F001)...");
    txtFoodId->setStyleSheet(inputStyle);

    // 3. Category
    QLineEdit *txtCategory = new QLineEdit(updateDialog);
    txtCategory->setPlaceholderText("Category / Parent ID...");
    txtCategory->setStyleSheet(inputStyle);

    formLayout->addRow(new QLabel("Booking ID:", updateDialog), txtBookingId);
    formLayout->addRow(new QLabel("Food ID:", updateDialog), txtFoodId);
    formLayout->addRow(new QLabel("Category:", updateDialog), txtCategory);

    mainLayout->addLayout(formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnUpdate = new QPushButton("Update", updateDialog);
    QPushButton *btnCancel = new QPushButton("Cancel", updateDialog);

    btnUpdate->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366f1, stop:1 #8b5cf6); color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4f46e5, stop:1 #7c3aed); }");
    btnCancel->setStyleSheet("background-color: #cbd5e1; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

    btnUpdate->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnUpdate);
    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, updateDialog, &QDialog::reject);

    connect(btnUpdate, &QPushButton::clicked, [=]()
            {
        QString bookingId = txtBookingId->text().trimmed();
        QString foodId = txtFoodId->text().trimmed();
        QString category = txtCategory->text().trimmed();

        if (bookingId.isEmpty() || foodId.isEmpty() || category.isEmpty())
        {
            QMessageBox::warning(updateDialog, "Warning", "Please fill in Booking ID, Food ID, and Category!");
            return;
        }

        FoodRepository repo;
        // Gọi repo.update với 3 tham số: (bookingId, foodId, category)
        if (repo.update(bookingId, foodId, category)) {
            QMessageBox::information(updateDialog, "Success", "Food details updated successfully!");
            updateDialog->accept();
            handleLogin_4(); 
        } else {
            QMessageBox::critical(updateDialog, "Error", "Failed to update food. Please check Booking ID and Food ID.");
        } });

    updateDialog->exec();
    updateDialog->deleteLater();
}

void MainWindowController::DeleteFoodClick()
{
    QDialog *deleteDialog = new QDialog(this);
    deleteDialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
        "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");
    deleteDialog->setWindowTitle("Delete Food");
    deleteDialog->setFixedSize(400, 250);

    QVBoxLayout *mainLayout = new QVBoxLayout(deleteDialog);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("Delete Food", deleteDialog);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #ef4444; margin-bottom: 15px;");
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    QString inputStyle =
        "QLineEdit {"
        "   background-color: #ffffff; "
        "   border: 2px solid #f87171; "
        "   border-radius: 8px; "
        "   padding: 10px; "
        "   font-size: 14px; "
        "   color: #0f172a; "
        "}"
        "QLineEdit:hover { border: 2px solid #dc2626; }"
        "QLineEdit:focus { border: 2px solid #b91c1c; background-color: #fef2f2; }";

    QLineEdit *txtFoodId = new QLineEdit(deleteDialog);
    txtFoodId->setPlaceholderText("Enter Food ID to delete...");
    txtFoodId->setStyleSheet(inputStyle);

    formLayout->addRow(new QLabel("Target Food ID:", deleteDialog), txtFoodId);
    mainLayout->addLayout(formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnDelete = new QPushButton("Delete", deleteDialog);
    QPushButton *btnCancel = new QPushButton("Cancel", deleteDialog);

    btnDelete->setStyleSheet(
        "QPushButton { background: #ef4444; color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold; }"
        "QPushButton:hover { background: #dc2626; }");
    btnCancel->setStyleSheet("background-color: #cbd5e1; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

    btnDelete->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnDelete);
    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, deleteDialog, &QDialog::reject);

    connect(btnDelete, &QPushButton::clicked, [=]()
            {
        QString targetId = txtFoodId->text().trimmed();

        if (targetId.isEmpty())
        {
            QMessageBox::warning(deleteDialog, "Warning", "Please enter Target Food ID!");
            return;
        }

        QMessageBox::StandardButton confirm = QMessageBox::question(
            deleteDialog, "Confirm Delete", 
            QString("Are you sure you want to delete Food ID: %1?").arg(targetId),
            QMessageBox::Yes | QMessageBox::No
        );

        if (confirm == QMessageBox::Yes)
        {
            FoodRepository repo;
            if (repo.delete_f(targetId)) {
                QMessageBox::information(deleteDialog, "Success", "Food deleted successfully!");
                deleteDialog->accept();
                handleLogin_4(); // Call function to refresh food table UI
            } else {
                QMessageBox::critical(deleteDialog, "Error", "Failed to delete food. Please check Food ID.");
            }
        } });

    deleteDialog->exec();
    deleteDialog->deleteLater();
}

void MainWindowController::FilterFoodClick()
{
    QDialog *filterDialog = new QDialog(this);
    filterDialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
        "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");
    filterDialog->setWindowTitle("Filter Food By Price");
    filterDialog->setFixedSize(400, 320);

    QVBoxLayout *mainLayout = new QVBoxLayout(filterDialog);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("Filter By Price", filterDialog);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #3730a3; margin-bottom: 20px;");
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    QString inputStyle =
        "QLineEdit {"
        "   background-color: #ffffff; "
        "   border: 2px solid #38bdf8; "
        "   border-radius: 8px; "
        "   padding: 10px; "
        "   font-size: 14px; "
        "   color: #0f172a; "
        "}"
        "QLineEdit:hover { border: 2px solid #0284c7; }"
        "QLineEdit:focus { border: 2px solid #0369a1; background-color: #f0f9ff; }";

    QLineEdit *txtMinPrice = new QLineEdit(filterDialog);
    txtMinPrice->setPlaceholderText("Min price (e.g. 10000)");
    txtMinPrice->setStyleSheet(inputStyle);

    QLineEdit *txtMaxPrice = new QLineEdit(filterDialog);
    txtMaxPrice->setPlaceholderText("Max price (e.g. 100000)");
    txtMaxPrice->setStyleSheet(inputStyle);

    formLayout->addRow(new QLabel("Min Price:", filterDialog), txtMinPrice);
    formLayout->addRow(new QLabel("Max Price:", filterDialog), txtMaxPrice);

    mainLayout->addLayout(formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnFilter = new QPushButton("Apply Filter", filterDialog);
    QPushButton *btnCancel = new QPushButton("Cancel", filterDialog);

    btnFilter->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366f1, stop:1 #8b5cf6); color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4f46e5, stop:1 #7c3aed); }");
    btnCancel->setStyleSheet("background-color: #cbd5e1; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

    btnFilter->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnFilter);
    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, filterDialog, &QDialog::reject);

    connect(btnFilter, &QPushButton::clicked, [=]()
            {
        bool okMin = false, okMax = false;
        double minPrice = txtMinPrice->text().trimmed().toDouble(&okMin);
        double maxPrice = txtMaxPrice->text().trimmed().toDouble(&okMax);

        if (!okMin || !okMax || minPrice < 0 || maxPrice < minPrice)
        {
            QMessageBox::warning(filterDialog, "Invalid Input", "Please enter a valid Min and Max price range!");
            return;
        }

        FoodRepository repo;
        QVector<Food> filteredList = repo.filter(minPrice, maxPrice);

        if (filteredList.isEmpty()) {
            QMessageBox::information(filterDialog, "Filter Results", "No food items found in this price range.");
        } else {
            // Render filtered items directly onto tableFood
            tableFood->setRowCount(0);
            int row = 0;
            for (const auto &item : filteredList) {
                tableFood->insertRow(row);
                tableFood->setItem(row, 0, new QTableWidgetItem(item.id));
                tableFood->setItem(row, 1, new QTableWidgetItem(item.name));
                tableFood->setItem(row, 2, new QTableWidgetItem(item.category));
                tableFood->setItem(row, 3, new QTableWidgetItem(QString::number(item.basePrice, 'f', 2)));
                row++;
            }

            QMessageBox::information(filterDialog, "Filter Results", 
                QString("Found %1 food items matching your range.").arg(filteredList.size()));
            filterDialog->accept();
        } });

    filterDialog->exec();
    filterDialog->deleteLater();
}
