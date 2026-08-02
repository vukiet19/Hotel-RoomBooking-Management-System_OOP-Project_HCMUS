#include "backend/Manager/DatabaseManager.h"
#include "backend/Repository/InventoryRepository.h"
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
#include <QSpinBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

// Section 5: Inventory Handler
void MainWindowController::showInventoryTab() {
  stackedWidget->setCurrentIndex(InventoryIndex);
  inventoryPage->setSection(0);
  setActionBarVisible(true);
  setActiveButton(buttonInventory);

  QString inventoryQuery =
      "SELECT item_id AS 'Item ID', item_name AS 'Item Name', quantity AS "
      "'Quantity', price AS 'Price ($)' FROM Inventory";
  Backend::loadTableData(tableInventory, inventoryQuery);

  btnAdd->setVisible(true);
  btnUpdate->setVisible(true);
  btnDelete->setVisible(true);
  btnFilter->setVisible(true);
  if (btnAddToBooking)
    btnAddToBooking->setVisible(false);

  btnAdd->disconnect();
  btnUpdate->disconnect();
  btnDelete->disconnect();
  btnFilter->disconnect();

  connect(btnAdd, &QPushButton::clicked, this,
          &MainWindowController::AddInventoryClick);
  connect(btnUpdate, &QPushButton::clicked, this,
          &MainWindowController::UpdateInventoryClick);
  connect(btnDelete, &QPushButton::clicked, this,
          &MainWindowController::DeleteInventoryClick);
  connect(btnFilter, &QPushButton::clicked, this,
          &MainWindowController::FilterInventoryClick);
}

void MainWindowController::AddInventoryClick() {
  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Add New Inventory Item");
  dialog->setFixedSize(420, 420);
  dialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

  QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
  mainLayout->setContentsMargins(30, 30, 30, 30);

  QLabel *titleLabel = new QLabel("Add Inventory Item", dialog);
  titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: "
                            "#3730a3; margin-bottom: 15px;");
  mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

  QFormLayout *formLayout = new QFormLayout();
  formLayout->setSpacing(12);

  QString inputStyle =
      "QLineEdit, QSpinBox {"
      "   background-color: #ffffff; "
      "   border: 2px solid #38bdf8; "
      "   border-radius: 8px; "
      "   padding: 10px; "
      "   font-size: 14px; "
      "   color: #0f172a; "
      "}"
      "QLineEdit:hover, QSpinBox:hover { border: 2px solid #0284c7; }"
      "QLineEdit:focus, QSpinBox:focus { border: 2px solid #0369a1; "
      "background-color: #f0f9ff; }";

  QLineEdit *txtName = new QLineEdit(dialog);
  txtName->setPlaceholderText("Item Name (e.g. Towel, Shampoo)...");
  txtName->setStyleSheet(inputStyle);

  QSpinBox *spinQty = new QSpinBox(dialog);
  spinQty->setRange(1, 10000);
  spinQty->setValue(10);
  spinQty->setStyleSheet(inputStyle);

  QLineEdit *txtPrice = new QLineEdit(dialog);
  txtPrice->setPlaceholderText("Price ($)...");
  txtPrice->setStyleSheet(inputStyle);

  formLayout->addRow("Item Name:", txtName);
  formLayout->addRow("Quantity:", spinQty);
  formLayout->addRow("Price (VND):", txtPrice);
  mainLayout->addLayout(formLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 15, 0, 0);
  QPushButton *btnSave = new QPushButton("Save Item", dialog);
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
    QString name = txtName->text().trimmed();
    int qty = spinQty->value();
    double price = txtPrice->text().trimmed().toDouble();

    if (name.isEmpty() || price <= 0.0) {
      QMessageBox::warning(dialog, "Input Error",
                           "Please input a valid Name and positive Price!");
      return;
    }

    InventoryRepository repo;
    if (repo.insertItem(name, qty, price)) {
      QMessageBox::information(dialog, "Success",
                               "Added new item to inventory successfully!");
      dialog->accept();
      showInventoryTab();
    } else {
      QMessageBox::critical(dialog, "Error", "Failed to add item to database!");
    }
  });

  dialog->exec();
  dialog->deleteLater();
}

