#include "backend/Repository/CustomerRepository.h"
#include "frontend/UI/UI.h"
#include "frontend/UX/UX.h"
#include "frontend/usercheck/usercheck.h"
#include <QDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QSqlQuery>
#include <QSqlError>
#include "backend/Manager/DatabaseManager.h"

// Section 3: Customer Handler
void MainWindowController::showCustomerTab() {
  stackedWidget->setCurrentIndex(CustomerIndex);
  setActionBarVisible(true);
  setActiveButton(buttonCustomer);
  QString customerQuery = R"(
    SELECT 
        id AS "ID",
        id_customer AS "Customer ID",
        full_name AS "Full Name",
        phone_number AS "Phone Number",
        CASE Type
            WHEN -1 THEN 'Temporary'
            WHEN 0 THEN 'Newbie'
            WHEN 1 THEN 'Silver'
            WHEN 2 THEN 'Gold'
            WHEN 3 THEN 'Platinum'
            ELSE 'Unknown'
        END AS "Rank",
        id_room AS "Room ID",
        Point AS "Points"
    FROM Customer
)";

  Backend::loadTableData(tableCustomer, customerQuery);
  btnAdd->setVisible(false);
  btnUpdate->setVisible(true);
  btnDelete->setVisible(false);
  btnFilter->setVisible(true);
  btnAdd->disconnect();
  btnUpdate->disconnect();
  btnDelete->disconnect();
  btnFilter->disconnect();
  connect(btnFilter, &QPushButton::clicked, this,
          &MainWindowController::showFilterCustomerDialog);
  connect(btnUpdate, &QPushButton::clicked, this,
          &MainWindowController::showUpdateCustomerDialog);
}

void MainWindowController::showUpdateCustomerDialog() {
  int currentRow = tableCustomer->currentRow();
  if (currentRow < 0) {
    QMessageBox::warning(this, "Selection Required", "Please select a customer to update.");
    return;
  }

  QString customerId = tableCustomer->item(currentRow, 1)->text(); // "Customer ID"
  QString fullName = tableCustomer->item(currentRow, 2)->text(); // "Full Name"
  QString phone = tableCustomer->item(currentRow, 3)->text(); // "Phone Number"

  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Update Customer Phone");
  dialog->setFixedSize(400, 250);

  dialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

  QVBoxLayout *layout = new QVBoxLayout(dialog);
  
  QLabel *titleLabel = new QLabel("Update Phone Number", dialog);
  titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #3730a3; margin-bottom: 10px;");
  layout->addWidget(titleLabel, 0, Qt::AlignCenter);

  QFormLayout *formLayout = new QFormLayout();
  
  QLineEdit *txtId = new QLineEdit(customerId, dialog);
  txtId->setReadOnly(true);
  txtId->setStyleSheet("background-color: #f1f5f9; border: 2px solid #cbd5e1; border-radius: 8px; padding: 8px; color: #475569;");

  QLineEdit *txtName = new QLineEdit(fullName, dialog);
  txtName->setReadOnly(true);
  txtName->setStyleSheet("background-color: #f1f5f9; border: 2px solid #cbd5e1; border-radius: 8px; padding: 8px; color: #475569;");

  QLineEdit *txtPhone = new QLineEdit(phone, dialog);
  txtPhone->setStyleSheet("background-color: #ffffff; border: 2px solid #38bdf8; border-radius: 8px; padding: 8px; color: #0f172a;");

  formLayout->addRow("Customer ID:", txtId);
  formLayout->addRow("Full Name:", txtName);
  formLayout->addRow("Phone Number:", txtPhone);
  
  layout->addLayout(formLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 10, 0, 0);
  QPushButton *btnSave = new QPushButton("Save", dialog);
  QPushButton *btnCancel = new QPushButton("Cancel", dialog);

  btnSave->setStyleSheet("QPushButton { background: #6366f1; color: white; padding: 8px 16px; border-radius: 6px; font-weight: bold; }"
                         "QPushButton:hover { background: #4f46e5; }");
  btnCancel->setStyleSheet("background-color: #cbd5e1; color: #475569; padding: 8px 16px; border-radius: 6px; font-weight: bold;");
  
  btnSave->setCursor(Qt::PointingHandCursor);
  btnCancel->setCursor(Qt::PointingHandCursor);

  buttonLayout->addWidget(btnCancel);
  buttonLayout->addWidget(btnSave);
  layout->addLayout(buttonLayout);

  connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);
  
  connect(btnSave, &QPushButton::clicked, [=]() {
    QString newPhone = txtPhone->text().trimmed();
    if (newPhone.size() != 10) {
      QMessageBox::warning(dialog, "Input Error", "Error: Phone Number must be 10 digits long.");
      return;
    }
    for (char g : newPhone.toStdString()) {
      if (!std::isdigit(g)) {
        QMessageBox::warning(dialog, "Input Error", "Error: Phone number must contain only numbers.");
        return;
      }
    }

    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery q(db);
    q.prepare("UPDATE Customer SET phone_number = :phone WHERE id_customer = :id_cust");
    q.bindValue(":phone", newPhone);
    q.bindValue(":id_cust", customerId);
    
    if (q.exec()) {
      QMessageBox::information(dialog, "Success", "Phone number updated successfully.");
      dialog->accept();
      showCustomerTab();
    } else {
      QMessageBox::critical(dialog, "Database Error", "Failed to update phone number.");
    }
  });

  dialog->exec();
}

