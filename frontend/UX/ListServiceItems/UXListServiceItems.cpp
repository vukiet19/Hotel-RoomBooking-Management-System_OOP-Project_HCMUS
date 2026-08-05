#include "backend/Manager/DatabaseManager.h"
#include "backend/Repository/BookingRepository.h"
#include "backend/Repository/ServiceItemRepository.h"
#include "cores/Service/InventoryService.h"
#include "frontend/UI/UI.h"
#include "frontend/UX/UX.h"
#include "frontend/usercheck/usercheck.h"

#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QtSql/QSqlQuery>

namespace {

QString inputStyle() {
  return "QLineEdit, QComboBox, QSpinBox { background-color: #ffffff; border: "
         "2px solid "
         "#38bdf8; border-radius: 8px; padding: 8px; font-size: 14px; "
         "color: #0f172a; }"
         "QLineEdit:hover, QComboBox:hover, QSpinBox:hover { border-color: "
         "#0284c7; }"
         "QLineEdit:focus, QComboBox:focus, QSpinBox:focus { border-color: "
         "#0369a1; "
         "background-color: #f0f9ff; }"
         "QLineEdit[readOnly=\"true\"] { background-color: #e2e8f0; color: "
         "#475569; border-color: #cbd5e1; }"
         "QComboBox::drop-down { border: none; width: 25px; }"
         "QComboBox QAbstractItemView { background-color: #ffffff; color: "
         "#0f172a; "
         "border: 2px solid #38bdf8; selection-background-color: #f0f9ff; "
         "selection-color: #0369a1; }";
}

QString dialogStyle() {
  return "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
         "stop:0 #f0f9ff, stop:1 #ffffff); }"
         "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }";
}

QString primaryButtonStyle() {
  return "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
         "stop:0 #6366f1, stop:1 #8b5cf6); color: white; border: none; "
         "border-radius: 8px; padding: 10px 0; font-size: 15px; "
         "font-weight: bold; }"
         "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, "
         "y2:0, stop:0 #4f46e5, stop:1 #7c3aed); }";
}

QString filterButtonStyle() {
  return "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
         "stop:0 #10b981, stop:1 #059669); color: white; border: none; "
         "border-radius: 8px; padding: 10px 0; font-size: 15px; "
         "font-weight: bold; }"
         "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, "
         "y2:0, stop:0 #059669, stop:1 #047857); }";
}

QString secondaryButtonStyle() {
  return "QPushButton { background-color: #cbd5e1; color: #475569; border: "
         "none; border-radius: 8px; padding: 10px 0; font-size: 15px; "
         "font-weight: bold; }"
         "QPushButton:hover { background-color: #94a3b8; color: #1e293b; }";
}

void addDialogHeading(QVBoxLayout *layout, QWidget *parent,
                      const QString &title) {
  auto *heading = new QLabel(title, parent);
  heading->setStyleSheet("font-size: 20px; font-weight: bold; color: #3730a3; "
                         "margin-bottom: 15px;");
  layout->addWidget(heading, 0, Qt::AlignCenter);
}

bool readNonNegativePrice(QLineEdit *input, double &price, QWidget *parent,
                          const QString &fieldName, bool optional = false) {
  const QString value = input->text().trimmed();
  if (optional && value.isEmpty()) {
    price = -1.0;
    return true;
  }

  bool ok = false;
  price = value.toDouble(&ok);
  if (!ok || price < 0.0) {
    QMessageBox::warning(parent, "Invalid input",
                         fieldName + " must be a non-negative number.");
    return false;
  }
  return true;
}

bool loadActiveBookings(QComboBox *comboBox) {
  QSqlQuery query(DatabaseManager::instance().database());
  if (!query.exec("SELECT id, room_number, status FROM Bookings "
                  "WHERE status IN ('CONFIRMED', 'CHECKED_IN') "
                  "ORDER BY id DESC")) {
    return false;
  }

  while (query.next()) {
    const int bookingId = query.value("id").toInt();
    const QString roomNumber = query.value("room_number").toString();
    const QString status = query.value("status").toString();
    comboBox->addItem(QString("Booking #%1 — Room %2 (%3)")
                          .arg(bookingId)
                          .arg(roomNumber.isEmpty() ? "-" : roomNumber,
                               status.isEmpty() ? "CONFIRMED" : status),
                      bookingId);
  }
  return comboBox->count() > 0;
}

bool isBookingActive(int bookingId) {
  QSqlQuery query(DatabaseManager::instance().database());
  query.prepare("SELECT status FROM Bookings WHERE id = :booking_id");
  query.bindValue(":booking_id", bookingId);
  if (!query.exec() || !query.next()) {
    return false;
  }
  QString st = query.value("status").toString().trimmed();
  return st == "CONFIRMED" || st == "CHECKED_IN";
}

} // namespace

