#include "backend/Manager/DatabaseManager.h"
#include "backend/Repository/BookingRepository.h"
#include "backend/Repository/CustomerRepository.h"
#include "backend/Repository/RoomRepository.h"
#include "frontend/Observers/Observer.h"
#include "frontend/UI/UI.h"
#include "frontend/UX/UX.h"
#include "frontend/usercheck/backend.h"
#include <QCheckBox>
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
void MainWindowController::showBookingTab() {
  setActiveButton(buttonBooking);
  bookingPage->setSection(0);
  setActionBarVisible(true);
  stackedWidget->setCurrentIndex(BookingIndex);
  QString bookingQuery =
      "SELECT id AS 'Booking ID', customer_id AS 'Customer ID', room_number AS "
      "'Room Number', "
      "check_in_time AS 'Check-in', check_out_time AS 'Check-out', status AS "
      "'Status', "
      "deposit_amount AS 'Deposit Amount', deposit_status AS 'Deposit Status', "
      "total_price AS 'Total Price (VND)' FROM Bookings ORDER BY check_in_time "
      "DESC, id DESC";

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
  connect(btnUpdate, &QPushButton::clicked, this,
          &MainWindowController::showUpdateBookingDialog);
  connect(btnDelete, &QPushButton::clicked, this,
          &MainWindowController::showDeleteBookingDialog);
  connect(btnFilter, &QPushButton::clicked, this,
          &MainWindowController::showFilterBookingDialog);
}

