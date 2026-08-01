#include "backend/Manager/DatabaseManager.h"
#include "backend/Repository/FoodRepository.h"
#include "frontend/UI/UI.h"
#include "frontend/UX/UX.h"
#include "frontend/usercheck/backend.h"
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlQuery>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

// Section 4: Food Options Handler
void MainWindowController::showFoodTab() {
  stackedWidget->setCurrentIndex(ServiceIndex);
  servicePage->setSection(1);
  setActionBarVisible(true);
  setActiveButton(buttonService);

  QString foodOptionsQuery = R"(
    SELECT 
        option_id AS "Option ID",
        parent_item_id AS "Parent Item ID",
        option_name AS "Option Name",
        extra_price AS "Extra Price"
    FROM FoodOptions
)";

  Backend::loadTableData(tableFood, foodOptionsQuery);

  btnAdd->setVisible(true);
  btnUpdate->setVisible(true);
  btnDelete->setVisible(true);
  btnFilter->setVisible(true);
  btnAddToBooking->setVisible(true);

  btnAdd->disconnect();
  btnUpdate->disconnect();
  btnDelete->disconnect();
  btnFilter->disconnect();
  btnAddToBooking->disconnect();

  connect(btnAdd, &QPushButton::clicked, this,
          &MainWindowController::showAddFoodDialog);
  connect(btnUpdate, &QPushButton::clicked, this,
          &MainWindowController::showUpdateFoodDialog);
  connect(btnDelete, &QPushButton::clicked, this,
          &MainWindowController::showDeleteFoodDialog);
  connect(btnFilter, &QPushButton::clicked, this,
          &MainWindowController::showFilterFoodDialog);
  connect(btnAddToBooking, &QPushButton::clicked, this,
          &MainWindowController::showAddFoodToBookingDialog);
}

void MainWindowController::showAddFoodDialog() {
  QDialog *addDialog = new QDialog(this);
  addDialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");
  addDialog->setWindowTitle("Add Food");
  addDialog->setFixedSize(400, 480);

  QVBoxLayout *mainLayout = new QVBoxLayout(addDialog);
  mainLayout->setContentsMargins(30, 30, 30, 30);

  QLabel *titleLabel = new QLabel("Food's Information", addDialog);
  titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: "
                            "#3730a3; margin-bottom: 20px;");
  mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

  QFormLayout *formLayout = new QFormLayout();
  formLayout->setSpacing(15);

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

  QLineEdit *txtId = new QLineEdit(addDialog);
  txtId->setPlaceholderText("Food ID (e.g. F001)...");
  txtId->setStyleSheet(inputStyle);

  QLineEdit *txtName = new QLineEdit(addDialog);
  txtName->setPlaceholderText("Food Name...");
  txtName->setStyleSheet(inputStyle);

  QLineEdit *txtCategory = new QLineEdit(addDialog);
  txtCategory->setPlaceholderText("Category...");
  txtCategory->setStyleSheet(inputStyle);

  QLineEdit *txtPrice = new QLineEdit(addDialog);
  txtPrice->setPlaceholderText("Base Price...");
  txtPrice->setStyleSheet(inputStyle);

  formLayout->addRow(new QLabel("Food ID:", addDialog), txtId);
  formLayout->addRow(new QLabel("Food Name:", addDialog), txtName);
  formLayout->addRow(new QLabel("Category:", addDialog), txtCategory);
  formLayout->addRow(new QLabel("Base Price:", addDialog), txtPrice);

  mainLayout->addLayout(formLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 15, 0, 0);
  QPushButton *btnSave = new QPushButton("Save", addDialog);
  QPushButton *btnCancel = new QPushButton("Cancel", addDialog);

  btnSave->setStyleSheet(
      "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366f1, "
      "stop:1 #8b5cf6); color: white; border: none; border-radius: 8px; "
      "padding: 10px 0; font-size: 15px; font-weight: bold;");
  btnCancel->setStyleSheet(
      "background-color: #cbd5e1; color: #475569; border: none; border-radius: "
      "8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

  buttonLayout->addWidget(btnCancel);
  buttonLayout->addWidget(btnSave);
  mainLayout->addLayout(buttonLayout);

  connect(btnCancel, &QPushButton::clicked, addDialog, &QDialog::reject);

  connect(btnSave, &QPushButton::clicked, [=]() {
    QString id = txtId->text().trimmed();
    QString name = txtName->text().trimmed();
    QString category = txtCategory->text().trimmed();
    double price = txtPrice->text().toDouble();

    if (id.isEmpty() || name.isEmpty() || price <= 0) {
      QMessageBox::warning(addDialog, "Error",
                           "Please input valid ID, Name and Price!");
      return;
    }

    FoodRepository repo(id, name, category, price);
    if (repo.add()) {
      QMessageBox::information(addDialog, "Success",
                               "Added new food item successfully!");
      addDialog->accept();
      showFoodTab();
    } else {
      QMessageBox::critical(addDialog, "Error", "Cannot save to database!");
    }
  });

  addDialog->exec();
  addDialog->deleteLater();
}

