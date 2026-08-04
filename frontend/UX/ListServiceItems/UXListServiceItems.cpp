#include "backend/Manager/DatabaseManager.h"
#include "backend/Repository/BookingRepository.h"
#include "backend/Repository/ServiceItemRepository.h"
#include "cores/Service/InventoryService.h"
#include "frontend/UI/UI.h"
#include "frontend/UX/UX.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QSpinBox>
#include <QtSql/QSqlQuery>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace {

void populateServiceCatalogTable(QTableWidget *table,
                                 const std::vector<ServiceCatalogData> &items) {
  table->setRowCount(0);
  for (const auto &item : items) {
    const int row = table->rowCount();
    table->insertRow(row);
    table->setItem(row, 0,
                   new QTableWidgetItem(QString::fromStdString(item.id)));
    table->setItem(row, 1,
                   new QTableWidgetItem(QString::fromStdString(item.name)));
    table->setItem(row, 2,
                   new QTableWidgetItem(QString::fromStdString(item.category)));
    table->setItem(row, 3,
                   new QTableWidgetItem(QString::number(item.basePrice, 'f', 2)));
    table->setItem(row, 4,
                   new QTableWidgetItem(item.vipFreeStatus ? "Yes" : "No"));
  }
}

QString inputStyle() {
  return "QLineEdit, QComboBox { background-color: #ffffff; border: 2px solid "
         "#38bdf8; border-radius: 8px; padding: 10px; font-size: 14px; "
         "color: #0f172a; }"
         "QLineEdit:hover, QComboBox:hover { border-color: #0284c7; }"
         "QLineEdit:focus, QComboBox:focus { border-color: #0369a1; "
         "background-color: #f0f9ff; }";
}

QString dialogStyle() {
  return "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
         "stop:0 #f0f9ff, stop:1 #ffffff); }"
         "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }"
         "QCheckBox { color: #1e293b; font-size: 14px; spacing: 8px; }"
         "QCheckBox::indicator { width: 18px; height: 18px; border: 2px solid "
         "#94a3b8; border-radius: 5px; background: #ffffff; }"
         "QCheckBox::indicator:checked { background: #6366f1; border-color: "
         "#4f46e5; }";
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
                  "WHERE status IS NULL OR status <> 'CHECKED_OUT' "
                  "ORDER BY id DESC")) {
    return false;
  }

  while (query.next()) {
    const int bookingId = query.value("id").toInt();
    const QString roomNumber = query.value("room_number").toString();
    const QString status = query.value("status").toString();
    comboBox->addItem(
        QString("Booking #%1 — Room %2 (%3)")
            .arg(bookingId)
            .arg(roomNumber.isEmpty() ? "-" : roomNumber,
                 status.isEmpty() ? "UNCONFIRMED" : status),
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
  return query.value("status").toString() != "CHECKED_OUT";
}

} // namespace

void MainWindowController::showServiceTab() {
  stackedWidget->setCurrentIndex(ServiceIndex);
  setActionBarVisible(true);
  setActiveButton(buttonService);

  ServiceItemRepository repository;
  populateServiceCatalogTable(tableService, repository.getAllCatalogItems());

  btnAdd->setVisible(true);
  btnUpdate->setVisible(true);
  btnDelete->setVisible(true);
  btnAddToBooking->setVisible(true);
  btnFilter->setVisible(true);
  btnAdd->disconnect();
  btnUpdate->disconnect();
  btnDelete->disconnect();
  btnAddToBooking->disconnect();
  btnFilter->disconnect();

  connect(btnAdd, &QPushButton::clicked, this,
          &MainWindowController::showAddServiceDialog);
  connect(btnUpdate, &QPushButton::clicked, this,
          &MainWindowController::showUpdateServiceDialog);
  connect(btnDelete, &QPushButton::clicked, this,
          &MainWindowController::showDeleteServiceDialog);
  connect(btnAddToBooking, &QPushButton::clicked, this,
          &MainWindowController::showAddServiceToBookingDialog);
  connect(btnFilter, &QPushButton::clicked, this,
          &MainWindowController::showFilterServiceDialog);
}