void MainWindowController::showUpdateBookingDialog() {
  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Update Booking");
  dialog->setFixedSize(520, 680);

  dialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

  QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
  mainLayout->setContentsMargins(30, 30, 30, 30);

  QLabel *titleLabel = new QLabel("Update Booking Details", dialog);
  titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: "
                            "#3730a3; margin-bottom: 15px;");
  mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

  QFormLayout *formLayout = new QFormLayout();
  formLayout->setSpacing(12);

  QString inputStyle = "QLineEdit, QComboBox, QDateEdit {"
                       "   background-color: #ffffff; "
                       "   border: 2px solid #38bdf8; "
                       "   border-radius: 8px; "
                       "   padding: 8px; "
                       "   font-size: 14px; "
                       "   color: #0f172a; "
                       "}"
                       "QLineEdit:hover, QComboBox:hover, QDateEdit:hover { "
                       "border: 2px solid #0284c7; }"
                       "QLineEdit:focus, QComboBox:focus, QDateEdit:focus { "
                       "border: 2px solid #0369a1; background-color: #f0f9ff; }"
                       "QComboBox::drop-down { border: none; width: 25px; }"
                       "QComboBox::down-arrow { image: none; }"
                       "QComboBox QAbstractItemView, QComboBox QListView {"
                       "   background-color: #ffffff; "
                       "   color: #0f172a; "
                       "   border: 2px solid #38bdf8; "
                       "   border-radius: 6px; "
                       "   selection-background-color: #f0f9ff; "
                       "   selection-color: #0369a1; "
                       "   outline: none;"
                       "}";

  QLineEdit *txtBookingId = new QLineEdit(dialog);
  txtBookingId->setPlaceholderText("Booking ID (Required)...");
  txtBookingId->setStyleSheet(inputStyle);

  QLineEdit *txtCustomerId = new QLineEdit(dialog);
  txtCustomerId->setPlaceholderText("Customer ID...");
  txtCustomerId->setStyleSheet(inputStyle);

  QLineEdit *txtRoomNumber = new QLineEdit(dialog);
  txtRoomNumber->setPlaceholderText("Room Number...");
  txtRoomNumber->setStyleSheet(inputStyle);

  QDateEdit *dateCheckIn = new QDateEdit(QDate::currentDate(), dialog);
  dateCheckIn->setCalendarPopup(true);
  dateCheckIn->setStyleSheet(inputStyle);

  QDateEdit *dateCheckOut =
      new QDateEdit(QDate::currentDate().addDays(1), dialog);
  dateCheckOut->setCalendarPopup(true);
  dateCheckOut->setStyleSheet(inputStyle);

  QComboBox *cbStatus = new QComboBox(dialog);
  cbStatus->addItems({"UNCONFIRMED", "CONFIRMED", "CHECKED_IN", "CHECKED_OUT"});
  cbStatus->setStyleSheet(inputStyle);

  QLineEdit *txtDepositAmount = new QLineEdit(dialog);
  txtDepositAmount->setPlaceholderText("Deposit Amount (VND)...");
  txtDepositAmount->setStyleSheet(inputStyle);

  QComboBox *cbDepositStatus = new QComboBox(dialog);
  cbDepositStatus->addItems({"NONE", "HELD", "RETURNED"});
  cbDepositStatus->setStyleSheet(inputStyle);

  if (tableBooking && tableBooking->currentRow() >= 0) {
    int row = tableBooking->currentRow();
    if (tableBooking->item(row, 0))
      txtBookingId->setText(tableBooking->item(row, 0)->text());
    if (tableBooking->item(row, 1))
      txtCustomerId->setText(tableBooking->item(row, 1)->text());
    if (tableBooking->item(row, 2))
      txtRoomNumber->setText(tableBooking->item(row, 2)->text());
    if (tableBooking->item(row, 3)) {
      QDate inD = QDate::fromString(tableBooking->item(row, 3)->text().left(10),
                                    "yyyy-MM-dd");
      if (inD.isValid())
        dateCheckIn->setDate(inD);
    }
    if (tableBooking->item(row, 4)) {
      QDate outD = QDate::fromString(
          tableBooking->item(row, 4)->text().left(10), "yyyy-MM-dd");
      if (outD.isValid())
        dateCheckOut->setDate(outD);
    }
    if (tableBooking->item(row, 5))
      cbStatus->setCurrentText(tableBooking->item(row, 5)->text());
    if (tableBooking->item(row, 6))
      txtDepositAmount->setText(tableBooking->item(row, 6)->text());
    if (tableBooking->item(row, 7))
      cbDepositStatus->setCurrentText(tableBooking->item(row, 7)->text());
  }

  formLayout->addRow("Booking ID:", txtBookingId);
  formLayout->addRow("Customer ID:", txtCustomerId);
  formLayout->addRow("Room Number:", txtRoomNumber);
  formLayout->addRow("Check-In:", dateCheckIn);
  formLayout->addRow("Check-Out:", dateCheckOut);
  formLayout->addRow("Status:", cbStatus);
  formLayout->addRow("Deposit Amount:", txtDepositAmount);
  formLayout->addRow("Deposit Status:", cbDepositStatus);

  mainLayout->addLayout(formLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 15, 0, 0);
  QPushButton *btnSave = new QPushButton("Save Update", dialog);
  QPushButton *btnCancel = new QPushButton("Cancel", dialog);

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

  connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);

  connect(btnSave, &QPushButton::clicked, [=]() {
    QString bIdStr = txtBookingId->text().trimmed();
    if (bIdStr.isEmpty()) {
      QMessageBox::warning(dialog, "Input Error",
                           "Booking ID is required to update a booking.");
      return;
    }

    bool okId = false;
    int bookingId = bIdStr.toInt(&okId);
    if (!okId || bookingId <= 0) {
      QMessageBox::warning(dialog, "Input Error",
                           "Booking ID must be a positive integer.");
      return;
    }

    QString customerIdStr = txtCustomerId->text().trimmed();
    QString roomNumberStr = txtRoomNumber->text().trimmed();
    QString checkInStr =
        dateCheckIn->date().toString("yyyy-MM-dd") + " 14:00:00";
    QString checkOutStr =
        dateCheckOut->date().toString("yyyy-MM-dd") + " 12:00:00";
    QString statusStr = cbStatus->currentText();
    double depositAmt = txtDepositAmount->text().toDouble();
    QString depositStatusStr = cbDepositStatus->currentText();

    // Prevent deposit from exceeding total room base price
    double roomBasePrice = 0.0;
    if (!roomNumberStr.isEmpty()) {
      QSqlQuery rq(DatabaseManager::instance().database());
      rq.prepare("SELECT base_price FROM ListRooms WHERE room_number = :rm OR room_id = :rm");
      rq.bindValue(":rm", roomNumberStr);
      if (rq.exec() && rq.next()) {
        roomBasePrice = rq.value(0).toDouble();
      }
    }
    int nights = dateCheckIn->date().daysTo(dateCheckOut->date());
    if (nights <= 0) nights = 1;
    double maxAllowedDeposit = (roomBasePrice > 0.0) ? (roomBasePrice * nights) : 0.0;

    if (maxAllowedDeposit > 0.0 && depositAmt > maxAllowedDeposit) {
      QMessageBox::warning(dialog, "Input Error",
                           QString("Deposit amount (%1) cannot exceed the room base price (%2).")
                               .arg(QLocale(QLocale::English).toString(depositAmt, 'f', 0))
                               .arg(QLocale(QLocale::English).toString(maxAllowedDeposit, 'f', 0)));
      return;
    }

    if (depositAmt > 0 && depositStatusStr == "NONE") {
      depositStatusStr = "HELD";
    }
    if ((depositAmt > 0 || depositStatusStr == "HELD") && statusStr == "UNCONFIRMED") {
      statusStr = "CONFIRMED";
    }

    QDateTime inDT = QDateTime::fromString(checkInStr, "yyyy-MM-dd hh:mm:ss");
    QDateTime outDT = QDateTime::fromString(checkOutStr, "yyyy-MM-dd hh:mm:ss");

    BookingRepository repo;
    bool success = repo.updateBooking(bookingId, customerIdStr.toInt(),
                                      roomNumberStr, inDT, outDT, 0.0,
                                      depositAmt, depositStatusStr, statusStr);

    if (success) {
      QMessageBox::information(dialog, "Success",
                               "Booking updated successfully!");
      Backend::loadTableData(
          tableBooking,
          "SELECT id AS 'Booking ID', customer_id AS 'Customer ID', "
          "room_number AS 'Room Number', check_in_time AS 'Check-In', "
          "check_out_time AS 'Check-Out', status AS 'Status', "
          "deposit_amount AS 'Deposit Amount', deposit_status AS 'Deposit "
          "Status', "
          "total_price AS 'Total Price (VND)' FROM Bookings "
          "ORDER BY check_in_time DESC, id DESC");
      dialog->accept();
    } else {
      QMessageBox::critical(
          dialog, "Error",
          "Failed to update Booking. Please verify the Booking ID exists.");
    }
  });

  dialog->exec();
  dialog->deleteLater();
}

