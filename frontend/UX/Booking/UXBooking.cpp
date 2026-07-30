#include "backend/Manager/DatabaseManager.h"
#include "backend/Repository/BookingRepository.h"
#include "backend/Repository/CustomerRepository.h"
#include "backend/Repository/RoomRepository.h"
#include "frontend/Observers/Observer.h"
#include "frontend/UI/UI.h"
#include "frontend/UX/UX.h"
#include "frontend/usercheck/backend.h"
#include <QComboBox>
#include <QDateEdit>
#include <QDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>
#include <QSqlQuery>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

// Section 1: Bookings Handler
void MainWindowController::handleLogin_1() {
  setActionBarVisible(true);
  stackedWidget->setCurrentIndex(BookingIndex);
  bookingPage->setSection(0);
  setActiveButton(buttonBooking);
  QString bookingQuery = R"(
    SELECT 
        booking_id AS "Booking ID",
        customer_name AS "Customer Name",
        room_number AS "Room Number",
        check_in AS "Check In",
        check_out AS "Check Out",
        status AS "Status",
        total_price AS "Total Price"
    FROM Bookings
)";

  Backend::loadTableData(tableBooking, bookingQuery);
  btnAdd->setVisible(true);
  btnUpdate->setVisible(true);
  btnDelete->setVisible(true);
  btnFilter->setVisible(true);
  btnAdd->disconnect();
  btnUpdate->disconnect();
  btnDelete->disconnect();
  btnFilter->disconnect();
  connect(btnAdd, &QPushButton::clicked, this,
          &MainWindowController::showAddBookingDialog);
  connect(btnFilter, &QPushButton::clicked, this,
          &MainWindowController::showFilterBookingDialog);
}

void MainWindowController::showFilterBookingDialog() {
  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Filter Bookings");
  dialog->setFixedSize(420, 380);

  dialog->setStyleSheet(
      "QDialog { background-color: #ffffff; }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 13px; }"
      "QComboBox { border: 1px solid #cbd5e1; border-radius: 4px; padding: "
      "6px; background-color: #ffffff; }");

  QVBoxLayout *layout = new QVBoxLayout(dialog);
  layout->setContentsMargins(20, 20, 20, 20);

  QFormLayout *form = new QFormLayout();
  form->setSpacing(12);

  QString inputStyle = "QLineEdit { border: 1px solid #cbd5e1; border-radius: "
                       "4px; padding: 6px; }";

  QLineEdit *txtCustomerId = new QLineEdit(dialog);
  txtCustomerId->setPlaceholderText("All Customer IDs");
  txtCustomerId->setStyleSheet(inputStyle);

  QLineEdit *txtRoomNumber = new QLineEdit(dialog);
  txtRoomNumber->setPlaceholderText("e.g. 101, 102");
  txtRoomNumber->setStyleSheet(inputStyle);

  QComboBox *cbStatus = new QComboBox(dialog);
  cbStatus->addItems(
      {"All", "UNCONFIRMED", "CONFIRMED", "CHECKED_IN", "CHECKED_OUT"});

  QComboBox *cbBookingType = new QComboBox(dialog);
  cbBookingType->addItems({"All", "STANDARD", "WALK_IN"});

  QLineEdit *txtStartDate = new QLineEdit(dialog);
  txtStartDate->setPlaceholderText("YYYY-MM-DD");
  txtStartDate->setText("2026-01-01");
  txtStartDate->setStyleSheet(inputStyle);

  QLineEdit *txtEndDate = new QLineEdit(dialog);
  txtEndDate->setPlaceholderText("YYYY-MM-DD");
  txtEndDate->setText("2026-12-31");
  txtEndDate->setStyleSheet(inputStyle);

  form->addRow("Customer ID:", txtCustomerId);
  form->addRow("Room Number:", txtRoomNumber);
  form->addRow("Status:", cbStatus);
  form->addRow("Booking Type:", cbBookingType);
  form->addRow("Start Date:", txtStartDate);
  form->addRow("End Date:", txtEndDate);
  layout->addLayout(form);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  QPushButton *btnApply = new QPushButton("Apply Filter", dialog);
  QPushButton *btnCancel = new QPushButton("Cancel", dialog);

  btnApply->setStyleSheet(
      "background-color: #3b82f6; color: white; border-radius: 6px; padding: "
      "10px; font-weight: bold;");
  btnCancel->setStyleSheet(
      "background-color: #94a3b8; color: white; border-radius: 6px; padding: "
      "10px; font-weight: bold;");

  buttonLayout->addWidget(btnCancel);
  buttonLayout->addWidget(btnApply);
  layout->addLayout(buttonLayout);

  connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);
  connect(btnApply, &QPushButton::clicked, [=]() {
    QString custIdStr = txtCustomerId->text().trimmed();
    QString roomStr = txtRoomNumber->text().trimmed();
    QString statusStr = cbStatus->currentText();
    QString typeStr = cbBookingType->currentText();
    QString startStr = txtStartDate->text().trimmed();
    QString endStr = txtEndDate->text().trimmed();

    QString queryStr = "SELECT * FROM Bookings WHERE 1=1";

    if (!custIdStr.isEmpty()) {
      queryStr += QString(" AND customer_id = %1").arg(custIdStr.toInt());
    }
    if (!roomStr.isEmpty()) {
      queryStr += QString(" AND room_number LIKE '%%1%'").arg(roomStr);
    }
    if (statusStr != "All") {
      queryStr += QString(" AND status = '%1'").arg(statusStr);
    }
    if (typeStr != "All") {
      queryStr += QString(" AND booking_type = '%1'").arg(typeStr);
    }
    if (!startStr.isEmpty()) {
      queryStr += QString(" AND check_in_time >= '%1 00:00:00'").arg(startStr);
    }
    if (!endStr.isEmpty()) {
      queryStr += QString(" AND check_in_time <= '%1 23:59:59'").arg(endStr);
    }

    queryStr += " ORDER BY check_in_time DESC, id DESC";

    Backend::loadTableData(tableBooking, queryStr);

    QMessageBox::information(dialog, "Filter Results",
                             "Booking filter applied successfully!");
    dialog->accept();
  });

  dialog->exec();
  dialog->deleteLater();
}