void MainWindowController::showAddServiceToBookingDialog() {
  if (!tableService || tableService->currentRow() < 0 ||
      !tableService->item(tableService->currentRow(), 0)) {
    QMessageBox::warning(this, "Select service",
                         "Select a Service Catalog row before adding it to a booking.");
    return;
  }

  const QString serviceId = tableService->item(tableService->currentRow(), 0)->text();
  ServiceItemRepository serviceRepository;
  const auto service = serviceRepository.findCatalogItemById(serviceId.toStdString());
  if (!service) {
    QMessageBox::warning(this, "Service not found",
                         "The selected service no longer exists in the catalog.");
    showServiceTab();
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

  auto *id = new QLineEdit(QString::fromStdString(service->id), &dialog);
  auto *name = new QLineEdit(QString::fromStdString(service->name), &dialog);
  auto *price = new QLineEdit(QString::number(service->basePrice, 'f', 2), &dialog);
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
  id->setStyleSheet(inputStyle() + "QLineEdit { background-color: #e2e8f0; color: #475569; }");
  name->setStyleSheet(inputStyle() + "QLineEdit { background-color: #e2e8f0; color: #475569; }");
  price->setStyleSheet(inputStyle() + "QLineEdit { background-color: #e2e8f0; color: #475569; }");
  quantity->setRange(1, 9999);
  quantity->setValue(1);
  note->setPlaceholderText("Optional customer note");

  if (!loadActiveBookings(booking)) {
    QMessageBox::information(this, "No active bookings",
                             "There is no active booking available for this service.");
    return;
  }

  form->addRow("Service ID:", id);
  form->addRow("Service name:", name);
  form->addRow("Final unit price:", price);
  form->addRow("Booking:", booking);

  // LOGIC ẨN/HIỆN QUANTITY DỰA VÀO CATEGORY
  QString categoryStr = QString::fromStdString(service->category);
  QLabel *qtyLabel = new QLabel("Quantity:", &dialog);
  qtyLabel->setStyleSheet("color: #1e293b; font-weight: bold; font-size: 14px;");
  
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
  connect(save, &QPushButton::clicked, &dialog,
          [this, &dialog, service, booking, quantity, note, categoryStr] {
            const int bookingId = booking->currentData().toInt();
            if (bookingId <= 0 || !isBookingActive(bookingId)) {
              QMessageBox::warning(&dialog, "Booking unavailable",
                                   "This booking no longer exists or has already checked out.");
              return;
            }

            int finalQty = (categoryStr == "Facility" || categoryStr == "Damage") ? 1 : quantity->value();

            // NẾU LÀ HÀNG HÓA TỪ KHO -> CHECK VÀ TRỪ KHO (INVENTORY LOG)
            if (categoryStr == "Food" || categoryStr == "Minibar" || categoryStr == "Furniture") {
                InventoryService invService;
                QString itemName = QString::fromStdString(service->name);
                if (!invService.reserveItem(itemName, finalQty)) {
                    QMessageBox::critical(&dialog, "Out of Stock", 
                                         QString("Not enough '%1' in inventory!").arg(itemName));
                    return;
                }
            }

            BookingRepository bookingRepository;
            const bool added = bookingRepository.addServiceItemToBooking(
                bookingId, service->id, finalQty, service->basePrice * finalQty,
                note->text().trimmed().toStdString());
            
            if (!added) {
              // NẾU LỖI LƯU BOOKING -> HOÀN TRẢ LẠI KHO ĐỂ KHÔNG BỊ THẤT THOÁT
              if (categoryStr == "Food" || categoryStr == "Minibar" || categoryStr == "Furniture") {
                  InventoryService invService;
                  invService.releaseItem(QString::fromStdString(service->name), finalQty);
              }
              QMessageBox::critical(&dialog, "Cannot add service",
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
  auto *vipFree = new QCheckBox("Free for VIP customers", &dialog);

  id->setPlaceholderText("Example: SPA001");
  name->setPlaceholderText("Service name");
  category->setPlaceholderText("Example: Spa, Laundry");
  price->setPlaceholderText("0.00");
  for (auto *input : {id, name, price})
    input->setStyleSheet(inputStyle());
  category->setStyleSheet(inputStyle());

  form->addRow("Service ID:", id);
  form->addRow("Name:", name);
  form->addRow("Category:", category);
  form->addRow("Base price:", price);
  form->addRow("VIP policy:", vipFree);
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
  connect(save, &QPushButton::clicked, &dialog, [this, &dialog, id, name,
                                                   category, price, vipFree] {
    double basePrice = 0.0;
    if (id->text().trimmed().isEmpty() || name->text().trimmed().isEmpty() ||
        category->currentText().trimmed().isEmpty()) {
      QMessageBox::warning(&dialog, "Invalid input",
                           "Service ID, name, and category are required.");
      return;
    }
    if (!readNonNegativePrice(price, basePrice, &dialog, "Base price"))
      return;

    ServiceCatalogData item{ id->text().trimmed().toStdString(),
                             name->text().trimmed().toStdString(),
                             category->currentText().trimmed().toStdString(), basePrice,
                             vipFree->isChecked() };
    ServiceItemRepository repository;
    if (!repository.addCatalogItem(item)) {
      QMessageBox::critical(&dialog, "Cannot add service",
                             "The Service ID may already exist or the database rejected the data.");
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

  const QString itemId = tableService->item(tableService->currentRow(), 0)->text();
  ServiceItemRepository repository;
  const auto current = repository.findCatalogItemById(itemId.toStdString());
  if (!current) {
    QMessageBox::warning(this, "Service not found",
                         "The selected service no longer exists in the database.");
    showServiceTab();
    return;
  }

  QDialog dialog(this);
  dialog.setWindowTitle("Update Service Catalog Item");
  dialog.setFixedSize(440, 430);
  dialog.setStyleSheet(dialogStyle());
  auto *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(30, 30, 30, 30);
  addDialogHeading(layout, &dialog, "Edit Service Catalog Item");
  auto *form = new QFormLayout();
  form->setSpacing(15);
  auto *id = new QLineEdit(QString::fromStdString(current->id), &dialog);
  auto *name = new QLineEdit(QString::fromStdString(current->name), &dialog);
  auto *category = new QComboBox(&dialog);
  category->addItems({"Food", "Minibar", "Furniture", "Facility", "Damage"});
  category->setCurrentText(QString::fromStdString(current->category));
  category->setStyleSheet(inputStyle());
  auto *price = new QLineEdit(QString::number(current->basePrice, 'f', 2), &dialog);
  auto *vipFree = new QCheckBox("Free for VIP customers", &dialog);
  vipFree->setChecked(current->vipFreeStatus);
  id->setReadOnly(true);
  for (auto *input : {id, name, price})
    input->setStyleSheet(inputStyle());
  category->setStyleSheet(inputStyle());
  id->setStyleSheet(inputStyle() +
                    "QLineEdit { background-color: #e2e8f0; color: #475569; }");

  form->addRow("Service ID:", id);
  form->addRow("Name:", name);
  form->addRow("Category:", category);
  form->addRow("Base price:", price);
  form->addRow("VIP policy:", vipFree);
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
  connect(save, &QPushButton::clicked, &dialog, [this, &dialog, id, name,
                                                   category, price, vipFree] {
    double basePrice = 0.0;
    if (name->text().trimmed().isEmpty() || category->currentText().trimmed().isEmpty()) {
      QMessageBox::warning(&dialog, "Invalid input",
                           "Name and category are required.");
      return;
    }
    if (!readNonNegativePrice(price, basePrice, &dialog, "Base price"))
      return;

    ServiceCatalogData item{ id->text().trimmed().toStdString(),
                             name->text().trimmed().toStdString(),
                             category->currentText().trimmed().toStdString(), basePrice,
                             vipFree->isChecked() };
    ServiceItemRepository repository;
    if (!repository.updateCatalogItem(item)) {
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

  const QString itemId = tableService->item(tableService->currentRow(), 0)->text();
  if (QMessageBox::question(this, "Delete service",
                            QString("Delete Service Catalog item '%1'?").arg(itemId),
                            QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
    return;
  }

  ServiceItemRepository repository;
  if (repository.isCatalogItemInUse(itemId.toStdString())) {
    QMessageBox::warning(this, "Cannot delete service",
                         "This service is already used by a booking and is kept to preserve booking history.");
    return;
  }
  if (!repository.removeCatalogItem(itemId.toStdString())) {
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
  auto *category = new QLineEdit(&dialog);
  auto *minPrice = new QLineEdit(&dialog);
  auto *maxPrice = new QLineEdit(&dialog);
  auto *vipFree = new QComboBox(&dialog);
  vipFree->addItem("Any", -1);
  vipFree->addItem("VIP free", 1);
  vipFree->addItem("Not VIP free", 0);
  for (auto *input : {id, name, category, minPrice, maxPrice})
    input->setStyleSheet(inputStyle());
  vipFree->setStyleSheet(inputStyle());

  form->addRow("Service ID:", id);
  form->addRow("Name contains:", name);
  form->addRow("Category:", category);
  form->addRow("Minimum price:", minPrice);
  form->addRow("Maximum price:", maxPrice);
  form->addRow("VIP policy:", vipFree);
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
  connect(apply, &QPushButton::clicked, &dialog,
          [this, &dialog, id, name, category, minPrice, maxPrice, vipFree] {
            double minValue = -1.0;
            double maxValue = -1.0;
            if (!readNonNegativePrice(minPrice, minValue, &dialog, "Minimum price", true) ||
                !readNonNegativePrice(maxPrice, maxValue, &dialog, "Maximum price", true)) {
              return;
            }
            if (minValue >= 0.0 && maxValue >= 0.0 && minValue > maxValue) {
              QMessageBox::warning(&dialog, "Invalid input",
                                   "Minimum price cannot exceed maximum price.");
              return;
            }

            ServiceCatalogFilter filter;
            filter.id = id->text().trimmed().toStdString();
            filter.name = name->text().trimmed().toStdString();
            filter.category = category->text().trimmed().toStdString();
            filter.minBasePrice = minValue;
            filter.maxBasePrice = maxValue;
            filter.vipFreeStatus = vipFree->currentData().toInt();

            ServiceItemRepository repository;
            populateServiceCatalogTable(tableService,
                                        repository.getFilteredCatalogItems(filter));
            dialog.accept();
          });

  dialog.exec();
}
