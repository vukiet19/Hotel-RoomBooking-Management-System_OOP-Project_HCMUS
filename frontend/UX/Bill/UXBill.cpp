#include "backend/Manager/CheckoutService.h"
#include "backend/Manager/DatabaseManager.h"
#include "frontend/UI/UI.h"
#include "frontend/UX/UX.h"

#include <QComboBox>
#include <QDate>
#include <QDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
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

namespace {

struct BillFilter {
  int billId = -1;
  int bookingId = -1;
  QString startDate;
  QString endDate;
  double minTotal = -1.0;
  double maxTotal = -1.0;
  QString paymentMethod;
};

QString formatMoney(double amount) {
  return QLocale(QLocale::English).toString(amount, 'f', 0);
}

bool loadBills(QTableWidget *table, const BillFilter &filter,
               QString *errorMessage = nullptr) {
  CheckoutService checkoutService;
  if (!checkoutService.prepareSchema(errorMessage))
    return false;

  QString sql = R"(
    SELECT bill.bill_id,
           bill.booking_id,
           COALESCE(customer.full_name, 'Guest #' || bill.booking_id) AS customer_name,
           COALESCE(room.room_number, booking.room_number, '-') AS room_number,
           COALESCE(bill.room_charge, 0.0) AS room_charge,
           COALESCE(bill.service_charge, 0.0) AS service_charge,
           COALESCE(bill.discount_amount, 0.0) AS discount_amount,
           COALESCE(bill.deposit_amount, 0.0) AS deposit_amount,
           bill.total_amount,
           COALESCE(bill.payment_method, '-') AS payment_method,
           COALESCE(bill.checkout_time, '-') AS checkout_time
      FROM Bills bill
      LEFT JOIN Bookings booking ON booking.id = bill.booking_id
      LEFT JOIN Customer customer ON customer.id = booking.customer_id
      LEFT JOIN ListRooms room ON room.room_number = booking.room_number
     WHERE 1 = 1
  )";

  if (filter.billId >= 0)
    sql += " AND bill.bill_id = :bill_id";
  if (filter.bookingId >= 0)
    sql += " AND bill.booking_id = :booking_id";
  if (!filter.startDate.isEmpty())
    sql += " AND bill.checkout_time >= :start_date";
  if (!filter.endDate.isEmpty())
    sql += " AND bill.checkout_time <= :end_date";
  if (filter.minTotal >= 0.0)
    sql += " AND bill.total_amount >= :min_total";
  if (filter.maxTotal >= 0.0)
    sql += " AND bill.total_amount <= :max_total";
  if (!filter.paymentMethod.isEmpty())
    sql += " AND bill.payment_method = :payment_method";
  sql += " ORDER BY bill.checkout_time DESC, bill.bill_id DESC";

  QSqlQuery query(DatabaseManager::instance().database());
  query.prepare(sql);
  if (filter.billId >= 0)
    query.bindValue(":bill_id", filter.billId);
  if (filter.bookingId >= 0)
    query.bindValue(":booking_id", filter.bookingId);
  if (!filter.startDate.isEmpty())
    query.bindValue(":start_date", filter.startDate + "T00:00:00");
  if (!filter.endDate.isEmpty())
    query.bindValue(":end_date", filter.endDate + "T23:59:59");
  if (filter.minTotal >= 0.0)
    query.bindValue(":min_total", filter.minTotal);
  if (filter.maxTotal >= 0.0)
    query.bindValue(":max_total", filter.maxTotal);
  if (!filter.paymentMethod.isEmpty())
    query.bindValue(":payment_method", filter.paymentMethod);

  if (!query.exec()) {
    if (errorMessage)
      *errorMessage = query.lastError().text();
    return false;
  }

