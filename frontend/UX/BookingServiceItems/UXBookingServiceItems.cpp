#include "backend/Repository/ServiceItemRepository.h"
#include "frontend/UI/UI.h"
#include "frontend/UX/UX.h"

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

namespace {

void populateBookingServiceItemsTable(
    QTableWidget *table, const std::vector<BookingServiceItemData> &items) {
  table->setRowCount(0);
  for (const auto &item : items) {
    const int row = table->rowCount();
    table->insertRow(row);
    table->setItem(row, 0, new QTableWidgetItem(QString::number(item.id)));
    table->setItem(row, 1,
                   new QTableWidgetItem(QString::number(item.bookingId)));
    table->setItem(row, 2,
                   new QTableWidgetItem(QString::fromStdString(item.itemId)));
    table->setItem(row, 3,
                   new QTableWidgetItem(QString::number(item.quantity)));
    table->setItem(
        row, 4,
        new QTableWidgetItem(QString::fromStdString(item.customerNote)));
    table->setItem(row, 5,
                   new QTableWidgetItem(QString::number(item.finalPrice, 'f', 2)));
  }
}

bool readOptionalNonNegativeInteger(QLineEdit *input, int &value,
                                    const QString &fieldName,
                                    QWidget *parent) {
  const QString text = input->text().trimmed();
  if (text.isEmpty()) {
    value = -1;
    return true;
  }

  bool ok = false;
  value = text.toInt(&ok);
  if (!ok || value < 0) {
    QMessageBox::warning(parent, "Invalid input",
                         fieldName + " must be a non-negative integer.");
    return false;
  }
  return true;
}

bool readOptionalNonNegativePrice(QLineEdit *input, double &value,
                                  const QString &fieldName,
                                  QWidget *parent) {
  const QString text = input->text().trimmed();
  if (text.isEmpty()) {
    value = -1.0;
    return true;
  }

  bool ok = false;
  value = text.toDouble(&ok);
  if (!ok || value < 0.0) {
    QMessageBox::warning(parent, "Invalid input",
                         fieldName + " must be a non-negative number.");
    return false;
  }
  return true;
}

} // namespace

// Section 2: Booking Service Items Handler
void MainWindowController::showBookingServicesTab() {
  stackedWidget->setCurrentIndex(BookingIndex);
  bookingPage->setSection(1);
  setActionBarVisible(true);
  setActiveButton(buttonBooking);

  ServiceItemRepository repository;
  BookingServiceItemFilter allItems;
  populateBookingServiceItemsTable(
      tableBookingItems, repository.getFilteredBookingServiceItems(allItems));

  btnAdd->setVisible(false);
  btnUpdate->setVisible(false);
  btnDelete->setVisible(false);
  btnFilter->setVisible(true);
  btnAdd->disconnect();
  btnUpdate->disconnect();
  btnDelete->disconnect();
  btnFilter->disconnect();
  connect(btnFilter, &QPushButton::clicked, this,
          &MainWindowController::showFilterBookingServicesDialog);
}