void MainWindowController::showUpdateFoodDialog() {
  int currentRow = tableFood->currentRow();
  if (currentRow < 0) {
    QMessageBox::warning(this, "Select Food Item",
                         "Please select a food item row to update!");
    return;
  }

  QString currentId = tableFood->item(currentRow, 0)->text();
  QString currentCategory = tableFood->item(currentRow, 1)->text();
  QString currentName = tableFood->item(currentRow, 2)->text();
  QString currentPrice = tableFood->item(currentRow, 3)->text();

  QDialog *updateDialog = new QDialog(this);
  updateDialog->setWindowTitle("Update Food Info");
  updateDialog->setFixedSize(420, 480);

  updateDialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

  QVBoxLayout *mainLayout = new QVBoxLayout(updateDialog);
  mainLayout->setContentsMargins(30, 30, 30, 30);

  QLabel *titleLabel = new QLabel("Update Food Info", updateDialog);
  titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: "
                            "#3730a3; margin-bottom: 15px;");
  mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

  QFormLayout *formLayout = new QFormLayout();
  formLayout->setSpacing(12);

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

  QLineEdit *txtId = new QLineEdit(currentId, updateDialog);
  txtId->setReadOnly(true);
  txtId->setStyleSheet(inputStyle + "QLineEdit { background-color: #e2e8f0; }");

  QLineEdit *txtCategory = new QLineEdit(currentCategory, updateDialog);
  txtCategory->setStyleSheet(inputStyle);

  QLineEdit *txtName = new QLineEdit(currentName, updateDialog);
  txtName->setStyleSheet(inputStyle);

  QLineEdit *txtPrice = new QLineEdit(currentPrice, updateDialog);
  txtPrice->setStyleSheet(inputStyle);

  formLayout->addRow("Food ID:", txtId);
  formLayout->addRow("Parent ID:", txtCategory);
  formLayout->addRow("Food Name:", txtName);
  formLayout->addRow("Extra Price ($):", txtPrice);

  mainLayout->addLayout(formLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 15, 0, 0);
  QPushButton *btnSave = new QPushButton("Save Changes", updateDialog);
  QPushButton *btnCancel = new QPushButton("Cancel", updateDialog);

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

  connect(btnCancel, &QPushButton::clicked, updateDialog, &QDialog::reject);

  connect(btnSave, &QPushButton::clicked, [=]() {
    QString foodId = txtId->text().trimmed();
    QString category = txtCategory->text().trimmed();
    QString name = txtName->text().trimmed();
    double price = txtPrice->text().trimmed().toDouble();

    if (name.isEmpty() || price <= 0.0) {
      QMessageBox::warning(updateDialog, "Error",
                           "Please enter valid Food Name and Extra Price!");
      return;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE FoodOptions SET parent_item_id = :cat, option_name = "
                  ":name, extra_price = :price WHERE option_id = :id");
    query.bindValue(":cat", category);
    query.bindValue(":name", name);
    query.bindValue(":price", price);
    query.bindValue(":id", foodId);

    if (query.exec()) {
      QMessageBox::information(updateDialog, "Success",
                               "Food information updated successfully!");
      updateDialog->accept();
      showFoodTab();
    } else {
      QMessageBox::critical(updateDialog, "Error",
                            "Failed to update food information in database!");
    }
  });

  updateDialog->exec();
  updateDialog->deleteLater();
}

