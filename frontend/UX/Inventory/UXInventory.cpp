#include "cores/Service/InventoryService.h"
#include "backend/Repository/ServiceItemRepository.h"
#include "backend/Manager/DatabaseManager.h"
#include "backend/Repository/InventoryRepository.h"
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
      "SELECT item_id AS 'Item ID', item_name AS 'Item Name', "
      "COALESCE(item_type, 'N/A') AS 'Category', "
      "quantity AS 'Quantity', price AS 'Price (VND)', "
      "COALESCE(minimum_quantity_required, 0) AS 'Min Required' FROM Inventory";
  Backend::loadTableData(tableInventory, inventoryQuery);

  // --- Cảnh báo mặt hàng sắp hết ---
  QSqlQuery lowStockQuery(DatabaseManager::instance().database());
  lowStockQuery.exec(
      "SELECT item_name, quantity, COALESCE(minimum_quantity_required, 0) AS min_qty "
      "FROM Inventory "
      "WHERE minimum_quantity_required IS NOT NULL "
      "  AND minimum_quantity_required > 0 "
      "  AND quantity <= minimum_quantity_required");

  QStringList lowStockItems;
  while (lowStockQuery.next()) {
    QString itemName = lowStockQuery.value("item_name").toString();
    int qty          = lowStockQuery.value("quantity").toInt();
    int minQty       = lowStockQuery.value("min_qty").toInt();
    lowStockItems << QString("• %1 (còn %2, tối thiểu %3)").arg(itemName).arg(qty).arg(minQty);
  }

  if (!lowStockItems.isEmpty()) {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("⚠️ Cảnh báo sắp hết hàng");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("<b style='color:#b45309;font-size:15px;'>Các mặt hàng dưới mức tối thiểu:</b>");
    msgBox.setInformativeText(lowStockItems.join("\n"));
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #fffbeb; }"
        "QLabel { color: #92400e; font-size: 14px; }"
        "QPushButton { background: #f59e0b; color: white; border: none; border-radius: 6px; "
        "              padding: 6px 18px; font-weight: bold; }"
        "QPushButton:hover { background: #d97706; }");
    msgBox.exec();
  }

  btnAdd->setVisible(true);
  btnUpdate->setVisible(true);
  btnDelete->setVisible(true);
  btnFilter->setVisible(true);
  if (btnAddToBooking) {
    btnAddToBooking->setVisible(true); 
    btnAddToBooking->disconnect();
    connect(btnAddToBooking, &QPushButton::clicked, this, &MainWindowController::AddToBookingInventoryClick);
  }

  btnAdd->disconnect();
  btnUpdate->disconnect();
  btnDelete->disconnect();
  btnFilter->disconnect();

  if (btnAddToBooking)
    btnAddToBooking->disconnect(); 

  connect(btnAdd, &QPushButton::clicked, this,
          &MainWindowController::AddInventoryClick);
  connect(btnUpdate, &QPushButton::clicked, this,
          &MainWindowController::UpdateInventoryClick);
  connect(btnDelete, &QPushButton::clicked, this,
          &MainWindowController::DeleteInventoryClick);
  connect(btnFilter, &QPushButton::clicked, this,
          &MainWindowController::FilterInventoryClick);
  connect(btnAddToBooking, &QPushButton::clicked, this,
          &MainWindowController::AddToBookingInventoryClick);
}