void MainWindowController::showFilterBookingServicesDialog() {
  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Filter Booking Services");
  dialog->setFixedSize(450, 500);
  dialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

  QVBoxLayout *layout = new QVBoxLayout(dialog);
  layout->setContentsMargins(30, 30, 30, 30);

  QLabel *titleLabel = new QLabel("Filter Booking Services", dialog);
  titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: "
                            "#3730a3; margin-bottom: 15px;");
  layout->addWidget(titleLabel, 0, Qt::AlignCenter);

  QFormLayout *form = new QFormLayout();
  form->setSpacing(12);
  const QString inputStyle =
      "QLineEdit { background-color: #ffffff; border: 2px solid #38bdf8; "
      "border-radius: 8px; padding: 8px; font-size: 14px; color: #0f172a; }"
      "QLineEdit:hover { border: 2px solid #0284c7; }"
      "QLineEdit:focus { border: 2px solid #0369a1; background-color: #f0f9ff; }";

  QLineEdit *bookingId = new QLineEdit(dialog);
  bookingId->setPlaceholderText("All booking IDs");
  QLineEdit *itemId = new QLineEdit(dialog);
  itemId->setPlaceholderText("All service item IDs");
  QLineEdit *minQuantity = new QLineEdit(dialog);
  minQuantity->setPlaceholderText("No minimum");
  QLineEdit *maxQuantity = new QLineEdit(dialog);
  maxQuantity->setPlaceholderText("No maximum");
  QLineEdit *minPrice = new QLineEdit(dialog);
  minPrice->setPlaceholderText("No minimum");
  QLineEdit *maxPrice = new QLineEdit(dialog);
  maxPrice->setPlaceholderText("No maximum");
  QLineEdit *note = new QLineEdit(dialog);
  note->setPlaceholderText("Note contains...");

  for (auto *input : {bookingId, itemId, minQuantity, maxQuantity, minPrice,
                      maxPrice, note}) {
    input->setStyleSheet(inputStyle);
  }

  form->addRow("Booking ID:", bookingId);
  form->addRow("Service Item ID:", itemId);
  form->addRow("Minimum quantity:", minQuantity);
  form->addRow("Maximum quantity:", maxQuantity);
  form->addRow("Minimum final price:", minPrice);
  form->addRow("Maximum final price:", maxPrice);
  form->addRow("Note contains:", note);
  layout->addLayout(form);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setContentsMargins(0, 15, 0, 0);
  QPushButton *cancel = new QPushButton("Cancel", dialog);
  QPushButton *reset = new QPushButton("Reset", dialog);
  QPushButton *apply = new QPushButton("Apply Filter", dialog);

  apply->setStyleSheet(
      "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #10b981, stop:1 #059669); color: white; border: none; "
      "border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold; }"
      "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #059669, stop:1 #047857); }");
  reset->setStyleSheet(
      "QPushButton { background-color: #cbd5e1; color: #475569; border: none; "
      "border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold; }"
      "QPushButton:hover { background-color: #94a3b8; color: #1e293b; }");
  cancel->setStyleSheet(
      "background-color: #e2e8f0; color: #64748b; border: none; border-radius: "
      "8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

  for (auto *button : {cancel, reset, apply})
    button->setCursor(Qt::PointingHandCursor);
  buttonLayout->addWidget(cancel);
  buttonLayout->addWidget(reset);
  buttonLayout->addWidget(apply);
  layout->addLayout(buttonLayout);

  connect(cancel, &QPushButton::clicked, dialog, &QDialog::reject);
  connect(reset, &QPushButton::clicked, this, [this, dialog] {
    showBookingServicesTab();
    dialog->accept();
  });
  connect(apply, &QPushButton::clicked, this,
          [this, dialog, bookingId, itemId, minQuantity, maxQuantity, minPrice,
           maxPrice, note] {
            BookingServiceItemFilter filter;
            if (!readOptionalNonNegativeInteger(bookingId, filter.bookingId,
                                                "Booking ID", dialog) ||
                !readOptionalNonNegativeInteger(minQuantity, filter.minQuantity,
                                                "Minimum quantity", dialog) ||
                !readOptionalNonNegativeInteger(maxQuantity, filter.maxQuantity,
                                                "Maximum quantity", dialog) ||
                !readOptionalNonNegativePrice(minPrice, filter.minFinalPrice,
                                              "Minimum final price", dialog) ||
                !readOptionalNonNegativePrice(maxPrice, filter.maxFinalPrice,
                                              "Maximum final price", dialog)) {
              return;
            }

            if (filter.minQuantity >= 0 && filter.maxQuantity >= 0 &&
                filter.minQuantity > filter.maxQuantity) {
              QMessageBox::warning(dialog, "Invalid input",
                                   "Minimum quantity cannot exceed maximum quantity.");
              return;
            }
            if (filter.minFinalPrice >= 0.0 && filter.maxFinalPrice >= 0.0 &&
                filter.minFinalPrice > filter.maxFinalPrice) {
              QMessageBox::warning(dialog, "Invalid input",
                                   "Minimum final price cannot exceed maximum final price.");
              return;
            }

            filter.itemId = itemId->text().trimmed().toStdString();
            filter.customerNote = note->text().trimmed().toStdString();

            ServiceItemRepository repository;
            const auto items = repository.getFilteredBookingServiceItems(filter);
            populateBookingServiceItemsTable(tableBookingItems, items);
            QMessageBox::information(dialog, "Filter Results",
                                     QString("Found %1 booking service item(s).").arg(items.size()));
            dialog->accept();
          });

  dialog->exec();
  dialog->deleteLater();
}