void MainWindowController::showServiceTab() {
  stackedWidget->setCurrentIndex(ServiceIndex);
  setActionBarVisible(true);
  setActiveButton(buttonService);

  // Kéo dữ liệu từ Database, lấy tên cột là "Status" và chuyển 1/0 thành
  // "Available/Unavailable"
  QString serviceQuery = R"(
    SELECT 
        item_id AS "Service ID",
        item_name AS "Name",
        category AS "Category",
        base_price AS "Price",
        CASE WHEN is_active = 1 THEN 'Available' ELSE 'Unavailable' END AS "Status"
    FROM ServiceCatalog
  )";
  Backend::loadTableData(tableService, serviceQuery);

  btnAdd->setVisible(true);
  btnUpdate->setVisible(true);
  btnDelete->setVisible(true);
  if (btnAddToBooking)
    btnAddToBooking->setVisible(true);
  btnFilter->setVisible(true);

  btnAdd->disconnect();
  btnUpdate->disconnect();
  btnDelete->disconnect();
  if (btnAddToBooking)
    btnAddToBooking->disconnect();
  btnFilter->disconnect();

  connect(btnAdd, &QPushButton::clicked, this,
          &MainWindowController::showAddServiceDialog);
  connect(btnUpdate, &QPushButton::clicked, this,
          &MainWindowController::showUpdateServiceDialog);
  connect(btnDelete, &QPushButton::clicked, this,
          &MainWindowController::showDeleteServiceDialog);
  if (btnAddToBooking) {
    connect(btnAddToBooking, &QPushButton::clicked, this,
            &MainWindowController::showAddServiceToBookingDialog);
  }
  connect(btnFilter, &QPushButton::clicked, this,
          &MainWindowController::showFilterServiceDialog);
}