void MainWindowController::AddInventoryClick() {
  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Add New Inventory Item");
  dialog->setFixedSize(460, 520);
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
      "QLineEdit, QSpinBox, QComboBox {"
      "   background-color: #ffffff; "
      "   border: 2px solid #38bdf8; "
      "   border-radius: 8px; "
      "   padding: 8px 10px; "
      "   font-size: 14px; "
      "   color: #0f172a; "
      "}"
      "QLineEdit:hover, QSpinBox:hover, QComboBox:hover { border: 2px solid #0284c7; }"
      "QLineEdit:focus, QSpinBox:focus, QComboBox:focus { border: 2px solid #0369a1; "
      "background-color: #f0f9ff; }"
      "QComboBox::drop-down { border: none; width: 28px; }"
      "QComboBox::down-arrow { image: none; border-left: 5px solid transparent; "
      "                        border-right: 5px solid transparent; "
      "                        border-top: 7px solid #0284c7; margin-right: 8px; }";

  QLineEdit *txtName = new QLineEdit(dialog);
  txtName->setPlaceholderText("Item Name (e.g. Pepsi, Towel)...");
  txtName->setStyleSheet(inputStyle);

  QComboBox *cbType = new QComboBox(dialog);
  cbType->addItems({"Food", "Minibar", "Furniture", "Supply"});

  // Category dropdown (dựa trên ServiceCatalog)
  QComboBox *cbCategory = new QComboBox(dialog);
  cbCategory->addItems({"Food", "Minibar", "Furniture"});
  cbCategory->setStyleSheet(inputStyle);
  cbCategory->setCursor(Qt::PointingHandCursor);

  QSpinBox *spinQty = new QSpinBox(dialog);
  spinQty->setRange(1, 10000);
  spinQty->setValue(10);
  spinQty->setStyleSheet(inputStyle);

  QLineEdit *txtPrice = new QLineEdit(dialog);
  txtPrice->setPlaceholderText("Price (VND)...");
  txtPrice->setStyleSheet(inputStyle);

  // Minimum quantity required để trigger cảnh báo
  QSpinBox *spinMinQty = new QSpinBox(dialog);
  spinMinQty->setRange(0, 10000);
  spinMinQty->setValue(5);
  spinMinQty->setToolTip("Khi số lượng đạt mức này, hệ thống sẽ cảnh báo sắp hết hàng");
  spinMinQty->setStyleSheet(inputStyle);

  formLayout->addRow("Item Name:", txtName);
  formLayout->addRow("Item Type:", cbType);
  formLayout->addRow("Category:", cbCategory);
  formLayout->addRow("Quantity:", spinQty);
  formLayout->addRow("Price (VND):", txtPrice);
  formLayout->addRow("Min. Stock Alert:", spinMinQty);

  // Ghi chú nhỏ cho Min Stock Alert
  QLabel *hintLabel = new QLabel("💡 Cảnh báo khi số lượng ≤ mức này", dialog);
  hintLabel->setStyleSheet("font-size: 12px; color: #64748b; font-weight: normal; margin-top: 2px;");
  formLayout->addRow("", hintLabel);

  mainLayout->addLayout(formLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 15, 0, 0);
  QPushButton *btnSave   = new QPushButton("Save Item", dialog);
  QPushButton *btnCancel = new QPushButton("Cancel", dialog);

  btnSave->setStyleSheet(
      "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #6366f1, stop:1 #8b5cf6); color: white; border: none; "
      "border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold; }"
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
    QString name     = txtName->text().trimmed();
    QString type = cbType->currentText();
    QString category = cbCategory->currentText();
    int qty          = spinQty->value();
    double price     = txtPrice->text().trimmed().toDouble();
    int minQty       = spinMinQty->value();

    if (name.isEmpty() || price <= 0.0) {
      QMessageBox::warning(dialog, "Input Error",
                           "Please input a valid Name and positive Price!");
      return;
    }

    InventoryRepository repo;
    if (repo.insertItem(name, type, qty, price)) {
      QMessageBox::information(dialog, "Success", "Added new item successfully!");
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
    QMessageBox::warning(this, "Select Item", "Please select an inventory item row to update!");
    return;
  }

  QString id          = tableInventory->item(currentRow, 0)->text();
  QString currentName = tableInventory->item(currentRow, 1)->text();
  int currentQty      = tableInventory->item(currentRow, 3) ? tableInventory->item(currentRow, 3)->text().toInt() : 0;
  double currentPrice = tableInventory->item(currentRow, 4) ? tableInventory->item(currentRow, 4)->text().toDouble() : 0.0;

  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Update Inventory Item");
  dialog->setFixedSize(400, 360);
  dialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

  QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
  mainLayout->setContentsMargins(30, 25, 30, 25);

  QLabel *titleLabel = new QLabel("Update Item Info", dialog);
  titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #3730a3; margin-bottom: 10px;");
  mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

  QFormLayout *formLayout = new QFormLayout();
  formLayout->setSpacing(12);

  QString inputStyle = "QLineEdit, QSpinBox { background-color: #ffffff; border: 2px solid #38bdf8; border-radius: 8px; padding: 8px; font-size: 14px; color: #0f172a; }";

  QLabel *lblName = new QLabel(currentName, dialog);
  lblName->setStyleSheet("font-size: 15px; color: #0284c7;");

  QSpinBox *spinQty = new QSpinBox(dialog);
  spinQty->setRange(0, 10000);
  spinQty->setValue(currentQty);
  spinQty->setStyleSheet(inputStyle);

  QLineEdit *txtPrice = new QLineEdit(QString::number(currentPrice, 'f', 0), dialog);
  txtPrice->setStyleSheet(inputStyle);

  formLayout->addRow("Item Name:", lblName);
  formLayout->addRow("New Quantity:", spinQty);
  formLayout->addRow("New Price (VND):", txtPrice);
  mainLayout->addLayout(formLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 15, 0, 0);
  QPushButton *btnSave   = new QPushButton("Update", dialog);
  QPushButton *btnCancel = new QPushButton("Cancel", dialog);

  btnSave->setStyleSheet("QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366f1, stop:1 #8b5cf6); color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold; }");
  btnCancel->setStyleSheet("background-color: #cbd5e1; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

  buttonLayout->addWidget(btnCancel);
  buttonLayout->addWidget(btnSave);
  mainLayout->addLayout(buttonLayout);

  connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);
  connect(btnSave, &QPushButton::clicked, [=]() {
    InventoryRepository repo;
    // BÂY GIỜ ĐÃ SỬ DỤNG ĐÚNG HÀM updateItem CÓ 3 THAM SỐ
    if (repo.updateItem(id.toInt(), spinQty->value(), txtPrice->text().toDouble())) {
      QMessageBox::information(dialog, "Success", "Item updated successfully!");
      dialog->accept();
      showInventoryTab();
    } else {
      QMessageBox::critical(dialog, "Error", "Failed to update item!");
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

  QString id   = tableInventory->item(currentRow, 0)->text();
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
  dialog->setFixedSize(420, 360);
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

  QString inputStyle =
      "QLineEdit, QSpinBox, QComboBox {"
      "   background-color: #ffffff; "
      "   border: 2px solid #38bdf8; "
      "   border-radius: 8px; "
      "   padding: 8px; "
      "   font-size: 14px; "
      "   color: #0f172a; "
      "}"
      "QComboBox::drop-down { border: none; width: 28px; }"
      "QComboBox::down-arrow { image: none; border-left: 5px solid transparent; "
      "                        border-right: 5px solid transparent; "
      "                        border-top: 7px solid #0284c7; margin-right: 8px; }";

  QLineEdit *txtName = new QLineEdit(dialog);
  txtName->setPlaceholderText("Item name contains...");
  txtName->setStyleSheet(inputStyle);

  // Filter by category
  QComboBox *cbCategory = new QComboBox(dialog);
  cbCategory->addItems({"All", "Food", "Minibar", "Furniture"});
  cbCategory->setStyleSheet(inputStyle);
  cbCategory->setCursor(Qt::PointingHandCursor);

  QSpinBox *spinMinQty = new QSpinBox(dialog);
  spinMinQty->setRange(0, 10000);
  spinMinQty->setStyleSheet(inputStyle);

  // Checkbox filter chỉ hiển thị hàng sắp hết
  QPushButton *btnLowStock = new QPushButton("🔴 Chỉ hiện hàng sắp hết", dialog);
  btnLowStock->setStyleSheet(
      "QPushButton { background: #fef3c7; color: #92400e; border: 2px solid #f59e0b; "
      "              border-radius: 8px; padding: 8px 12px; font-size: 13px; font-weight: bold; }"
      "QPushButton:hover { background: #fde68a; }");
  btnLowStock->setCursor(Qt::PointingHandCursor);

  formLayout->addRow("Name Search:", txtName);
  formLayout->addRow("Category:", cbCategory);
  formLayout->addRow("Min Quantity ≥:", spinMinQty);
  formLayout->addRow("", btnLowStock);
  mainLayout->addLayout(formLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 15, 0, 0);
  QPushButton *btnApply = new QPushButton("Apply Filter", dialog);
  QPushButton *btnReset = new QPushButton("Reset All", dialog);

  btnApply->setStyleSheet(
      "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #10b981, stop:1 #059669); color: white; border: none; "
      "border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold; }"
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

  QString baseSelect =
      "SELECT item_id AS 'Item ID', item_name AS 'Item Name', "
      "COALESCE(item_type, 'N/A') AS 'Category', "
      "quantity AS 'Quantity', price AS 'Price (VND)', "
      "COALESCE(minimum_quantity_required, 0) AS 'Min Required' FROM Inventory";

  // Nút tắt lọc về default
  connect(btnReset, &QPushButton::clicked, [=]() {
    Backend::loadTableData(tableInventory, baseSelect);
    dialog->accept();
  });

  // Nút lọc hàng sắp hết (quantity <= minimum_quantity_required)
  connect(btnLowStock, &QPushButton::clicked, [=]() {
    QString queryStr = baseSelect +
        " WHERE minimum_quantity_required IS NOT NULL "
        " AND minimum_quantity_required > 0 "
        " AND quantity <= minimum_quantity_required";
    Backend::loadTableData(tableInventory, queryStr);
    dialog->accept();
  });

  connect(btnApply, &QPushButton::clicked, [=]() {
    QString queryStr = baseSelect + " WHERE 1=1";

    if (!txtName->text().trimmed().isEmpty()) {
      queryStr +=
          QString(" AND item_name LIKE '%%1%'").arg(txtName->text().trimmed());
    }

    QString selectedCat = cbCategory->currentText();
    if (selectedCat != "All") {
      queryStr += QString(" AND item_type = '%1'").arg(selectedCat);
    }

    if (spinMinQty->value() > 0) {
      queryStr += QString(" AND quantity >= %1").arg(spinMinQty->value());
    }

    Backend::loadTableData(tableInventory, queryStr);
    dialog->accept();
  });

  dialog->exec();
  dialog->deleteLater();
}

void MainWindowController::AddToBookingInventoryClick()
{
    int currentRow = tableInventory->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Select Item", "Please select an item from Inventory first!");
        return;
    }

    QString itemId = tableInventory->item(currentRow, 0)->text();
    QString itemName = tableInventory->item(currentRow, 1)->text();
    QString itemType = tableInventory->item(currentRow, 2)->text();
    double itemPrice = tableInventory->item(currentRow, 4)->text().toDouble();

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Add " + itemName + " to Booking");
    dialog->setFixedSize(450, 350);
    // FIX LỖI Ô ĐEN TẠI ĐÂY
    dialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; background: transparent; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("Add " + itemName + " to Booking", dialog);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #3730a3; margin-bottom: 15px;");
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(12);

    QString inputStyle = "QLineEdit, QSpinBox, QComboBox {"
                       "   background-color: #ffffff; border: 2px solid #38bdf8; "
                       "   border-radius: 8px; padding: 4px 10px; font-size: 14px; "
                       "   color: #0f172a; min-height: 25px; }";

    // CHUYỂN TỪ NHẬP ID TAY SANG DROPDOWN CHỌN BOOKING ĐỂ KHÔNG BỊ LỖI
    QComboBox *cbBookingId = new QComboBox(dialog);
    cbBookingId->setStyleSheet(inputStyle);
    
    QSqlQuery bQuery(DatabaseManager::instance().database());
    if (bQuery.exec("SELECT id, room_number, status FROM Bookings WHERE status IS NULL OR status <> 'CHECKED_OUT' ORDER BY id DESC")) {
      while (bQuery.next()) {
        int bId = bQuery.value("id").toInt();
        QString rm = bQuery.value("room_number").toString();
        cbBookingId->addItem(QString("Booking #%1 — Room %2").arg(bId).arg(rm.isEmpty() ? "-" : rm), bId);
      }
    }

    QSpinBox *spinQty = new QSpinBox(dialog);
    spinQty->setMinimum(1);
    spinQty->setMaximum(100);
    spinQty->setStyleSheet(inputStyle);

    QLineEdit *txtNote = new QLineEdit(dialog);
    txtNote->setStyleSheet(inputStyle);

    formLayout->addRow("Select Booking:", cbBookingId);
    formLayout->addRow("Quantity:", spinQty);
    formLayout->addRow("Customer Note:", txtNote);
    mainLayout->addLayout(formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnSave = new QPushButton("Confirm Add", dialog);
    QPushButton *btnCancel = new QPushButton("Cancel", dialog);
    
    btnSave->setStyleSheet("QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #10b981, stop:1 #059669); color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold; }");
    btnCancel->setStyleSheet("background-color: #cbd5e1; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);
    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);
    connect(btnSave, &QPushButton::clicked, [=]() {
        if (!cbBookingId->currentData().isValid()) {
            QMessageBox::warning(dialog, "Error", "No active bookings available!");
            return;
        }
        
        int bookingId = cbBookingId->currentData().toInt();
        int qty = spinQty->value();

        InventoryService invService;
        if (!invService.reserveItem(itemName, qty)) {
            QMessageBox::critical(dialog, "Out of Stock", "Not enough quantity in inventory!");
            return;
        }

        ServiceItemRepository serviceRepo;
        BookingServiceItemData itemData;
        itemData.bookingId = bookingId;
        itemData.itemId = itemId.toStdString();
        itemData.quantity = qty;
        itemData.customerNote = txtNote->text().toStdString();
        itemData.finalPrice = itemPrice * qty; 
        
        if (serviceRepo.addBookingServiceItem(itemData) != -1) {
            QMessageBox::information(dialog, "Success", "Added to Booking successfully and deducted from Inventory!");
            dialog->accept();
            showInventoryTab(); 
        } else {
            invService.releaseItem(itemName, qty); // Rollback nếu lỗi DB
            QMessageBox::critical(dialog, "Error", "Failed to add to booking bill!");
        }
    });

    dialog->exec();
    dialog->deleteLater();
}