void MainWindowController::UpdateInventoryClick() {
  int currentRow = tableInventory->currentRow();
  if (currentRow < 0) {
    QMessageBox::warning(this, "Select Item",
                         "Please select an inventory item row to update!");
    return;
  }

  QString id = tableInventory->item(currentRow, 0)->text();
  QString currentName = tableInventory->item(currentRow, 1)->text();
  int currentQty = tableInventory->item(currentRow, 2)
                       ? tableInventory->item(currentRow, 2)->text().toInt()
                       : 0;

  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Update Inventory Stock");
  dialog->setFixedSize(400, 320);
  dialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

  QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
  mainLayout->setContentsMargins(30, 25, 30, 25);

  QLabel *titleLabel = new QLabel("Update Item Stock", dialog);
  titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: "
                            "#3730a3; margin-bottom: 10px;");
  mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

  QFormLayout *formLayout = new QFormLayout();
  formLayout->setSpacing(12);

  QString inputStyle = "QLineEdit, QSpinBox {"
                       "   background-color: #ffffff; "
                       "   border: 2px solid #38bdf8; "
                       "   border-radius: 8px; "
                       "   padding: 8px; "
                       "   font-size: 14px; "
                       "   color: #0f172a; "
                       "}";

  QLabel *lblName = new QLabel(currentName, dialog);
  lblName->setStyleSheet("font-size: 15px; color: #0284c7;");

  QSpinBox *spinQty = new QSpinBox(dialog);
  spinQty->setRange(0, 10000);
  spinQty->setValue(currentQty);
  spinQty->setStyleSheet(inputStyle);

  formLayout->addRow("Item Name:", lblName);
  formLayout->addRow("New Quantity:", spinQty);
  mainLayout->addLayout(formLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 15, 0, 0);
  QPushButton *btnSave = new QPushButton("Update Stock", dialog);
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
    InventoryRepository repo;
    if (repo.updateQuantity(id.toInt(), spinQty->value())) {
      QMessageBox::information(dialog, "Success",
                               "Item stock updated successfully!");
      dialog->accept();
      showInventoryTab();
    } else {
      QMessageBox::critical(dialog, "Error",
                            "Failed to update stock quantity!");
    }
  });

  dialog->exec();
  dialog->deleteLater();
}

void MainWindowController::DeleteInventoryClick() {
  int currentRow = tableInventory->currentRow();
  if (currentRow < 0) {
    QMessageBox::warning(this, "Select Item",
                         "Please select an item row to delete!");
    return;
  }

  QString id = tableInventory->item(currentRow, 0)->text();
  QString name = tableInventory->item(currentRow, 1)->text();

  if (QMessageBox::question(
          this, "Confirm Deletion",
          QString("Are you sure you want to delete '%1' from Inventory?")
              .arg(name)) == QMessageBox::Yes) {
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM Inventory WHERE item_id = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
      QMessageBox::information(this, "Success", "Item deleted successfully!");
      showInventoryTab();
    } else {
      QMessageBox::critical(
          this, "Error", "Failed to delete item: " + query.lastError().text());
    }
  }
}

void MainWindowController::FilterInventoryClick() {
  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Filter Inventory");
  dialog->setFixedSize(380, 280);
  dialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

  QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
  mainLayout->setContentsMargins(30, 25, 30, 25);

  QLabel *titleLabel = new QLabel("Filter Inventory Items", dialog);
  titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: "
                            "#3730a3; margin-bottom: 10px;");
  mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

  QFormLayout *formLayout = new QFormLayout();
  formLayout->setSpacing(12);

  QString inputStyle = "QLineEdit, QSpinBox {"
                       "   background-color: #ffffff; "
                       "   border: 2px solid #38bdf8; "
                       "   border-radius: 8px; "
                       "   padding: 8px; "
                       "   font-size: 14px; "
                       "   color: #0f172a; "
                       "}";

  QLineEdit *txtName = new QLineEdit(dialog);
  txtName->setPlaceholderText("Item name contains...");
  txtName->setStyleSheet(inputStyle);

  QSpinBox *spinMinQty = new QSpinBox(dialog);
  spinMinQty->setRange(0, 10000);
  spinMinQty->setStyleSheet(inputStyle);

  formLayout->addRow("Name Search:", txtName);
  formLayout->addRow("Min Quantity:", spinMinQty);
  mainLayout->addLayout(formLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 15, 0, 0);
  QPushButton *btnApply = new QPushButton("Apply Filter", dialog);
  QPushButton *btnReset = new QPushButton("Reset All", dialog);

  btnApply->setStyleSheet(
      "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #10b981, stop:1 #059669); color: white; border: none; "
      "border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: "
      "bold; }"
      "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #059669, stop:1 #047857); }");
  btnReset->setStyleSheet(
      "background-color: #cbd5e1; color: #475569; border: none; border-radius: "
      "8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

  btnApply->setCursor(Qt::PointingHandCursor);
  btnReset->setCursor(Qt::PointingHandCursor);

  buttonLayout->addWidget(btnReset);
  buttonLayout->addWidget(btnApply);
  mainLayout->addLayout(buttonLayout);

  QString inventoryBaseQuery =
      "SELECT item_id AS 'Item ID', item_name AS 'Item Name', quantity AS "
      "'Quantity', price AS 'Price ($)' FROM Inventory";

  connect(btnReset, &QPushButton::clicked, [=]() {
    Backend::loadTableData(tableInventory, inventoryBaseQuery);
    dialog->accept();
  });

  connect(btnApply, &QPushButton::clicked, [=]() {
    QString queryStr =
        QString("SELECT item_id AS 'Item ID', item_name AS 'Item Name', "
                "quantity AS 'Quantity', price AS 'Price ($)' FROM Inventory "
                "WHERE quantity >= %1")
            .arg(spinMinQty->value());

    if (!txtName->text().trimmed().isEmpty()) {
      queryStr +=
          QString(" AND item_name LIKE '%%1%'").arg(txtName->text().trimmed());
    }

    Backend::loadTableData(tableInventory, queryStr);
    dialog->accept();
  });

  dialog->exec();
  dialog->deleteLater();
}