void MainWindowController::showFilterCustomerDialog() {
  QDialog *filterDialog = new QDialog(this);
  filterDialog->setWindowTitle("Filter Customers");
  filterDialog->setFixedSize(400, 320);

  filterDialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

  QVBoxLayout *layout = new QVBoxLayout(filterDialog);
  layout->setContentsMargins(30, 25, 30, 25);

  QLabel *titleLabel = new QLabel("Filter Customer Records", filterDialog);
  titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: "
                            "#3730a3; margin-bottom: 10px;");
  layout->addWidget(titleLabel, 0, Qt::AlignCenter);

  QFormLayout *formLayout = new QFormLayout();
  formLayout->setSpacing(12);

  QString inputStyle = "QLineEdit {"
                       "   background-color: #ffffff; "
                       "   border: 2px solid #38bdf8; "
                       "   border-radius: 8px; "
                       "   padding: 8px; "
                       "   font-size: 14px; "
                       "   color: #0f172a; "
                       "}"
                       "QLineEdit:hover { border: 2px solid #0284c7; }"
                       "QLineEdit:focus { border: 2px solid #0369a1; "
                       "background-color: #f0f9ff; }";

  QLineEdit *txtName = new QLineEdit(filterDialog);
  txtName->setPlaceholderText("Filter by name...");
  txtName->setStyleSheet(inputStyle);

  QLineEdit *txtPhone = new QLineEdit(filterDialog);
  txtPhone->setPlaceholderText("Filter by phone...");
  txtPhone->setStyleSheet(inputStyle);

  QLineEdit *txtIdCard = new QLineEdit(filterDialog);
  txtIdCard->setPlaceholderText("Filter by ID...");
  txtIdCard->setStyleSheet(inputStyle);

  formLayout->addRow("Name Contains:", txtName);
  formLayout->addRow("Phone Contains:", txtPhone);
  formLayout->addRow("ID Card:", txtIdCard);

  layout->addLayout(formLayout);

  QPushButton *btnApply = new QPushButton("Apply Filter", filterDialog);
  QPushButton *btnReset = new QPushButton("Reset", filterDialog);

  btnApply->setStyleSheet(
      "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #10b981, stop:1 #059669); color: white; border: none; "
      "border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: "
      "bold; }"
      "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #059669, stop:1 #047857); }");
  btnReset->setStyleSheet(
      "QPushButton { background-color: #cbd5e1; color: #475569; border: none; "
      "border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold; }"
      "QPushButton:hover { background-color: #94a3b8; color: #1e293b; }");

  btnApply->setCursor(Qt::PointingHandCursor);
  btnReset->setCursor(Qt::PointingHandCursor);

  QHBoxLayout *btnLayout = new QHBoxLayout();
  btnLayout->setContentsMargins(0, 15, 0, 0);
  btnLayout->addWidget(btnReset);
  btnLayout->addWidget(btnApply);
  layout->addLayout(btnLayout);

  QString customerBaseQuery = R"(
    SELECT 
        id AS "ID",
        id_customer AS "Customer ID",
        full_name AS "Full Name",
        phone_number AS "Phone Number",
        CASE Type
            WHEN -1 THEN 'Temporary'
            WHEN 0 THEN 'Newbie'
            WHEN 1 THEN 'Silver'
            WHEN 2 THEN 'Gold'
            WHEN 3 THEN 'Platinum'
            ELSE 'Unknown'
        END AS "Rank",
        id_room AS "Room ID",
        Point AS "Points"
    FROM Customer
)";

  connect(btnReset, &QPushButton::clicked, [=]() {
    Backend::loadTableData(tableCustomer, customerBaseQuery);
    filterDialog->accept();
  });

  connect(btnApply, &QPushButton::clicked, [=]() {
    QString queryStr = R"(
      SELECT 
        id AS "ID",
        id_customer AS "Customer ID",
        full_name AS "Full Name",
        phone_number AS "Phone Number",
        CASE Type
            WHEN -1 THEN 'Temporary'
            WHEN 0 THEN 'Newbie'
            WHEN 1 THEN 'Silver'
            WHEN 2 THEN 'Gold'
            WHEN 3 THEN 'Platinum'
            ELSE 'Unknown'
        END AS "Rank",
        id_room AS "Room ID",
        Point AS "Points"
    FROM Customer
    )";
    if (!txtName->text().trimmed().isEmpty()) {
      queryStr +=
          QString(" AND full_name LIKE '%%1%'").arg(txtName->text().trimmed());
    }
    if (!txtPhone->text().trimmed().isEmpty()) {
      queryStr +=
          QString(" AND phone_number LIKE '%%1%'").arg(txtPhone->text().trimmed());
    }
    if (!txtIdCard->text().trimmed().isEmpty()) {
      queryStr += QString(" AND (id_customer LIKE '%%1%' OR id_card LIKE '%%1%')")
                      .arg(txtIdCard->text().trimmed());
    }

    Backend::loadTableData(tableCustomer, queryStr);
    filterDialog->accept();
  });

  filterDialog->exec();
  filterDialog->deleteLater();
}
