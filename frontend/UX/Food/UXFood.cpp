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
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

// Section 4: Food Options Handler
void MainWindowController::handleLogin_4() {
  stackedWidget->setCurrentIndex(ServiceIndex);
  servicePage->setSection(0);
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

  btnAdd->disconnect();
  btnUpdate->disconnect();
  btnDelete->disconnect();
  btnFilter->disconnect();

  connect(btnAdd, &QPushButton::clicked, this,
          &MainWindowController::AddNewFoodClick);
  connect(btnUpdate, &QPushButton::clicked, this,
          &MainWindowController::UpdateFoodClick);
  connect(btnDelete, &QPushButton::clicked, this,
          &MainWindowController::DeleteFoodClick);
  connect(btnFilter, &QPushButton::clicked, this,
          &MainWindowController::FilterFoodClick);
}

void MainWindowController::AddNewFoodClick() {
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
      handleLogin_4();
    } else {
      QMessageBox::critical(addDialog, "Error", "Cannot save to database!");
    }
  });

  addDialog->exec();
  addDialog->deleteLater();
}

void MainWindowController::UpdateFoodClick() {
  int currentRow = tableFood->currentRow();
  if (currentRow < 0) {
    QMessageBox::warning(this, "Select Food Item",
                         "Please select a food item row to update!");
    return;
  }

  QString currentId = tableFood->item(currentRow, 0)->text();
  QString currentName = tableFood->item(currentRow, 1)->text();
  QString currentCategory = tableFood->item(currentRow, 2)->text();
  QString currentPrice = tableFood->item(currentRow, 3)->text();

  QDialog *updateDialog = new QDialog(this);
  updateDialog->setWindowTitle("Update Food Details");
  updateDialog->setFixedSize(400, 380);

  QVBoxLayout *mainLayout = new QVBoxLayout(updateDialog);
  QFormLayout *formLayout = new QFormLayout();

  QString inputStyle = "QLineEdit { border: 1px solid #cbd5e1; border-radius: "
                       "4px; padding: 6px; }";

  QLineEdit *txtId = new QLineEdit(currentId, updateDialog);
  txtId->setReadOnly(true);
  txtId->setStyleSheet(inputStyle + "QLineEdit { background-color: #e2e8f0; }");

  QLineEdit *txtBookingId = new QLineEdit(updateDialog);
  txtBookingId->setPlaceholderText("Booking ID...");
  txtBookingId->setStyleSheet(inputStyle);

  QLineEdit *txtCategory = new QLineEdit(currentCategory, updateDialog);
  txtCategory->setStyleSheet(inputStyle);

  formLayout->addRow("Food ID:", txtId);
  formLayout->addRow("Booking ID:", txtBookingId);
  formLayout->addRow("Category:", txtCategory);

  mainLayout->addLayout(formLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  QPushButton *btnSave = new QPushButton("Update", updateDialog);
  QPushButton *btnCancel = new QPushButton("Cancel", updateDialog);

  buttonLayout->addWidget(btnCancel);
  buttonLayout->addWidget(btnSave);
  mainLayout->addLayout(buttonLayout);

  connect(btnCancel, &QPushButton::clicked, updateDialog, &QDialog::reject);

  connect(btnSave, &QPushButton::clicked, [=]() {
    QString foodId = txtId->text().trimmed();
    QString bookingId = txtBookingId->text().trimmed();
    QString category = txtCategory->text().trimmed();

    FoodRepository repo;
    if (repo.update(bookingId, foodId, category)) {
      QMessageBox::information(updateDialog, "Success",
                               "Food item updated successfully!");
      updateDialog->accept();
      handleLogin_4();
    } else {
      QMessageBox::critical(updateDialog, "Error",
                            "Failed to update food item!");
    }
  });

  updateDialog->exec();
  updateDialog->deleteLater();
}

void MainWindowController::DeleteFoodClick() {
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
      handleLogin_4();
    } else {
      QMessageBox::critical(this, "Error",
                            "Failed to delete food option from database.");
    }
  }
}

void MainWindowController::FilterFoodClick() {
  QDialog *filterDialog = new QDialog(this);
  filterDialog->setWindowTitle("Filter Food Options");
  filterDialog->setFixedSize(380, 240);

  QVBoxLayout *mainLayout = new QVBoxLayout(filterDialog);
  QFormLayout *formLayout = new QFormLayout();

  QString inputStyle = "QLineEdit { border: 1px solid #cbd5e1; border-radius: "
                       "4px; padding: 6px; }";

  QLineEdit *txtMinPrice = new QLineEdit(filterDialog);
  txtMinPrice->setPlaceholderText("Min Price ($)");
  txtMinPrice->setStyleSheet(inputStyle);

  QLineEdit *txtMaxPrice = new QLineEdit(filterDialog);
  txtMaxPrice->setPlaceholderText("Max Price ($)");
  txtMaxPrice->setStyleSheet(inputStyle);

  formLayout->addRow("Min Price:", txtMinPrice);
  formLayout->addRow("Max Price:", txtMaxPrice);

  mainLayout->addLayout(formLayout);

  QHBoxLayout *btnLayout = new QHBoxLayout();
  QPushButton *btnApply = new QPushButton("Filter Price", filterDialog);
  QPushButton *btnReset = new QPushButton("Reset All", filterDialog);

  btnLayout->addWidget(btnReset);
  btnLayout->addWidget(btnApply);
  mainLayout->addLayout(btnLayout);

  connect(btnReset, &QPushButton::clicked, [=]() {
    Backend::loadTableData(tableFood, "SELECT * FROM FoodOptions");
    filterDialog->accept();
  });

  connect(btnApply, &QPushButton::clicked, [=]() {
    double minP = txtMinPrice->text().toDouble();
    double maxP = txtMaxPrice->text().toDouble();

    FoodRepository repo;
    QVector<Food> filteredList = repo.filter(minP, maxP);

    if (filteredList.isEmpty()) {
      QMessageBox::information(filterDialog, "Filter Results",
                               "No food items found in this price range.");
    } else {
      tableFood->setRowCount(0);
      int row = 0;
      for (const auto &item : filteredList) {
        tableFood->insertRow(row);
        tableFood->setItem(row, 0, new QTableWidgetItem(item.id));
        tableFood->setItem(row, 1, new QTableWidgetItem(item.name));
        tableFood->setItem(row, 2, new QTableWidgetItem(item.category));
        tableFood->setItem(
            row, 3,
            new QTableWidgetItem(QString::number(item.basePrice, 'f', 2)));
        row++;
      }

      QMessageBox::information(
          filterDialog, "Filter Results",
          QString("Found %1 food items matching your range.")
              .arg(filteredList.size()));
      filterDialog->accept();
    }
  });

  filterDialog->exec();
  filterDialog->deleteLater();
}