void MainWindowController::showAddServiceToBookingDialog() {
  if (!tableService || tableService->currentRow() < 0 ||
      !tableService->item(tableService->currentRow(), 0)) {
    QMessageBox::warning(
        this, "Select service",
        "Select a Service Catalog row before adding it to a booking.");
    return;
  }

  const QString serviceId =
      tableService->item(tableService->currentRow(), 0)->text();
  QString serviceName = "";
  QString categoryStr = "";
  double basePrice = 0.0;
  bool isActive = false;

  QSqlDatabase db = DatabaseManager::instance().database();
  QSqlQuery query(db);
  query.prepare("SELECT item_name, category, base_price, is_active FROM "
                "ServiceCatalog WHERE item_id = :id");
  query.bindValue(":id", serviceId);

  if (query.exec() && query.next()) {
    serviceName = query.value("item_name").toString();
    categoryStr = query.value("category").toString();
    basePrice = query.value("base_price").toDouble();
    isActive = query.value("is_active").toBool();
  } else {
    QMessageBox::warning(
        this, "Service not found",
        "The selected service no longer exists in the catalog.");
    showServiceTab();
    return;
  }

  // 1. NGĂN CHẶN NẾU DỊCH VỤ UNAVAILABLE
  if (!isActive) {
    QMessageBox::critical(this, "Service Unavailable",
                          "This service is currently marked as 'Unavailable' "
                          "and cannot be added to a booking.");
    return;
  }

  QDialog dialog(this);
  dialog.setWindowTitle("Add Service to Booking");
  dialog.setFixedSize(560, 560);
  dialog.setStyleSheet(dialogStyle());

  auto *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(30, 30, 30, 30);
  addDialogHeading(layout, &dialog, "Add Service to Booking");
  auto *form = new QFormLayout();
  form->setSpacing(15);

  auto *id = new QLineEdit(serviceId, &dialog);
  auto *name = new QLineEdit(serviceName, &dialog);
  auto *price = new QLineEdit(QString::number(basePrice, 'f', 2), &dialog);
  auto *booking = new QComboBox(&dialog);
  auto *quantity = new QSpinBox(&dialog);
  auto *note = new QLineEdit(&dialog);

  for (auto *input : {id, name, price, note})
    input->setStyleSheet(inputStyle());
  booking->setStyleSheet(inputStyle());
  quantity->setStyleSheet(inputStyle());

  id->setReadOnly(true);
  name->setReadOnly(true);
  price->setReadOnly(true);
  quantity->setRange(1, 9999);
  quantity->setValue(1);
  note->setPlaceholderText("Optional customer note");

  if (!loadActiveBookings(booking)) {
    QMessageBox::information(this, "No Confirmed Booking",
                             "No confirm booking");
    return;
  }

  form->addRow("Service ID:", id);
  form->addRow("Service name:", name);
  form->addRow("Final unit price:", price);
  form->addRow("Booking:", booking);

  // LOGIC ẨN/HIỆN QUANTITY THEO CATEGORY
  QLabel *qtyLabel = new QLabel("Quantity:", &dialog);
  qtyLabel->setStyleSheet(
      "color: #1e293b; font-weight: bold; font-size: 14px;");

  if (categoryStr == "Facility" || categoryStr == "Damage") {
    quantity->setVisible(false);
    qtyLabel->setVisible(false);
  }

  form->addRow(qtyLabel, quantity);
  form->addRow("Customer note:", note);
  layout->addLayout(form);

  auto *buttons = new QHBoxLayout();
  buttons->setContentsMargins(0, 15, 0, 0);
  auto *cancel = new QPushButton("Cancel", &dialog);
  auto *save = new QPushButton("Add to Booking", &dialog);
  cancel->setStyleSheet(secondaryButtonStyle());
  save->setStyleSheet(primaryButtonStyle());
  cancel->setCursor(Qt::PointingHandCursor);
  save->setCursor(Qt::PointingHandCursor);
  buttons->addWidget(cancel);
  buttons->addWidget(save);
  layout->addLayout(buttons);

  connect(cancel, &QPushButton::clicked, &dialog, &QDialog::reject);
  connect(
      save, &QPushButton::clicked, &dialog,
      [this, &dialog, serviceId, serviceName, basePrice, booking, quantity,
       note, categoryStr] {
        const int bookingId = booking->currentData().toInt();
        if (bookingId <= 0 || !isBookingActive(bookingId)) {
          QMessageBox::warning(
              &dialog, "Booking unavailable",
              "This booking no longer exists or has already checked out.");
          return;
        }

        int finalQty = (categoryStr == "Facility" || categoryStr == "Damage")
                           ? 1
                           : quantity->value();

        // RÚT HÀNG TỪ KHO INVENTORY (NẾU LÀ VẬT PHẨM)
        if (categoryStr == "Food" || categoryStr == "Minibar" ||
            categoryStr == "Furniture") {
          InventoryService invService;
          if (!invService.reserveItem(serviceName, finalQty)) {
            QMessageBox::critical(
                &dialog, "Out of Stock",
                QString("Not enough '%1' in inventory!").arg(serviceName));
            return;
          }
        }

        BookingRepository bookingRepository;
        const bool added = bookingRepository.addServiceItemToBooking(
            bookingId, serviceId.toStdString(), finalQty, basePrice * finalQty,
            note->text().trimmed().toStdString());

        if (!added) {
          if (categoryStr == "Food" || categoryStr == "Minibar" ||
              categoryStr == "Furniture") {
            InventoryService invService;
            invService.releaseItem(serviceName,
                                   finalQty); // Hoàn lại kho nếu lưu lỗi
          }
          QMessageBox::critical(
              &dialog, "Cannot add service",
              "The service could not be added to the selected booking.");
          return;
        }

        QMessageBox::information(&dialog, "Success",
                                 "Service added to the booking successfully.");
        dialog.accept();
        showBookingServicesTab();
      });

  dialog.exec();
}