void MainWindowController::showAddFoodToBookingDialog() {
  int currentRow = tableFood->currentRow();
  if (currentRow < 0) {
    QMessageBox::warning(this, "Select Food Item",
                         "Please select a food item row to add to a booking!");
    return;
  }

  QString currentId = tableFood->item(currentRow, 0)->text();
  QString currentCategory = tableFood->item(currentRow, 1)->text();
  QString currentName = tableFood->item(currentRow, 2)->text();
  QString currentPrice = tableFood->item(currentRow, 3)->text();

  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Add Food to Booking");
  dialog->setFixedSize(450, 480);

  dialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

  QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
  mainLayout->setContentsMargins(30, 30, 30, 30);

  QLabel *titleLabel = new QLabel("Add Food to Booking", dialog);
  titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: "
                            "#3730a3; margin-bottom: 15px;");
  mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

  QFormLayout *formLayout = new QFormLayout();
  formLayout->setSpacing(12);

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

  QLineEdit *txtId = new QLineEdit(currentId, dialog);
  txtId->setReadOnly(true);
  txtId->setStyleSheet(inputStyle + "QLineEdit { background-color: #e2e8f0; }");

  QLineEdit *txtName = new QLineEdit(currentName, dialog);
  txtName->setReadOnly(true);
  txtName->setStyleSheet(inputStyle +
                         "QLineEdit { background-color: #e2e8f0; }");

  QLineEdit *txtPrice = new QLineEdit(currentPrice, dialog);
  txtPrice->setReadOnly(true);
  txtPrice->setStyleSheet(inputStyle +
                          "QLineEdit { background-color: #e2e8f0; }");

  QComboBox *cbBookingId = new QComboBox(dialog);
  cbBookingId->setEditable(true);
  cbBookingId->setStyleSheet("QComboBox {"
                             "   background-color: #ffffff; "
                             "   border: 2px solid #38bdf8; "
                             "   border-radius: 8px; "
                             "   padding: 8px; "
                             "   font-size: 14px; "
                             "   color: #0f172a; "
                             "}");

  QSqlQuery bQuery(DatabaseManager::instance().database());
  if (bQuery.exec("SELECT id, room_number, status FROM Bookings "
                  "WHERE status IS NULL OR status <> 'CHECKED_OUT' "
                  "ORDER BY id DESC")) {
    while (bQuery.next()) {
      int bId = bQuery.value("id").toInt();
      QString rm = bQuery.value("room_number").toString();
      QString st = bQuery.value("status").toString();
      cbBookingId->addItem(
          QString("Booking #%1 — Room %2 (%3)")
              .arg(bId)
              .arg(rm.isEmpty() ? "-" : rm, st.isEmpty() ? "UNCONFIRMED" : st),
          bId);
    }
  }

  QLineEdit *txtQuantity = new QLineEdit("1", dialog);
  txtQuantity->setPlaceholderText("Quantity (e.g. 1, 2)...");
  txtQuantity->setStyleSheet(inputStyle);

  formLayout->addRow("Food ID:", txtId);
  formLayout->addRow("Food Name:", txtName);
  formLayout->addRow("Price ($):", txtPrice);
  formLayout->addRow("Select Booking:", cbBookingId);
  formLayout->addRow("Quantity:", txtQuantity);

  mainLayout->addLayout(formLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 15, 0, 0);
  QPushButton *btnSave = new QPushButton("Add to Booking", dialog);
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
    QString foodId = txtId->text().trimmed();
    QString bookingId = "";
    if (cbBookingId->currentData().isValid()) {
      bookingId = cbBookingId->currentData().toString();
    } else {
      bookingId = cbBookingId->currentText().trimmed();
    }

    if (bookingId.isEmpty()) {
      QMessageBox::warning(dialog, "Error",
                           "Please select or enter a valid Booking ID!");
      return;
    }

    bool okQty = false;
    int qtyVal = txtQuantity->text().trimmed().toInt(&okQty);
    if (!okQty || qtyVal <= 0) {
      QMessageBox::warning(dialog, "Error",
                           "Quantity must be a valid positive number!");
      return;
    }

    FoodRepository repo;
    if (repo.update(bookingId, foodId, txtQuantity->text().trimmed())) {
      QMessageBox::information(
          dialog, "Success",
          QString("Food item '%1' added to Booking #%2 successfully!")
              .arg(currentName, bookingId));
      dialog->accept();
      showFoodTab();
    } else {
      QMessageBox::critical(
          dialog, "Error",
          "Failed to add food to booking! Please verify Booking ID.");
    }
  });

  dialog->exec();
  dialog->deleteLater();
}