void MainWindowController::showDeleteBookingDialog() {
  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Delete Booking");
  dialog->setFixedSize(380, 220);

  dialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

  QVBoxLayout *layout = new QVBoxLayout(dialog);
  layout->setContentsMargins(30, 20, 30, 20);

  QLabel *titleLabel = new QLabel("Remove Booking", dialog);
  titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: "
                            "#e11d48; margin-bottom: 10px;");
  layout->addWidget(titleLabel, 0, Qt::AlignCenter);

  QFormLayout *form = new QFormLayout();

  QString inputStyle = "QLineEdit {"
                       "   background-color: #ffffff; "
                       "   border: 2px solid #38bdf8; "
                       "   border-radius: 8px; "
                       "   padding: 10px; "
                       "   font-size: 14px; "
                       "   color: #0f172a; "
                       "}"
                       "QLineEdit:hover { border: 2px solid #0284c7; }"
                       "QLineEdit:focus { border: 2px solid #0369a1; "
                       "background-color: #f0f9ff; }";

  QLineEdit *txtId = new QLineEdit(dialog);
  txtId->setPlaceholderText("Enter Booking ID...");
  txtId->setStyleSheet(inputStyle);

  if (tableBooking && tableBooking->currentRow() >= 0) {
    int row = tableBooking->currentRow();
    if (tableBooking->item(row, 0)) {
      txtId->setText(tableBooking->item(row, 0)->text());
    }
  }

  form->addRow("Booking ID:", txtId);
  layout->addLayout(form);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 15, 0, 0);
  QPushButton *btnDelete = new QPushButton("Delete", dialog);
  QPushButton *btnCancel = new QPushButton("Cancel", dialog);

  btnDelete->setStyleSheet(
      "background-color: #e11d48; color: white; border: none; border-radius: "
      "8px; padding: 10px 0; font-size: 14px; font-weight: bold;");
  btnCancel->setStyleSheet(
      "background-color: #cbd5e1; color: #475569; border: none; border-radius: "
      "8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

  btnDelete->setCursor(Qt::PointingHandCursor);
  btnCancel->setCursor(Qt::PointingHandCursor);

  buttonLayout->addWidget(btnCancel);
  buttonLayout->addWidget(btnDelete);
  layout->addLayout(buttonLayout);

  connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);

  connect(btnDelete, &QPushButton::clicked, dialog, [=]() {
    QString idStr = txtId->text().trimmed();
    if (idStr.isEmpty()) {
      QMessageBox::warning(dialog, "Error", "Please enter a Booking ID!");
      return;
    }

    bool ok = false;
    int bookingId = idStr.toInt(&ok);
    if (!ok || bookingId <= 0) {
      QMessageBox::warning(dialog, "Error",
                           "Booking ID must be a valid integer!");
      return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        dialog, "Confirm",
        QString("Are you sure you want to delete Booking ID %1?")
            .arg(bookingId),
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No)
      return;

    BookingRepository repo;
    bool success = repo.remove(bookingId);

    if (success) {
      QMessageBox::information(dialog, "Success",
                               "Booking deleted successfully!");
      dialog->accept();
      Backend::loadTableData(
          tableBooking,
          "SELECT id AS 'Booking ID', customer_id AS 'Customer ID', "
          "room_number AS 'Room Number', check_in_time AS 'Check-In', "
          "check_out_time AS 'Check-Out', status AS 'Status', "
          "deposit_amount AS 'Deposit Amount', deposit_status AS 'Deposit "
          "Status', "
          "total_price AS 'Total Price (VND)' FROM Bookings "
          "ORDER BY check_in_time DESC, id DESC");
      Backend::loadTableData(
          tableRoom,
          "SELECT room_id AS 'Room ID', room_number AS 'Room Number', "
          "room_type AS 'Type', status AS 'Status', base_price AS 'Price', "
          "number_people AS 'Number People' FROM ListRooms");
    } else {
      QMessageBox::critical(
          dialog, "Error",
          "Cannot delete booking. It may not exist in the database.");
    }
  });

  dialog->exec();
  dialog->deleteLater();
}