void MainWindowController::showAddServiceDialog() {
  QDialog dialog(this);
  dialog.setWindowTitle("Add Service Catalog Item");
  dialog.setFixedSize(440, 430);
  dialog.setStyleSheet(dialogStyle());

  auto *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(30, 30, 30, 30);
  addDialogHeading(layout, &dialog, "Service Catalog Details");
  auto *form = new QFormLayout();
  form->setSpacing(15);
  auto *id = new QLineEdit(&dialog);
  auto *name = new QLineEdit(&dialog);

  auto *category = new QComboBox(&dialog);
  category->addItems({"Food", "Minibar", "Furniture", "Facility", "Damage"});
  category->setStyleSheet(inputStyle());

  auto *price = new QLineEdit(&dialog);

  auto *statusCombo = new QComboBox(&dialog);
  statusCombo->addItems({"Available", "Unavailable"});
  statusCombo->setStyleSheet(inputStyle());

  id->setPlaceholderText("Example: SPA001");
  name->setPlaceholderText("Service name");
  price->setPlaceholderText("0.00");
  for (auto *input : {id, name, price})
    input->setStyleSheet(inputStyle());

  form->addRow("Service ID:", id);
  form->addRow("Name:", name);
  form->addRow("Category:", category);
  form->addRow("Base price:", price);
  form->addRow("Status:", statusCombo);
  layout->addLayout(form);

  auto *buttons = new QHBoxLayout();
  buttons->setContentsMargins(0, 15, 0, 0);
  auto *cancel = new QPushButton("Cancel", &dialog);
  auto *save = new QPushButton("Save", &dialog);
  cancel->setStyleSheet(secondaryButtonStyle());
  save->setStyleSheet(primaryButtonStyle());
  cancel->setCursor(Qt::PointingHandCursor);
  save->setCursor(Qt::PointingHandCursor);
  buttons->addWidget(cancel);
  buttons->addWidget(save);
  layout->addLayout(buttons);

  connect(cancel, &QPushButton::clicked, &dialog, &QDialog::reject);
  connect(save, &QPushButton::clicked, &dialog,
          [this, &dialog, id, name, category, price, statusCombo] {
            double basePrice = 0.0;
            if (id->text().trimmed().isEmpty() ||
                name->text().trimmed().isEmpty()) {
              QMessageBox::warning(&dialog, "Invalid input",
                                   "Service ID and name are required.");
              return;
            }
            if (!readNonNegativePrice(price, basePrice, &dialog, "Base price"))
              return;

            int is_active = (statusCombo->currentText() == "Available") ? 1 : 0;

            QSqlDatabase db = DatabaseManager::instance().database();
            QSqlQuery query(db);
            query.prepare("INSERT INTO ServiceCatalog (item_id, item_name, "
                          "category, base_price, is_active) "
                          "VALUES (:id, :name, :cat, :price, :status)");
            query.bindValue(":id", id->text().trimmed());
            query.bindValue(":name", name->text().trimmed());
            query.bindValue(":cat", category->currentText());
            query.bindValue(":price", basePrice);
            query.bindValue(":status", is_active);

            if (!query.exec()) {
              QMessageBox::critical(&dialog, "Cannot add service",
                                    "The Service ID may already exist.");
              return;
            }

            dialog.accept();
            showServiceTab();
          });

  dialog.exec();
}