void MainWindowController::showAddBookingDialog() {
  QDialog *addDialog = new QDialog(this);
  addDialog->setWindowTitle("Add Booking");
  addDialog->setFixedSize(450, 500);

  addDialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

  QVBoxLayout *mainLayout = new QVBoxLayout(addDialog);
  mainLayout->setContentsMargins(30, 30, 30, 30);

  QLabel *titleLabel = new QLabel("Booking Information", addDialog);
  titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: "
                            "#3730a3; margin-bottom: 20px;");
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
      "QLineEdit:focus, QDateEdit:focus { border: 2px solid #0369a1; "
      "background-color: #f0f9ff; }";

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

  QDateEdit *dateCheckOut =
      new QDateEdit(QDate::currentDate().addDays(1), addDialog);
  dateCheckOut->setCalendarPopup(true);
  dateCheckOut->setStyleSheet(inputStyle);

  formLayout->addRow(new QLabel("Customer ID:", addDialog), txtId);
  formLayout->addRow(new QLabel("Customer Name:", addDialog), txtCustomer);
  formLayout->addRow(new QLabel("Phone Number:", addDialog), txtPhone);
  formLayout->addRow(new QLabel("Room ID:", addDialog), txtRoom);
  formLayout->addRow(new QLabel("Check-In:", addDialog), dateCheckIn);
  formLayout->addRow(new QLabel("Check-Out:", addDialog), dateCheckOut);

  mainLayout->addLayout(formLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 15, 0, 0);
  QPushButton *btnSave = new QPushButton("Save", addDialog);
  QPushButton *btnCancel = new QPushButton("Cancel", addDialog);

  btnSave->setStyleSheet(
      "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #6366f1, stop:1 #8b5cf6); color: white; border: none; "
      "border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: "
      "bold; }"
      "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #4f46e5, stop:1 #7c3aed); }");
  btnCancel->setStyleSheet(
      "background-color: #cbd5e1; color: #475569; border: none; border-radius: "
      "8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

  btnSave->setCursor(Qt::PointingHandCursor);
  btnCancel->setCursor(Qt::PointingHandCursor);

  buttonLayout->addWidget(btnCancel);
  buttonLayout->addWidget(btnSave);
  mainLayout->addLayout(buttonLayout);

  connect(btnCancel, &QPushButton::clicked, addDialog, &QDialog::reject);

  connect(btnSave, &QPushButton::clicked, [=]() {
    QString id = txtId->text();
    QString customer = txtCustomer->text();
    QString phone = txtPhone->text();
    QString room = txtRoom->text();

    if (id.toStdString().size() != 10) {
      QMessageBox::warning(this, "Input Error",
                           "Error: ID Card must be 10 digits long.");
      return;
    }

    for (char g : id.toStdString()) {
      if (!std::isdigit(g)) {
        QMessageBox::warning(this, "Input Error",
                             "Error:ID Card must contain only numbers.");
        return;
      }
    }

    if (phone.toStdString().size() != 10) {
      QMessageBox::warning(this, "Input Error",
                           "Error: Phone Number must be 10 digits long.");
      return;
    }

    for (char g : phone.toStdString()) {
      if (!std::isdigit(g)) {
        QMessageBox::warning(this, "Input Error",
                             "Error: Phone number must contain only numbers.");
        return;
      }
    }

    QString checkInDate = dateCheckIn->date().toString("yyyy-MM-dd");
    QString checkOutDate = dateCheckOut->date().toString("yyyy-MM-dd");

    if (id.isEmpty() || customer.isEmpty() || phone.isEmpty() ||
        room.isEmpty()) {
      QMessageBox::warning(addDialog, "Error",
                           "Please fill in all customer and room details!");
      return;
    }

    QSqlDatabase db = DatabaseManager::instance().database();
    double doublePrice = 0.0;

    RoomRepository roomRepo;
    Room *roomObj = roomRepo.getById(room.toStdString());
    if (roomObj) {
      doublePrice = roomObj->getBasePrice();
      delete roomObj;
    } else {
      QSqlQuery rq(db);
      rq.prepare("SELECT base_price FROM ListRooms WHERE room_id = :rm OR "
                 "room_number = :rm");
      rq.bindValue(":rm", room);
      if (rq.exec() && rq.next()) {
        doublePrice = rq.value(0).toDouble();
      }
    }
    if (doublePrice <= 0.0)
      doublePrice = 1000000.0;

    CustomerRepository re;
    Customer a(customer.toStdString(), phone.toStdString(), id.toStdString());
    re.add(a);

    int realCustomerId = 0;
    QSqlQuery custQuery(db);
    custQuery.prepare("SELECT id FROM Customer WHERE id_customer = :id_cust");
    custQuery.bindValue(":id_cust", id);
    if (custQuery.exec() && custQuery.next()) {
      realCustomerId = custQuery.value(0).toInt();
    } else {
      realCustomerId = id.toInt();
    }

    QSqlQuery query(db);
    query.prepare("UPDATE ListRooms SET status = 'Occupied' WHERE room_id = "
                  ":rm OR room_number = :rm");
    query.bindValue(":rm", room);

    if (!query.exec()) {
      QMessageBox::critical(addDialog, "Database Error",
                            "Failed to update Room status:\n" +
                                query.lastError().text());
      return;
    }

    BookingRepository r;
    BookingData t;
    t.customerId = realCustomerId;
    t.roomNumber = room;
    t.checkInTime = checkInDate;
    t.checkOutTime = checkOutDate;
    t.totalPrice = doublePrice;
    bool addSuccess = r.add(t);

    if (addSuccess) {
      HotelEventManager::instance().notifyRoomStatus(RoomEvent{
          room.toStdString(), RoomStatus::Occupied,
          QDateTime::currentDateTime().toString(Qt::ISODate).toStdString()});

      QSqlQuery lastIdQuery(db);
      int newBookingId = 0;
      if (lastIdQuery.exec("SELECT MAX(id) FROM Bookings") &&
          lastIdQuery.next()) {
        newBookingId = lastIdQuery.value(0).toInt();
      }

      HotelEventManager::instance().notifyBookingStatus(BookingEvent{
          newBookingId, customer.toStdString(), room.toStdString(),
          BookingStatus::CONFIRMED, doublePrice,
          QDateTime::currentDateTime().toString(Qt::ISODate).toStdString()});
    }

    QMessageBox::information(
        addDialog, "Success",
        "Booking created and room status updated successfully!");
    addDialog->accept();
  });

  addDialog->exec();
  addDialog->deleteLater();
}
