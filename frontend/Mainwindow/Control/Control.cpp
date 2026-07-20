#include "Control.h"
#include "../UI/UI.h"
#include "../../backend/backend.h"
#include "../../Repository/CustomerRepository.h"
#include "../../Repository/RoomRepository.h"
#include "../../Checkout/CheckoutPage.h"
#include "../../Manager/DatabaseManager.h"
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QComboBox>
#include <QDateEdit>
#include <QSqlDatabase>
#include <QLineEdit>

#include <QSqlQuery>
#include <QSqlError>
#include "../../Booking/Booking.h"
#include "../../Room/DerivedRooms.h"

MainWindowController::MainWindowController(QWidget *parent) : MainWindowUi(parent)
{
    MainWindowUi::setupUi();
    initConnections();
}

void MainWindowController::initConnections()
{
    // FIX: Thay &MainWindow:: thành &MainWindowController:: vì đây là class Controller
    connect(button1, &QPushButton::clicked, this, &MainWindowController::handleLogin_1);
    connect(button2, &QPushButton::clicked, this, &MainWindowController::handleLogin_2);
    connect(button3, &QPushButton::clicked, this, &MainWindowController::handleLogin_3);
    connect(button4, &QPushButton::clicked, this, &MainWindowController::handleLogin_4);
    connect(button5, &QPushButton::clicked, this, &MainWindowController::handleLogin_5);
    connect(button6, &QPushButton::clicked, this, &MainWindowController::handleLogin_6);
    connect(button7, &QPushButton::clicked, this, &MainWindowController::handleLogin_7);
    connect(button8, &QPushButton::clicked, this, &MainWindowController::handleLogin_8);
    connect(button9, &QPushButton::clicked, this, &MainWindowController::handleLogin_9);
    connect(button10, &QPushButton::clicked, this, &MainWindowController::handleLogin_10);
    connect(buttonCheckout, &QPushButton::clicked, this, &MainWindowController::handleCheckout);
}

void MainWindowController::handleLogin_1()
{
    setActionBarVisible(true);
    stackedWidget->setCurrentIndex(0);
    setActiveButton(button1);
    Backend::loadTableData(tableBooking, "SELECT * FROM Bookings");
    btnAdd->disconnect();
    connect(btnAdd, &QPushButton::clicked, this, &MainWindowController::showAddBookingDialog);
}

void MainWindowController::handleLogin_2()
{
    stackedWidget->setCurrentIndex(1);
    setActionBarVisible(true);
    setActiveButton(button2);
    btnAdd->disconnect();
    Backend::loadTableData(tableBookingItems, "SELECT * FROM BookingServiceItems");
}

void MainWindowController::handleLogin_3()
{
    stackedWidget->setCurrentIndex(2);
    setActionBarVisible(true);
    setActiveButton(button3);
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

void MainWindowController::handleLogin_4()
{
    stackedWidget->setCurrentIndex(3);
    setActionBarVisible(true);
    setActiveButton(button4);
    Backend::loadTableData(tableFood, "SELECT * FROM FoodOptions");
    btnAdd->disconnect();
}

void MainWindowController::handleLogin_5()
{
    stackedWidget->setCurrentIndex(4);
    setActionBarVisible(true);
    setActiveButton(button5);
    Backend::loadTableData(tableInventory, "SELECT * FROM Inventory");
    btnAdd->disconnect();
}

void MainWindowController::handleLogin_6()
{
    stackedWidget->setCurrentIndex(5);
    setActionBarVisible(true);
    setActiveButton(button6);
    Backend::loadTableData(tableInventoryLog, "SELECT * FROM InventoryLog");
    btnAdd->disconnect();
}

void MainWindowController::handleLogin_7()
{
    stackedWidget->setCurrentIndex(6);
    setActionBarVisible(true);
    setActiveButton(button7);
    Backend::loadTableData(tableRoom, "SELECT * FROM ListRooms");

    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();

    connect(btnAdd, &QPushButton::clicked, this, &MainWindowController::showAddRoomDialog);
    connect(btnUpdate, &QPushButton::clicked, this, &MainWindowController::showUpdateRoomDialog);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindowController::showDeleteRoomDialog);
}

void MainWindowController::handleLogin_8()
{
    stackedWidget->setCurrentIndex(7);
    setActionBarVisible(true);
    setActiveButton(button8);
    Backend::loadTableData(tableRoomType, "SELECT * FROM RoomTypeCatalog");
    btnAdd->disconnect();
}

void MainWindowController::handleLogin_9()
{
    stackedWidget->setCurrentIndex(8);
    setActionBarVisible(true);
    setActiveButton(button9);
    Backend::loadTableData(tableService, "SELECT * FROM ListServiceItems");
    btnAdd->disconnect();
}

void MainWindowController::handleLogin_10()
{
    stackedWidget->setCurrentIndex(9);
    setActionBarVisible(true);
    setActiveButton(button10);
    Backend::loadTableData(tableBill, "SELECT * FROM Bills");
    btnAdd->disconnect();
}

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

void MainWindowController::setActiveButton(QPushButton *clickedButton)
{
    QList<QPushButton *> buttons = {
        button1, button2, button3, button4, button5,
        button6, button7, button8, button9, button10, buttonCheckout};

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

void MainWindowController::showUpdateCustomerDialog()
{
    handleLogin_3();
}

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