void MainWindowController::showUpdateServiceDialog() {
  if (!tableService || tableService->currentRow() < 0 ||
      !tableService->item(tableService->currentRow(), 0)) {
    QMessageBox::warning(this, "Select service",
                         "Select a Service Catalog row to update.");
    return;
  }

  const QString itemId =
      tableService->item(tableService->currentRow(), 0)->text();

  QSqlDatabase db = DatabaseManager::instance().database();
  QSqlQuery fetchQuery(db);
  fetchQuery.prepare("SELECT item_name, category, base_price, is_active FROM "
                     "ServiceCatalog WHERE item_id = :id");
  fetchQuery.bindValue(":id", itemId);

  if (!fetchQuery.exec() || !fetchQuery.next()) {
    QMessageBox::warning(this, "Service not found",
                         "The selected service no longer exists.");
    showServiceTab();
    return;
  }

  QString currName = fetchQuery.value("item_name").toString();
  QString currCat = fetchQuery.value("category").toString();
  double currPrice = fetchQuery.value("base_price").toDouble();
  bool currActive = fetchQuery.value("is_active").toBool();

  QDialog dialog(this);
  dialog.setWindowTitle("Update Service Catalog Item");
  dialog.setFixedSize(440, 430);
  dialog.setStyleSheet(dialogStyle());

  auto *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(30, 30, 30, 30);
  addDialogHeading(layout, &dialog, "Edit Service Catalog Item");
  auto *form = new QFormLayout();
  form->setSpacing(15);

  auto *id = new QLineEdit(itemId, &dialog);
  auto *name = new QLineEdit(currName, &dialog);

  auto *category = new QComboBox(&dialog);
  category->addItems({"Food", "Minibar", "Furniture", "Facility", "Damage"});
  category->setCurrentText(currCat);
  category->setStyleSheet(inputStyle());

  auto *price = new QLineEdit(QString::number(currPrice, 'f', 2), &dialog);

  auto *statusCombo = new QComboBox(&dialog);
  statusCombo->addItems({"Available", "Unavailable"});
  statusCombo->setCurrentText(currActive ? "Available" : "Unavailable");
  statusCombo->setStyleSheet(inputStyle());

  id->setReadOnly(true);
  for (auto *input : {id, name, price})
    input->setStyleSheet(inputStyle());

  id->setStyleSheet(inputStyle() +
                    "QLineEdit { background-color: #e2e8f0; color: #475569; }");

  form->addRow("Service ID:", id);
  form->addRow("Name:", name);
  form->addRow("Category:", category);
  form->addRow("Base price:", price);
  form->addRow("Status:", statusCombo);
  layout->addLayout(form);

  auto *buttons = new QHBoxLayout();
  buttons->setContentsMargins(0, 15, 0, 0);
  auto *cancel = new QPushButton("Cancel", &dialog);
  auto *save = new QPushButton("Update", &dialog);
  cancel->setStyleSheet(secondaryButtonStyle());
  save->setStyleSheet(primaryButtonStyle());
  cancel->setCursor(Qt::PointingHandCursor);
  save->setCursor(Qt::PointingHandCursor);
  buttons->addWidget(cancel);
  buttons->addWidget(save);
  layout->addLayout(buttons);

  connect(cancel, &QPushButton::clicked, &dialog, &QDialog::reject);
  connect(
      save, &QPushButton::clicked, &dialog,
      [this, &dialog, id, name, category, price, statusCombo] {
        double basePrice = 0.0;
        if (name->text().trimmed().isEmpty()) {
          QMessageBox::warning(&dialog, "Invalid input", "Name is required.");
          return;
        }
        if (!readNonNegativePrice(price, basePrice, &dialog, "Base price"))
          return;

        int is_active = (statusCombo->currentText() == "Available") ? 1 : 0;

        QSqlDatabase db = DatabaseManager::instance().database();
        QSqlQuery query(db);
        query.prepare(
            "UPDATE ServiceCatalog SET item_name = :name, category = :cat, "
            "base_price = :price, is_active = :status WHERE item_id = :id");
        query.bindValue(":id", id->text().trimmed());
        query.bindValue(":name", name->text().trimmed());
        query.bindValue(":cat", category->currentText());
        query.bindValue(":price", basePrice);
        query.bindValue(":status", is_active);

        if (!query.exec()) {
          QMessageBox::critical(&dialog, "Cannot update service",
                                "The selected service could not be updated.");
          return;
        }

        dialog.accept();
        showServiceTab();
      });

  dialog.exec();
}