void MainWindowController::showFilterBookingDialog() {
  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Filter Bookings");
  dialog->setFixedSize(520, 640);

  dialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

  QVBoxLayout *layout = new QVBoxLayout(dialog);
  layout->setContentsMargins(30, 30, 30, 30);

  QLabel *titleLabel = new QLabel("Filter Bookings", dialog);
  titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: "
                            "#3730a3; margin-bottom: 15px;");
  layout->addWidget(titleLabel, 0, Qt::AlignCenter);

  QFormLayout *form = new QFormLayout();
  form->setSpacing(12);

  QString inputStyle =
      "QLineEdit, QComboBox {"
      "   background-color: #ffffff; "
      "   border: 2px solid #38bdf8; "
      "   border-radius: 8px; "
      "   padding: 8px; "
      "   font-size: 14px; "
      "   color: #0f172a; "
      "}"
      "QLineEdit:hover, QComboBox:hover { border: 2px solid #0284c7; }"
      "QLineEdit:focus, QComboBox:focus { border: 2px solid #0369a1; "
      "background-color: #f0f9ff; }"
      "QComboBox::drop-down { border: none; width: 25px; }"
      "QComboBox::down-arrow { image: none; }"
      "QComboBox QAbstractItemView, QComboBox QListView {"
      "   background-color: #ffffff; "
      "   color: #0f172a; "
      "   border: 2px solid #38bdf8; "
      "   border-radius: 6px; "
      "   selection-background-color: #f0f9ff; "
      "   selection-color: #0369a1; "
      "   outline: none;"
      "}";

  QLineEdit *txtCustomerId = new QLineEdit(dialog);
  txtCustomerId->setPlaceholderText("All Customer IDs");
  txtCustomerId->setStyleSheet(inputStyle);

  QLineEdit *txtRoomNumber = new QLineEdit(dialog);
  txtRoomNumber->setPlaceholderText("e.g. 101, 102");
  txtRoomNumber->setStyleSheet(inputStyle);

  QComboBox *cbStatus = new QComboBox(dialog);
  cbStatus->addItems(
      {"All", "UNCONFIRMED", "CONFIRMED", "CHECKED_IN", "CHECKED_OUT"});
  cbStatus->setStyleSheet(inputStyle);

  QComboBox *cbBookingType = new QComboBox(dialog);
  cbBookingType->addItems({"All", "STANDARD", "WALK_IN"});
  cbBookingType->setStyleSheet(inputStyle);

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
  buttonLayout->setContentsMargins(0, 15, 0, 0);
  QPushButton *btnApply = new QPushButton("Apply Filter", dialog);
  QPushButton *btnReset = new QPushButton("Reset", dialog);
  QPushButton *btnCancel = new QPushButton("Cancel", dialog);

  btnApply->setStyleSheet(
      "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #10b981, stop:1 #059669); color: white; border: none; "
      "border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: "
      "bold; }"
      "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #059669, stop:1 #047857); }");
  btnReset->setStyleSheet(
      "QPushButton { background-color: #cbd5e1; color: #475569; border: none; "
      "border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: "
      "bold; }"
      "QPushButton:hover { background-color: #94a3b8; color: #1e293b; }");
  btnCancel->setStyleSheet(
      "background-color: #e2e8f0; color: #64748b; border: none; border-radius: "
      "8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

  btnApply->setCursor(Qt::PointingHandCursor);
  btnReset->setCursor(Qt::PointingHandCursor);
  btnCancel->setCursor(Qt::PointingHandCursor);

  buttonLayout->addWidget(btnCancel);
  buttonLayout->addWidget(btnReset);
  buttonLayout->addWidget(btnApply);
  layout->addLayout(buttonLayout);

  connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);
  connect(btnReset, &QPushButton::clicked, [=]() {
    Backend::loadTableData(
        tableBooking,
        "SELECT id AS 'Booking ID', customer_id AS 'Customer ID', "
        "room_number AS 'Room Number', check_in_time AS 'Check-In', "
        "check_out_time AS 'Check-Out', status AS 'Status', "
        "deposit_amount AS 'Deposit Amount', deposit_status AS 'Deposit "
        "Status', "
        "total_price AS 'Total Price (VND)' FROM Bookings "
        "ORDER BY check_in_time DESC, id DESC");
    dialog->accept();
  });
  connect(btnApply, &QPushButton::clicked, [=]() {
    QString custIdStr = txtCustomerId->text().trimmed();
    QString roomStr = txtRoomNumber->text().trimmed();
    QString statusStr = cbStatus->currentText();
    QString typeStr = cbBookingType->currentText();
    QString startStr = txtStartDate->text().trimmed();
    QString endStr = txtEndDate->text().trimmed();

    QString queryStr =
        "SELECT id AS 'Booking ID', customer_id AS 'Customer ID', "
        "room_number AS 'Room Number', check_in_time AS 'Check-in', "
        "check_out_time AS 'Check-out', status AS 'Status', "
        "deposit_amount AS 'Deposit Amount', deposit_status AS 'Deposit "
        "Status', "
        "total_price AS 'Total Price (VND)' "
        "FROM Bookings WHERE 1=1";

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
  addDialog->setFixedSize(500, 470);

  addDialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 13px; }");

  QVBoxLayout *mainLayout = new QVBoxLayout(addDialog);
  mainLayout->setContentsMargins(25, 18, 25, 18);

  QLabel *titleLabel = new QLabel("BOOKING INFORMATION", addDialog);
  titleLabel->setStyleSheet(
      "font-size: 20px; font-weight: 800; color: "
      "#3730a3; margin-bottom: 8px; letter-spacing: 1px;");
  mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

  QFormLayout *formLayout = new QFormLayout();
  formLayout->setSpacing(8);
  formLayout->setLabelAlignment(Qt::AlignLeft);
  formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

  QString inputStyle = "QLineEdit, QComboBox, QDateEdit {"
                       "   background-color: #ffffff; "
                       "   border: 2px solid #38bdf8; "
                       "   border-radius: 6px; "
                       "   padding: 5px 8px; "
                       "   font-size: 13px; "
                       "   color: #0f172a; "
                       "}"
                       "QLineEdit:hover, QComboBox:hover, QDateEdit:hover { "
                       "border: 2px solid #0284c7; }"
                       "QLineEdit:focus, QComboBox:focus, QDateEdit:focus { "
                       "border: 2px solid #0369a1; "
                       "background-color: #f0f9ff; }"
                       "QComboBox::drop-down { border: none; width: 25px; }"
                       "QComboBox::down-arrow { image: none; }"
                       "QComboBox QAbstractItemView, QComboBox QListView {"
                       "   background-color: #ffffff; "
                       "   color: #0f172a; "
                       "   border: 2px solid #38bdf8; "
                       "   border-radius: 6px; "
                       "   selection-background-color: #f0f9ff; "
                       "   selection-color: #0369a1; "
                       "   outline: none;"
                       "}";

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

  for (QWidget *w : std::initializer_list<QWidget *>{
           txtId, txtCustomer, txtPhone, txtRoom, dateCheckIn, dateCheckOut}) {
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  }

  QString checkBoxStyle =
      "QCheckBox { color: #0f172a; font-weight: bold; font-size: 13px; "
      "spacing: 8px; background: transparent; margin-top: 8px; }"
      "QCheckBox::indicator { width: 18px; height: 18px; border: 2px solid "
      "#38bdf8; border-radius: 4px; background-color: #ffffff; }"
      "QCheckBox::indicator:hover { border-color: #0284c7; }"
      "QCheckBox::indicator:checked { background-color: #0369a1; border-color: "
      "#0369a1; image: url(\"data:image/svg+xml;utf8,<svg "
      "xmlns='http://www.w3.org/2000/svg' width='14' height='14' viewBox='0 0 "
      "24 24' fill='none' stroke='white' stroke-width='4' "
      "stroke-linecap='round' stroke-linejoin='round'><polyline points='20 6 9 "
      "17 4 12'></polyline></svg>\"); }";

  QCheckBox *chkDeposit = new QCheckBox("Yes (Base Room Rate)", addDialog);
  chkDeposit->setStyleSheet(checkBoxStyle);
  chkDeposit->setChecked(false);

  // Tích điểm Membership
  QCheckBox *chkMembership = new QCheckBox("Register & Earn Points", addDialog);
  chkMembership->setStyleSheet(checkBoxStyle);
  chkMembership->setChecked(false);

  QLabel *lblDeposit = new QLabel("Pay Deposit:", addDialog);
  lblDeposit->setStyleSheet(
      "color: #1e293b; font-weight: bold; font-size: 13px; margin-top: 8px;");

  QLabel *lblMembership = new QLabel("Membership:", addDialog);
  lblMembership->setStyleSheet(
      "color: #1e293b; font-weight: bold; font-size: 13px; margin-top: 8px;");

  formLayout->addRow(new QLabel("Customer ID:", addDialog), txtId);
  formLayout->addRow(new QLabel("Customer Name:", addDialog), txtCustomer);
  formLayout->addRow(new QLabel("Phone Number:", addDialog), txtPhone);
  formLayout->addRow(new QLabel("Room ID:", addDialog), txtRoom);
  formLayout->addRow(new QLabel("Check-In:", addDialog), dateCheckIn);
  formLayout->addRow(new QLabel("Check-Out:", addDialog), dateCheckOut);
  formLayout->addRow(lblDeposit, chkDeposit);
  formLayout->addRow(lblMembership, chkMembership);

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
    QString id = txtId->text().trimmed();
    QString customer = txtCustomer->text().trimmed();
    QString phone = txtPhone->text().trimmed();
    QString room = txtRoom->text().trimmed();

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

    if (chkDeposit->isChecked() && room.isEmpty()) {
      QMessageBox::warning(
          addDialog, "Deposit Error",
          "Please enter a valid Room ID to pay a deposit for the room.");
      return;
    }

    QString checkInDate =
        dateCheckIn->date().toString("yyyy-MM-dd") + " 14:00:00";
    QString checkOutDate =
        dateCheckOut->date().toString("yyyy-MM-dd") + " 12:00:00";

    if (id.isEmpty() || customer.isEmpty() || phone.isEmpty()) {
      QMessageBox::warning(
          addDialog, "Error",
          "Please fill in customer details (ID, Name, Phone)!");
      return;
    }

    QSqlDatabase db = DatabaseManager::instance().database();
    double doublePrice = 0.0;

    // 1. If room number/ID is provided, check if room exists and is available
    if (!room.isEmpty()) {
      QSqlQuery checkRoomExist(db);
      checkRoomExist.prepare("SELECT status FROM ListRooms WHERE room_id = :rm "
                             "OR room_number = :rm");
      checkRoomExist.bindValue(":rm", room);
      if (!checkRoomExist.exec() || !checkRoomExist.next()) {
        QMessageBox::warning(
            addDialog, "Room Error",
            QString("Room number/ID '%1' does not exist in the system!")
                .arg(room));
        return;
      }

      QString currentRmStatus = checkRoomExist.value(0).toString().trimmed();
      if (currentRmStatus.compare("Available", Qt::CaseInsensitive) != 0 &&
          currentRmStatus.compare("AVAILABLE", Qt::CaseInsensitive) != 0) {
        QMessageBox::warning(
            addDialog, "Room Error",
            QString("Room '%1' is currently %2 and not available for booking!")
                .arg(room, currentRmStatus.isEmpty() ? "Unavailable"
                                                     : currentRmStatus));
        return;
      }

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
      if (doublePrice <= 0.0) {
        QSqlQuery catalogQuery(db);
        catalogQuery.prepare(
            "SELECT R.base_price FROM ListRooms L "
            "JOIN RoomTypeCatalog R ON L.room_type = R.room_type "
            "WHERE L.room_id = :rm OR L.room_number = :rm");
        catalogQuery.bindValue(":rm", room);
        if (catalogQuery.exec() && catalogQuery.next()) {
          doublePrice = catalogQuery.value(0).toDouble();
        }
      }
    }

    int currentTierVal = static_cast<int>(chkMembership->isChecked()
                                              ? MembershipTier::Unknown
                                              : MembershipTier::Temporary);

    CustomerRepository re;
    Customer a(customer.toStdString(), phone.toStdString(), id.toStdString());
    if (!room.isEmpty()) {
      a.setIdroom(room.toStdString());
    }

    int realCustomerId = 0;

    QSqlQuery custQuery(db);
    custQuery.prepare(
        "SELECT id, Point, Type FROM Customer WHERE id_customer = :id_cust");
    custQuery.bindValue(":id_cust", id);

    if (custQuery.exec() && custQuery.next()) {
      a.setPoint(custQuery.value("Point").toInt());
      int dbTier = custQuery.value("Type").toInt();
      if (dbTier >= 0) {
        currentTierVal = dbTier;
      } else if (chkMembership->isChecked()) {
        currentTierVal = static_cast<int>(MembershipTier::Unknown);
      }
      re.update(a);
      realCustomerId = custQuery.value("id").toInt();
    } else {
      re.add(a);
      QSqlQuery newCustQuery(db);
      newCustQuery.prepare(
          "SELECT id FROM Customer WHERE id_customer = :id_cust");
      newCustQuery.bindValue(":id_cust", id);
      if (newCustQuery.exec() && newCustQuery.next()) {
        realCustomerId = newCustQuery.value("id").toInt();
      } else {
        realCustomerId = id.toInt();
      }
    }

    if (!db.transaction()) {
      QMessageBox::critical(addDialog, "Database Error",
                            "Cannot start booking transaction.");
      return;
    }

    double depositAmt =
        (chkDeposit->isChecked() && !room.isEmpty()) ? doublePrice : 0.0;
    QString depositStatusStr = (depositAmt > 0.0) ? "HELD" : "NONE";

    BookingRepository bookingRepository;
    BookingData bookingData;
    bookingData.customerId = realCustomerId;
    bookingData.roomNumber = room;
    bookingData.checkInTime = checkInDate;
    bookingData.checkOutTime = checkOutDate;
    bookingData.totalPrice = doublePrice;
    bookingData.depositAmount = depositAmt;
    bookingData.depositStatus = depositStatusStr;

    if (!bookingRepository.add(bookingData)) {
      db.rollback();
      QMessageBox::critical(
          addDialog, "Booking Error",
          "Cannot create booking. The room may no longer be available.");
      return;
    }

    QSqlQuery updateRoom(db);
    updateRoom.prepare("UPDATE ListRooms "
                       "SET status = 'Occupied' "
                       "WHERE room_id = :rm OR room_number = :rm");
    updateRoom.bindValue(":rm", room);

    if (!updateRoom.exec()) {
      db.rollback();
      QMessageBox::critical(addDialog, "Database Error",
                            "Booking was cancelled because the room status "
                            "could not be updated:\n" +
                                updateRoom.lastError().text());
      return;
    }

    if (!db.commit()) {
      db.rollback();
      QMessageBox::critical(addDialog, "Database Error",
                            "Cannot save the booking transaction.");
      return;
    }

    QSqlQuery lastIdQuery(db);
    int newBookingId = 0;
    if (lastIdQuery.exec("SELECT MAX(id) FROM Bookings") &&
        lastIdQuery.next()) {
      newBookingId = lastIdQuery.value(0).toInt();
    }

    HotelEventManager::instance().notifyRoomStatus(RoomEvent{
        room.toStdString(), RoomStatus::Occupied,
        QDateTime::currentDateTime().toString(Qt::ISODate).toStdString()});

    HotelEventManager::instance().notifyBookingStatus(BookingEvent{
        newBookingId, customer.toStdString(), room.toStdString(),
        BookingStatus::CONFIRMED, doublePrice,
        QDateTime::currentDateTime().toString(Qt::ISODate).toStdString()});

    QMessageBox::information(
        addDialog, "Success",
        "Booking created and room status updated successfully!");

    addDialog->accept();
    showBookingTab();
  });

  addDialog->exec();
  addDialog->deleteLater();
}