  table->setRowCount(0);
  while (query.next()) {
    const int row = table->rowCount();
    table->insertRow(row);
    table->setItem(row, 0,
                   new QTableWidgetItem(query.value("bill_id").toString()));
    table->setItem(row, 1,
                   new QTableWidgetItem(query.value("booking_id").toString()));
    table->setItem(
        row, 2, new QTableWidgetItem(query.value("customer_name").toString()));
    table->setItem(row, 3,
                   new QTableWidgetItem(query.value("room_number").toString()));
    table->setItem(row, 4,
                   new QTableWidgetItem(
                       formatMoney(query.value("room_charge").toDouble())));
    table->setItem(row, 5,
                   new QTableWidgetItem(
                       formatMoney(query.value("service_charge").toDouble())));
    table->setItem(row, 6,
                   new QTableWidgetItem(
                       formatMoney(query.value("discount_amount").toDouble())));
    table->setItem(row, 7,
                   new QTableWidgetItem(
                       formatMoney(query.value("deposit_amount").toDouble())));
    table->setItem(row, 8,
                   new QTableWidgetItem(
                       formatMoney(query.value("total_amount").toDouble())));
    table->setItem(
        row, 9, new QTableWidgetItem(query.value("payment_method").toString()));
    table->setItem(
        row, 10, new QTableWidgetItem(query.value("checkout_time").toString()));
  }

  table->resizeColumnsToContents();
  table->horizontalHeader()->setMinimumSectionSize(110);
  return true;
}

bool readOptionalId(QLineEdit *input, int &value, const QString &label,
                    QWidget *parent) {
  const QString text = input->text().trimmed();
  if (text.isEmpty()) {
    value = -1;
    return true;
  }

  bool ok = false;
  value = text.toInt(&ok);
  if (!ok || value <= 0) {
    QMessageBox::warning(parent, "Invalid input",
                         label + " must be a positive integer.");
    return false;
  }
  return true;
}

bool readOptionalAmount(QLineEdit *input, double &value, const QString &label,
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
                         label + " must be a non-negative number.");
    return false;
  }
  return true;
}

bool readOptionalDate(QLineEdit *input, QString &value, const QString &label,
                      QWidget *parent) {
  value = input->text().trimmed();
  if (value.isEmpty())
    return true;
  if (!QDate::fromString(value, "yyyy-MM-dd").isValid()) {
    QMessageBox::warning(parent, "Invalid input",
                         label + " must use YYYY-MM-DD.");
    return false;
  }
  return true;
}

QString inputStyle() {
  return "QLineEdit, QComboBox { background-color: #ffffff; border: 2px solid "
         "#38bdf8; border-radius: 8px; padding: 8px; font-size: 14px; "
         "color: #0f172a; }"
         "QLineEdit:hover, QComboBox:hover { border-color: #0284c7; }"
         "QLineEdit:focus, QComboBox:focus { border-color: #0369a1; "
         "background-color: #f0f9ff; }";
}

} // namespace

void MainWindowController::showBillTab() {
  stackedWidget->setCurrentIndex(BillIndex);
  setActionBarVisible(true);
  setActiveButton(buttonBill);

  QString errorMessage;
  if (!loadBills(tableBill, BillFilter{}, &errorMessage)) {
    QMessageBox::critical(this, "Cannot load bills", errorMessage);
  }

  btnAdd->setVisible(false);
  btnUpdate->setVisible(false);
  btnDelete->setVisible(false);
  btnFilter->setVisible(true);
  btnAdd->disconnect();
  btnUpdate->disconnect();
  btnDelete->disconnect();
  btnFilter->disconnect();
  connect(btnFilter, &QPushButton::clicked, this,
          &MainWindowController::showFilterBillDialog);
}