void MainWindowController::showDeleteServiceDialog() {
  if (!tableService || tableService->currentRow() < 0 ||
      !tableService->item(tableService->currentRow(), 0)) {
    QMessageBox::warning(this, "Select service",
                         "Select a Service Catalog row to delete.");
    return;
  }

  const QString itemId =
      tableService->item(tableService->currentRow(), 0)->text();
  if (QMessageBox::question(
          this, "Delete service",
          QString("Delete Service Catalog item '%1'?").arg(itemId),
          QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
    return;
  }

  QSqlDatabase db = DatabaseManager::instance().database();
  QSqlQuery checkQuery(db);
  checkQuery.prepare(
      "SELECT 1 FROM BookingServiceItems WHERE item_id = :id LIMIT 1");
  checkQuery.bindValue(":id", itemId);
  if (checkQuery.exec() && checkQuery.next()) {
    QMessageBox::warning(this, "Cannot delete service",
                         "This service is already used by a booking and is "
                         "kept to preserve booking history.");
    return;
  }

  QSqlQuery delQuery(db);
  delQuery.prepare("DELETE FROM ServiceCatalog WHERE item_id = :id");
  delQuery.bindValue(":id", itemId);

  if (!delQuery.exec()) {
    QMessageBox::critical(this, "Cannot delete service",
                          "The service could not be deleted.");
    return;
  }

  showServiceTab();
}

void MainWindowController::showFilterServiceDialog() {
  QDialog dialog(this);
  dialog.setWindowTitle("Filter Service Catalog");
  dialog.setFixedSize(440, 480);
  dialog.setStyleSheet(dialogStyle());

  auto *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(30, 30, 30, 30);
  addDialogHeading(layout, &dialog, "Filter Service Catalog");
  auto *form = new QFormLayout();
  form->setSpacing(12);

  auto *id = new QLineEdit(&dialog);
  auto *name = new QLineEdit(&dialog);
  auto *category = new QComboBox(&dialog);
  category->addItems(
      {"All", "Food", "Minibar", "Furniture", "Facility", "Damage"});
  category->setStyleSheet(inputStyle());

  auto *minPrice = new QLineEdit(&dialog);
  auto *maxPrice = new QLineEdit(&dialog);
  auto *statusCombo = new QComboBox(&dialog);
  statusCombo->addItems({"All", "Available", "Unavailable"});
  statusCombo->setStyleSheet(inputStyle());

  for (auto *input : {id, name, minPrice, maxPrice})
    input->setStyleSheet(inputStyle());

  form->addRow("Service ID:", id);
  form->addRow("Name contains:", name);
  form->addRow("Category:", category);
  form->addRow("Minimum price:", minPrice);
  form->addRow("Maximum price:", maxPrice);
  form->addRow("Status:", statusCombo);
  layout->addLayout(form);

  auto *buttons = new QHBoxLayout();
  buttons->setContentsMargins(0, 15, 0, 0);
  auto *cancel = new QPushButton("Cancel", &dialog);
  auto *reset = new QPushButton("Reset", &dialog);
  auto *apply = new QPushButton("Apply Filter", &dialog);

  cancel->setStyleSheet(secondaryButtonStyle());
  reset->setStyleSheet(secondaryButtonStyle());
  apply->setStyleSheet(filterButtonStyle());
  cancel->setCursor(Qt::PointingHandCursor);
  reset->setCursor(Qt::PointingHandCursor);
  apply->setCursor(Qt::PointingHandCursor);

  buttons->addWidget(cancel);
  buttons->addWidget(reset);
  buttons->addWidget(apply);
  layout->addLayout(buttons);

  connect(cancel, &QPushButton::clicked, &dialog, &QDialog::reject);
  connect(reset, &QPushButton::clicked, &dialog, [this, &dialog] {
    showServiceTab();
    dialog.accept();
  });

  connect(
      apply, &QPushButton::clicked, &dialog,
      [this, &dialog, id, name, category, minPrice, maxPrice, statusCombo] {
        double minValue = -1.0;
        double maxValue = -1.0;
        if (!readNonNegativePrice(minPrice, minValue, &dialog, "Minimum price",
                                  true) ||
            !readNonNegativePrice(maxPrice, maxValue, &dialog, "Maximum price",
                                  true)) {
          return;
        }
        if (minValue >= 0.0 && maxValue >= 0.0 && minValue > maxValue) {
          QMessageBox::warning(&dialog, "Invalid input",
                               "Minimum price cannot exceed maximum price.");
          return;
        }

        QString queryStr = R"(
                SELECT 
                    item_id AS "Service ID",
                    item_name AS "Name",
                    category AS "Category",
                    base_price AS "Price",
                    CASE WHEN is_active = 1 THEN 'Available' ELSE 'Unavailable' END AS "Status"
                FROM ServiceCatalog
                WHERE 1=1
            )";

        if (!id->text().trimmed().isEmpty()) {
          queryStr += QString(" AND item_id = '%1'").arg(id->text().trimmed());
        }
        if (!name->text().trimmed().isEmpty()) {
          queryStr +=
              QString(" AND item_name LIKE '%%1%'").arg(name->text().trimmed());
        }
        if (category->currentText() != "All") {
          queryStr +=
              QString(" AND category = '%1'").arg(category->currentText());
        }
        if (minValue >= 0.0) {
          queryStr += QString(" AND base_price >= %1").arg(minValue);
        }
        if (maxValue >= 0.0) {
          queryStr += QString(" AND base_price <= %1").arg(maxValue);
        }

        if (statusCombo->currentText() == "Available") {
          queryStr += " AND is_active = 1";
        } else if (statusCombo->currentText() == "Unavailable") {
          queryStr += " AND is_active = 0";
        }

        Backend::loadTableData(tableService, queryStr);
        dialog.accept();
      });

  dialog.exec();
}