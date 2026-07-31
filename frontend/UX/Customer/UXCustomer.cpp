#include "backend/Repository/CustomerRepository.h"
#include "frontend/UI/UI.h"
#include "frontend/UX/UX.h"
#include "frontend/usercheck/backend.h"
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
        Type AS "Rank",
        id_room AS "Room ID",
        Point AS "Points"
    FROM Customer
)";

  Backend::loadTableData(tableCustomer, customerQuery);
  btnAdd->setVisible(false);
  btnUpdate->setVisible(false);
  btnDelete->setVisible(false);
  btnFilter->setVisible(true);
  btnAdd->disconnect();
  btnUpdate->disconnect();
  btnDelete->disconnect();
  btnFilter->disconnect();
  connect(btnFilter, &QPushButton::clicked, this,
          &MainWindowController::showFilterCustomerDialog);
}

void MainWindowController::showFilterCustomerDialog() {
  QDialog *filterDialog = new QDialog(this);
  filterDialog->setWindowTitle("Filter Customers");
  filterDialog->setFixedSize(380, 280);

  QVBoxLayout *layout = new QVBoxLayout(filterDialog);
  QFormLayout *formLayout = new QFormLayout();

  QLineEdit *txtName = new QLineEdit(filterDialog);
  QLineEdit *txtPhone = new QLineEdit(filterDialog);
  QLineEdit *txtIdCard = new QLineEdit(filterDialog);

  formLayout->addRow("Name Contains:", txtName);
  formLayout->addRow("Phone Contains:", txtPhone);
  formLayout->addRow("ID Card:", txtIdCard);

  layout->addLayout(formLayout);

  QPushButton *btnApply = new QPushButton("Apply Filter", filterDialog);
  QPushButton *btnReset = new QPushButton("Reset", filterDialog);
  QHBoxLayout *btnLayout = new QHBoxLayout();
  btnLayout->addWidget(btnReset);
  btnLayout->addWidget(btnApply);
  layout->addLayout(btnLayout);

  connect(btnReset, &QPushButton::clicked, [=]() {
    Backend::loadTableData(tableCustomer, "SELECT * FROM Customer");
    filterDialog->accept();
  });

  connect(btnApply, &QPushButton::clicked, [=]() {
    QString queryStr = "SELECT * FROM Customer WHERE 1=1";
    if (!txtName->text().trimmed().isEmpty()) {
      queryStr +=
          QString(" AND name LIKE '%%1%'").arg(txtName->text().trimmed());
    }
    if (!txtPhone->text().trimmed().isEmpty()) {
      queryStr +=
          QString(" AND phone LIKE '%%1%'").arg(txtPhone->text().trimmed());
    }
    if (!txtIdCard->text().trimmed().isEmpty()) {
      queryStr += QString(" AND id_customer LIKE '%%1%'")
                      .arg(txtIdCard->text().trimmed());
    }

    Backend::loadTableData(tableCustomer, queryStr);
    filterDialog->accept();
  });

  filterDialog->exec();
  filterDialog->deleteLater();
}