void MainWindowController::showFilterBillDialog() {
  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Filter Bills");
  dialog->setFixedSize(450, 470);
  dialog->setStyleSheet(
      "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
      "#f0f9ff, stop:1 #ffffff); }"
      "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

  auto *layout = new QVBoxLayout(dialog);
  layout->setContentsMargins(30, 30, 30, 30);
  auto *title = new QLabel("Filter Bills", dialog);
  title->setStyleSheet("font-size: 20px; font-weight: bold; color: #3730a3; "
                       "margin-bottom: 15px;");
  layout->addWidget(title, 0, Qt::AlignCenter);

  auto *form = new QFormLayout();
  form->setSpacing(12);
  auto *billId = new QLineEdit(dialog);
  auto *bookingId = new QLineEdit(dialog);
  auto *startDate = new QLineEdit(dialog);
  auto *endDate = new QLineEdit(dialog);
  auto *minTotal = new QLineEdit(dialog);
  auto *maxTotal = new QLineEdit(dialog);
  auto *paymentMethod = new QComboBox(dialog);
  paymentMethod->addItem("All payment methods", "");
  paymentMethod->addItem("Cash", "Cash");
  paymentMethod->addItem("Card", "Card");
  paymentMethod->addItem("Bank transfer", "Bank transfer");

  billId->setPlaceholderText("All bill IDs");
  bookingId->setPlaceholderText("All booking IDs");
  startDate->setPlaceholderText("YYYY-MM-DD");
  endDate->setPlaceholderText("YYYY-MM-DD");
  minTotal->setPlaceholderText("No minimum");
  maxTotal->setPlaceholderText("No maximum");
  for (auto *input :
       {billId, bookingId, startDate, endDate, minTotal, maxTotal})
    input->setStyleSheet(inputStyle());
  paymentMethod->setStyleSheet(inputStyle());

  form->addRow("Bill ID:", billId);
  form->addRow("Booking ID:", bookingId);
  form->addRow("From checkout date:", startDate);
  form->addRow("To checkout date:", endDate);
  form->addRow("Minimum total:", minTotal);
  form->addRow("Maximum total:", maxTotal);
  form->addRow("Payment method:", paymentMethod);
  layout->addLayout(form);

  auto *buttonLayout = new QHBoxLayout();
  auto *cancel = new QPushButton("Cancel", dialog);
  auto *reset = new QPushButton("Reset", dialog);
  auto *apply = new QPushButton("Apply Filter", dialog);
  cancel->setStyleSheet(
      "background-color: #e2e8f0; color: #64748b; border: none; border-radius: "
      "8px; padding: 10px 0; font-size: 15px; font-weight: bold;");
  reset->setStyleSheet(
      "QPushButton { background-color: #cbd5e1; color: #475569; border: none; "
      "border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: "
      "bold; }"
      "QPushButton:hover { background-color: #94a3b8; color: #1e293b; }");
  apply->setStyleSheet(
      "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #10b981, stop:1 #059669); color: white; border: none; "
      "border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: "
      "bold; }"
      "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #059669, stop:1 #047857); }");
  for (auto *button : {cancel, reset, apply})
    button->setCursor(Qt::PointingHandCursor);
  buttonLayout->addWidget(cancel);
  buttonLayout->addWidget(reset);
  buttonLayout->addWidget(apply);
  layout->addLayout(buttonLayout);

  connect(cancel, &QPushButton::clicked, dialog, &QDialog::reject);
  connect(reset, &QPushButton::clicked, this, [this, dialog] {
    showBillTab();
    dialog->accept();
  });
  connect(
      apply, &QPushButton::clicked, this,
      [this, dialog, billId, bookingId, startDate, endDate, minTotal, maxTotal,
       paymentMethod] {
        BillFilter filter;
        if (!readOptionalId(billId, filter.billId, "Bill ID", dialog) ||
            !readOptionalId(bookingId, filter.bookingId, "Booking ID",
                            dialog) ||
            !readOptionalDate(startDate, filter.startDate, "Start date",
                              dialog) ||
            !readOptionalDate(endDate, filter.endDate, "End date", dialog) ||
            !readOptionalAmount(minTotal, filter.minTotal, "Minimum total",
                                dialog) ||
            !readOptionalAmount(maxTotal, filter.maxTotal, "Maximum total",
                                dialog)) {
          return;
        }
        if (!filter.startDate.isEmpty() && !filter.endDate.isEmpty() &&
            filter.startDate > filter.endDate) {
          QMessageBox::warning(dialog, "Invalid input",
                               "Start date cannot be after end date.");
          return;
        }
        if (filter.minTotal >= 0.0 && filter.maxTotal >= 0.0 &&
            filter.minTotal > filter.maxTotal) {
          QMessageBox::warning(dialog, "Invalid input",
                               "Minimum total cannot exceed maximum total.");
          return;
        }

        filter.paymentMethod = paymentMethod->currentData().toString();
        QString errorMessage;
        if (!loadBills(tableBill, filter, &errorMessage)) {
          QMessageBox::critical(dialog, "Cannot filter bills", errorMessage);
          return;
        }
        QMessageBox::information(
            dialog, "Filter Results",
            QString("Found %1 bill(s).").arg(tableBill->rowCount()));
        dialog->accept();
      });

  dialog->exec();
  dialog->deleteLater();
}