void MainWindowController::showDeleteFoodDialog() {
  int currentRow = tableFood->currentRow();
  if (currentRow < 0) {
    QMessageBox::warning(this, "Select Food Item",
                         "Please select a food option row to delete!");
    return;
  }

  QString foodId = tableFood->item(currentRow, 0)->text();

  QMessageBox::StandardButton reply = QMessageBox::question(
      this, "Confirm Deletion",
      QString("Are you sure you want to delete food option '%1'?").arg(foodId),
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    FoodRepository repo;
    if (repo.delete_f(foodId)) {
      QMessageBox::information(this, "Deleted",
                               "Food option deleted successfully!");
      showFoodTab();
    } else {
      QMessageBox::critical(this, "Error",
                            "Failed to delete food option from database.");
    }
  }
}

void MainWindowController::showFilterFoodDialog() {
  QDialog *filterDialog = new QDialog(this);
  filterDialog->setWindowTitle("Filter Food Options");
  filterDialog->setFixedSize(400, 280);

  filterDialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

  QVBoxLayout *mainLayout = new QVBoxLayout(filterDialog);
  mainLayout->setContentsMargins(30, 25, 30, 25);

  QLabel *titleLabel = new QLabel("Filter Food by Price", filterDialog);
  titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: "
                            "#3730a3; margin-bottom: 10px;");
  mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

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

  QLineEdit *txtMinPrice = new QLineEdit(filterDialog);
  txtMinPrice->setPlaceholderText("Min Price ($)...");
  txtMinPrice->setStyleSheet(inputStyle);

  QLineEdit *txtMaxPrice = new QLineEdit(filterDialog);
  txtMaxPrice->setPlaceholderText("Max Price ($)...");
  txtMaxPrice->setStyleSheet(inputStyle);

  formLayout->addRow("Min Price:", txtMinPrice);
  formLayout->addRow("Max Price:", txtMaxPrice);

  mainLayout->addLayout(formLayout);

  QHBoxLayout *btnLayout = new QHBoxLayout();
  btnLayout->setContentsMargins(0, 15, 0, 0);

  QPushButton *btnApply = new QPushButton("Filter Price", filterDialog);
  QPushButton *btnReset = new QPushButton("Reset All", filterDialog);

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

  btnApply->setCursor(Qt::PointingHandCursor);
  btnReset->setCursor(Qt::PointingHandCursor);

  btnLayout->addWidget(btnReset);
  btnLayout->addWidget(btnApply);
  mainLayout->addLayout(btnLayout);

  QString foodBaseQuery = R"(
    SELECT 
        option_id AS "Option ID",
        parent_item_id AS "Parent Item ID",
        option_name AS "Option Name",
        extra_price AS "Extra Price"
    FROM FoodOptions
)";

  connect(btnReset, &QPushButton::clicked, [=]() {
    Backend::loadTableData(tableFood, foodBaseQuery);
    filterDialog->accept();
  });

  connect(btnApply, &QPushButton::clicked, [=]() {
    double minP = txtMinPrice->text().toDouble();
    double maxP = txtMaxPrice->text().toDouble();

    QString queryStr = QString(R"(
      SELECT 
          option_id AS "Option ID",
          parent_item_id AS "Parent Item ID",
          option_name AS "Option Name",
          extra_price AS "Extra Price"
      FROM FoodOptions
      WHERE extra_price BETWEEN %1 AND %2
      ORDER BY extra_price ASC
    )")
                           .arg(minP)
                           .arg(maxP);

    Backend::loadTableData(tableFood, queryStr);

    if (tableFood->rowCount() == 0) {
      QMessageBox::information(filterDialog, "Filter Results",
                               "No food items found in this price range.");
    } else {
      QMessageBox::information(
          filterDialog, "Filter Results",
          QString("Found %1 food items matching your range.")
              .arg(tableFood->rowCount()));
      filterDialog->accept();
    }
  });

  filterDialog->exec();
  filterDialog->deleteLater();
}
