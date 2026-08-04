#include "CheckoutPage.h"

#include <QComboBox>
#include <QDate>
#include <QDialog>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include <QDebug>

namespace {
QString formatCleanDate(QString dtStr) {
  dtStr = dtStr.trimmed();
  if (dtStr.isEmpty())
    return "-";
  if (dtStr.length() >= 10 && dtStr[4] == '-' && dtStr[7] == '-') {
    QDate dVal = QDate::fromString(dtStr.left(10), "yyyy-MM-dd");
    if (dVal.isValid())
      return dVal.toString("yyyy-MM-dd");
  }
  return dtStr;
}

QString formatMoney(double amount) {
  return QString("%1").arg(amount, 0, 'f', 0);
}

QLabel *createFieldLabel(const QString &text, QWidget *parent) {
  auto *label = new QLabel(text, parent);
  label->setStyleSheet("color: #475569; font-weight: 600; font-size: 14px;");
  return label;
}

QLabel *createValueLabel(QWidget *parent) {
  auto *label = new QLabel("-", parent);
  label->setStyleSheet("color: #0f172a; font-weight: 600; font-size: 14px;");
  return label;
}
} // namespace

CheckoutPage::CheckoutPage(QWidget *parent) : QWidget(parent) {
  setObjectName("checkoutPage");
  setupUi();
  loadBookings();
  populateBookingTable();
  clearBookingDetails();
}

void CheckoutPage::showEvent(QShowEvent *event) {
  QWidget::showEvent(event);
  loadBookings();
  populateBookingTable();
}

void CheckoutPage::setupUi() {
  setStyleSheet(R"(
        #checkoutPage { background-color: transparent; }
        #checkoutPage QGroupBox {
            background-color: #ffffff;
            color: #3730a3;
            border: 2px solid #bae6fd;
            border-radius: 10px;
            margin-top: 12px;
            padding: 16px 12px 12px 12px;
            font-weight: bold;
            font-size: 15px;
        }
        #checkoutPage QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
            color: #3730a3;
            background: transparent;
        }
        #checkoutPage QScrollArea#checkoutDetailsScroll {
            background: transparent;
            border: none;
        }
        #checkoutPage QScrollArea#checkoutDetailsScroll::viewport {
            background: transparent;
        }
        #checkoutPage #checkoutDetailsContainer { background: transparent; }
        #checkoutPage QTableWidget {
            background-color: #ffffff;
            alternate-background-color: #f0f9ff;
            border: 2px solid #bae6fd;
            border-radius: 8px;
            gridline-color: #e0f2fe; 
            font-size: 14px;
            color: #0f172a; 
            selection-background-color: #38bdf8; 
            selection-color: #ffffff;
            outline: none; 
        }
        #checkoutPage QTableWidget::item { padding: 6px; }
        #checkoutPage QTableWidget::item:hover { background-color: #e0f2fe; color: #0f172a; }
        #checkoutPage QHeaderView::section:horizontal {
            background-color: #312e81; 
            color: #ffffff;
            font-weight: bold;
            font-size: 14px;
            padding: 10px;
            border: none;
            border-right: 1px solid #1e1b4b;
        }
        #checkoutPage QHeaderView::section:horizontal:first { border-top-left-radius: 8px; }
        #checkoutPage QHeaderView::section:horizontal:last { border-top-right-radius: 8px; border-right: none; }
        #checkoutPage QHeaderView::section:vertical {
            background-color: #f8fafc;
            color: #64748b;
            font-weight: bold;
            padding: 4px; 
            border: none;
            border-right: 1px solid #bae6fd;
        }
        #checkoutPage QTableCornerButton::section { background-color: #312e81; border: none; }
        #checkoutPage QComboBox, #checkoutPage QLineEdit {
            background-color: #ffffff;
            border: 2px solid #38bdf8;
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 14px;
            color: #0f172a;
        }
        #checkoutPage QComboBox:hover, #checkoutPage QLineEdit:hover { border: 2px solid #0284c7; }
        #checkoutPage QComboBox:focus, #checkoutPage QLineEdit:focus { border: 2px solid #0369a1; background-color: #f0f9ff; }
        #checkoutPage QComboBox QAbstractItemView, #checkoutPage QComboBox QListView {
            background-color: #ffffff;
            color: #0f172a;
            border: 2px solid #38bdf8;
            border-radius: 6px;
            selection-background-color: #f0f9ff;
            selection-color: #0369a1;
            outline: none;
        }
        #checkoutPage QGroupBox QLabel {
            color: #475569;
            font-weight: 600;
            font-size: 14px;
        }
    )");

  auto *rootLayout = new QVBoxLayout(this);
  rootLayout->setContentsMargins(0, 0, 0, 0);
  rootLayout->setSpacing(12);

  auto *title = new QLabel("Checkout", this);
  title->setStyleSheet(
      "font-size: 28px; font-weight: bold; color: #3730a3; font-family: 'Segoe "
      "UI', Arial, sans-serif; background: transparent;");
  rootLayout->addWidget(title);

  auto *subtitle = new QLabel("Select an active booking to review guest info "
                              "and calculate checkout total.",
                              this);
  subtitle->setStyleSheet("color: #475569; font-size: 14px; font-weight: 500; "
                          "background: transparent;");
  rootLayout->addWidget(subtitle);

  auto *searchLayout = new QHBoxLayout();
  searchEdit = new QLineEdit(this);
  searchEdit->setPlaceholderText(
      "Search by booking ID, customer name, phone or room...");

  auto *searchButton = new QPushButton("Search", this);
  auto *clearButton = new QPushButton("Clear", this);
  searchButton->setCursor(Qt::PointingHandCursor);
  clearButton->setCursor(Qt::PointingHandCursor);
  searchButton->setStyleSheet(
      "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #6366f1, stop:1 #8b5cf6); color: white; border: none; "
      "border-radius: 8px; padding: 9px 20px; font-size: 14px; font-weight: "
      "bold; }"
      "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #4f46e5, stop:1 #7c3aed); }");
  clearButton->setStyleSheet(
      "QPushButton { background-color: #cbd5e1; color: #475569; border: none; "
      "border-radius: 8px; padding: 9px 20px; font-size: 14px; font-weight: "
      "bold; }"
      "QPushButton:hover { background-color: #94a3b8; color: #ffffff; }");
  searchLayout->addWidget(searchEdit, 1);
  searchLayout->addWidget(searchButton);
  searchLayout->addWidget(clearButton);
  rootLayout->addLayout(searchLayout);

  auto *bookingGroup = new QGroupBox("Active bookings", this);
  auto *bookingLayout = new QVBoxLayout(bookingGroup);
  bookingLayout->setContentsMargins(10, 14, 10, 10);

  bookingTable = new QTableWidget(0, 5, bookingGroup);
  bookingTable->setHorizontalHeaderLabels(
      {"Booking ID", "Customer", "Room", "Check-in", "Expected checkout"});
  bookingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  bookingTable->setSelectionMode(QAbstractItemView::SingleSelection);
  bookingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  bookingTable->setAlternatingRowColors(true);
  bookingTable->verticalHeader()->setVisible(false);
  bookingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  bookingTable->setMinimumHeight(170);
  bookingTable->setMaximumHeight(220);
  bookingLayout->addWidget(bookingTable);
  rootLayout->addWidget(bookingGroup);

  auto *detailsScroll = new QScrollArea(this);
  detailsScroll->setObjectName("checkoutDetailsScroll");
  detailsScroll->setWidgetResizable(true);
  detailsScroll->setFrameShape(QFrame::NoFrame);
  detailsScroll->viewport()->setAutoFillBackground(false);

  detailsContainer = new QWidget(detailsScroll);
  detailsContainer->setObjectName("checkoutDetailsContainer");
  detailsContainer->setAttribute(Qt::WA_StyledBackground, true);
  detailsContainer->setSizePolicy(QSizePolicy::Expanding,
                                  QSizePolicy::Preferred);
  auto *detailsLayout = new QVBoxLayout(detailsContainer);
  detailsLayout->setContentsMargins(0, 0, 0, 0);
  detailsLayout->setSpacing(12);

  auto *topDetailsLayout = new QHBoxLayout();
  topDetailsLayout->setSpacing(12);

  auto *bookingInfoGroup =
      new QGroupBox("Booking information", detailsContainer);
  auto *bookingInfoLayout = new QFormLayout(bookingInfoGroup);
  bookingInfoLayout->setContentsMargins(12, 16, 12, 12);
  bookingInfoLayout->setSpacing(8);

  bookingIdLabel = createValueLabel(bookingInfoGroup);
  customerNameLabel = createValueLabel(bookingInfoGroup);
  phoneLabel = createValueLabel(bookingInfoGroup);
  roomLabel = createValueLabel(bookingInfoGroup);
  roomTypeLabel = createValueLabel(bookingInfoGroup);
  checkInLabel = createValueLabel(bookingInfoGroup);
  expectedCheckOutLabel = createValueLabel(bookingInfoGroup);
  nightsLabel = createValueLabel(bookingInfoGroup);

  bookingInfoLayout->addRow(createFieldLabel("Booking ID:", bookingInfoGroup),
                            bookingIdLabel);
  bookingInfoLayout->addRow(createFieldLabel("Customer:", bookingInfoGroup),
                            customerNameLabel);
  bookingInfoLayout->addRow(createFieldLabel("Phone:", bookingInfoGroup),
                            phoneLabel);
  bookingInfoLayout->addRow(createFieldLabel("Room:", bookingInfoGroup),
                            roomLabel);
  bookingInfoLayout->addRow(createFieldLabel("Room type:", bookingInfoGroup),
                            roomTypeLabel);
  bookingInfoLayout->addRow(createFieldLabel("Check-in:", bookingInfoGroup),
                            checkInLabel);
  bookingInfoLayout->addRow(
      createFieldLabel("Expected checkout:", bookingInfoGroup),
      expectedCheckOutLabel);
  bookingInfoLayout->addRow(
      createFieldLabel("Number of nights:", bookingInfoGroup), nightsLabel);
  topDetailsLayout->addWidget(bookingInfoGroup, 1);

  auto *summaryGroup = new QGroupBox("Checkout summary", detailsContainer);
  auto *summaryLayout = new QFormLayout(summaryGroup);
  summaryLayout->setContentsMargins(12, 16, 12, 12);
  summaryLayout->setSpacing(8);
  roomChargeLabel = createValueLabel(summaryGroup);
  serviceChargeLabel = createValueLabel(summaryGroup);
  discountLabel = createValueLabel(summaryGroup);
  depositLabel = createValueLabel(summaryGroup);
  totalLabel = createValueLabel(summaryGroup);
  totalLabel->setStyleSheet(
      "color: #1d4ed8; font-size: 17px; font-weight: bold; padding: 4px 0;");

  summaryLayout->addRow(createFieldLabel("Room charge:", summaryGroup),
                        roomChargeLabel);
  summaryLayout->addRow(createFieldLabel("Service charge:", summaryGroup),
                        serviceChargeLabel);
  summaryLayout->addRow(createFieldLabel("Discount:", summaryGroup),
                        discountLabel);
  summaryLayout->addRow(createFieldLabel("Deposit deducted:", summaryGroup),
                        depositLabel);
  summaryLayout->addRow(createFieldLabel("Total to pay:", summaryGroup),
                        totalLabel);
  topDetailsLayout->addWidget(summaryGroup, 1);
  detailsLayout->addLayout(topDetailsLayout);

  auto *serviceGroup = new QGroupBox("Services used", detailsContainer);
  auto *serviceLayout = new QVBoxLayout(serviceGroup);
  serviceLayout->setContentsMargins(10, 16, 10, 10);
  serviceTable = new QTableWidget(0, 3, serviceGroup);
  serviceTable->setHorizontalHeaderLabels({"Service", "Quantity", "Amount"});
  serviceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  serviceTable->verticalHeader()->setVisible(false);
  serviceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  serviceTable->setMinimumHeight(100);
  serviceTable->setMaximumHeight(150);
  serviceLayout->addWidget(serviceTable);
  detailsLayout->addWidget(serviceGroup);

  auto *actionsLayout = new QHBoxLayout();
  actionsLayout->setSpacing(10);
  auto *paymentLabel = createFieldLabel("Payment method:", detailsContainer);
  paymentMethodComboBox = new QComboBox(detailsContainer);
  paymentMethodComboBox->addItems({"Cash", "Card", "Bank transfer"});
  paymentMethodComboBox->setMinimumWidth(150);
  actionsLayout->addWidget(paymentLabel);
  actionsLayout->addWidget(paymentMethodComboBox);
  actionsLayout->addStretch();

  confirmButton = new QPushButton("Confirm checkout", detailsContainer);
  confirmButton->setMinimumSize(190, 42);
  confirmButton->setCursor(Qt::PointingHandCursor);
  confirmButton->setStyleSheet(
      "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #10b981, stop:1 #059669); color: white; border: none; "
      "border-radius: 8px; padding: 10px 22px; font-size: 15px; font-weight: "
      "bold; }"
      "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
      "stop:0 #059669, stop:1 #047857); }"
      "QPushButton:disabled { background-color: #cbd5e1; color: #64748b; }");
  actionsLayout->addWidget(confirmButton);
  detailsLayout->addLayout(actionsLayout);

  detailsScroll->setWidget(detailsContainer);
  rootLayout->addWidget(detailsScroll, 1);

  connect(searchButton, &QPushButton::clicked, this,
          [this]() { populateBookingTable(searchEdit->text()); });
  connect(searchEdit, &QLineEdit::returnPressed, this,
          [this]() { populateBookingTable(searchEdit->text()); });
  connect(clearButton, &QPushButton::clicked, this, [this]() {
    searchEdit->clear();
    loadBookings();
    populateBookingTable();
  });
  connect(bookingTable, &QTableWidget::cellClicked, this,
          [this](int row, int) { showBookingDetails(row); });
  connect(confirmButton, &QPushButton::clicked, this,
          &CheckoutPage::showConfirmDialog);
}

void CheckoutPage::loadBookings() {
  QString errorMessage;
  CheckoutService checkoutService;
  bookings = checkoutService.getActiveBookings(&errorMessage);
  if (!errorMessage.isEmpty())
    qDebug() << "[ERROR] Failed to load checkout bookings:" << errorMessage;
}
void CheckoutPage::populateBookingTable(const QString &filter) {
  bookingTable->setRowCount(0);
  clearBookingDetails();

  const QString needle = filter.trimmed().toLower();
  for (int index = 0; index < bookings.size(); ++index) {
    const auto &booking = bookings.at(index);
    const QString searchable =
        QString("%1 %2 %3 %4")
            .arg(QString::number(booking.bookingId), booking.customerName,
                 booking.phone, booking.roomNumber)
            .toLower();
    if (!needle.isEmpty() && !searchable.contains(needle))
      continue;

    const int row = bookingTable->rowCount();
    bookingTable->insertRow(row);

    auto *bookingIdItem =
        new QTableWidgetItem(QString::number(booking.bookingId));
    bookingIdItem->setData(Qt::UserRole, index);
    bookingTable->setItem(row, 0, bookingIdItem);
    bookingTable->setItem(row, 1, new QTableWidgetItem(booking.customerName));
    bookingTable->setItem(row, 2, new QTableWidgetItem(booking.roomNumber));
    bookingTable->setItem(
        row, 3, new QTableWidgetItem(formatCleanDate(booking.checkInDate)));
    bookingTable->setItem(
        row, 4,
        new QTableWidgetItem(formatCleanDate(booking.expectedCheckOutDate)));
  }

  if (bookingTable->rowCount() > 0)
    bookingTable->selectRow(0);
}

void CheckoutPage::showBookingDetails(int row) {
  if (row < 0 || !bookingTable->item(row, 0))
    return;

  const int index = bookingTable->item(row, 0)->data(Qt::UserRole).toInt();
  if (index < 0 || index >= bookings.size())
    return;

  const auto &booking = bookings.at(index);
  detailsContainer->setEnabled(true);

  bookingIdLabel->setText(QString::number(booking.bookingId));
  customerNameLabel->setText(booking.customerName);
  phoneLabel->setText(booking.phone);
  roomLabel->setText(booking.roomNumber);
  roomTypeLabel->setText(booking.roomType);
  checkInLabel->setText(formatCleanDate(booking.checkInDate));
  expectedCheckOutLabel->setText(formatCleanDate(booking.expectedCheckOutDate));
  nightsLabel->setText(QString::number(booking.nights));

  serviceTable->setRowCount(0);
  for (const auto &service : booking.services) {
    const int serviceRow = serviceTable->rowCount();
    serviceTable->insertRow(serviceRow);
    serviceTable->setItem(serviceRow, 0, new QTableWidgetItem(service.name));
    serviceTable->setItem(
        serviceRow, 1, new QTableWidgetItem(QString::number(service.quantity)));
    serviceTable->setItem(serviceRow, 2,
                          new QTableWidgetItem(formatMoney(service.quantity *
                                                           service.unitPrice)));
  }

  roomChargeLabel->setText(formatMoney(booking.roomCharge));
  serviceChargeLabel->setText(formatMoney(booking.serviceCharge));
  discountLabel->setText(formatMoney(booking.discount));
  depositLabel->setText(formatMoney(booking.deposit));
  totalLabel->setText(formatMoney(booking.totalAmount));
}

void CheckoutPage::clearBookingDetails() {
  detailsContainer->setEnabled(false);
  bookingIdLabel->setText("-");
  customerNameLabel->setText("-");
  phoneLabel->setText("-");
  roomLabel->setText("-");
  roomTypeLabel->setText("-");
  checkInLabel->setText("-");
  expectedCheckOutLabel->setText("-");
  nightsLabel->setText("-");
  serviceTable->setRowCount(0);
  roomChargeLabel->setText("-");
  serviceChargeLabel->setText("-");
  discountLabel->setText("-");
  depositLabel->setText("-");
  totalLabel->setText("-");
}

void CheckoutPage::showConfirmDialog() {
  if (!bookingTable->currentItem())
    return;

  const int row = bookingTable->currentRow();
  const int index = bookingTable->item(row, 0)->data(Qt::UserRole).toInt();
  if (index < 0 || index >= bookings.size())
    return;

  const auto &booking = bookings.at(index);
  QDialog dialog(this);
  dialog.setObjectName("checkoutConfirmDialog");
  dialog.setWindowTitle("Confirm checkout");
  dialog.setModal(true);
  dialog.setFixedSize(480, 330);
  dialog.setStyleSheet(R"(
    QDialog#checkoutConfirmDialog { background: #f8fafc; }
    #checkoutConfirmDialog QLabel { font-family: 'Segoe UI', Arial, sans-serif; }
    #checkoutConfirmDialog #checkoutSummaryCard {
      background: #ffffff;
      border: 1px solid #dbeafe;
      border-radius: 10px;
    }
    #checkoutConfirmDialog QPushButton {
      border-radius: 8px;
      padding: 10px 18px;
      font-size: 14px;
      font-weight: 700;
      min-width: 120px;
    }
    #checkoutConfirmDialog QPushButton#cancelCheckoutButton {
      background: #ffffff;
      color: #475569;
      border: 1px solid #cbd5e1;
    }
    #checkoutConfirmDialog QPushButton#cancelCheckoutButton:hover {
      background: #f1f5f9;
      border-color: #94a3b8;
    }
    #checkoutConfirmDialog QPushButton#confirmCheckoutButton {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                  stop:0 #10b981, stop:1 #059669);
      color: #ffffff;
      border: none;
    }
    #checkoutConfirmDialog QPushButton#confirmCheckoutButton:hover {
      background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                  stop:0 #059669, stop:1 #047857);
    }
  )");

  auto *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(26, 24, 26, 22);
  layout->setSpacing(12);

  auto *title = new QLabel("Confirm checkout", &dialog);
  title->setStyleSheet("color: #3730a3; font-size: 22px; font-weight: 700;");
  layout->addWidget(title);

  auto *subtitle = new QLabel(
      "Review the payment details before completing this checkout.", &dialog);
  subtitle->setWordWrap(true);
  subtitle->setStyleSheet("color: #64748b; font-size: 13px; font-weight: 500;");
  layout->addWidget(subtitle);

  auto *summaryCard = new QFrame(&dialog);
  summaryCard->setObjectName("checkoutSummaryCard");
  auto *summaryLayout = new QGridLayout(summaryCard);
  summaryLayout->setContentsMargins(16, 14, 16, 14);
  summaryLayout->setHorizontalSpacing(22);
  summaryLayout->setVerticalSpacing(8);

  const auto addSummaryRow = [&summaryLayout, summaryCard](int row,
                                                             const QString &label,
                                                             const QString &value,
                                                             bool emphasize = false) {
    auto *field = new QLabel(label, summaryCard);
    field->setStyleSheet("color: #64748b; font-size: 13px; font-weight: 600;");
    auto *data = new QLabel(value, summaryCard);
    data->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    data->setStyleSheet(emphasize
                            ? "color: #047857; font-size: 16px; font-weight: 700;"
                            : "color: #0f172a; font-size: 13px; font-weight: 700;");
    summaryLayout->addWidget(field, row, 0);
    summaryLayout->addWidget(data, row, 1);
  };
  addSummaryRow(0, "Customer", booking.customerName);
  addSummaryRow(1, "Room", booking.roomNumber);
  addSummaryRow(2, "Payment method", paymentMethodComboBox->currentText());
  addSummaryRow(3, "Total to pay", formatMoney(booking.totalAmount), true);
  summaryLayout->setColumnStretch(1, 1);
  layout->addWidget(summaryCard);

  auto *hint = new QLabel(
      "This will create the bill and make the room available again.", &dialog);
  hint->setStyleSheet("color: #475569; font-size: 12px; font-weight: 500;");
  layout->addWidget(hint);
  layout->addStretch();

  auto *buttonLayout = new QHBoxLayout();
  buttonLayout->addStretch();
  auto *cancelButton = new QPushButton("Cancel", &dialog);
  cancelButton->setObjectName("cancelCheckoutButton");
  auto *confirmDialogButton = new QPushButton("Confirm checkout", &dialog);
  confirmDialogButton->setObjectName("confirmCheckoutButton");
  confirmDialogButton->setDefault(true);
  buttonLayout->addWidget(cancelButton);
  buttonLayout->addWidget(confirmDialogButton);
  layout->addLayout(buttonLayout);

  connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
  connect(confirmDialogButton, &QPushButton::clicked, &dialog,
          &QDialog::accept);

  if (dialog.exec() != QDialog::Accepted)
    return;

  CheckoutService checkoutService;
  const CheckoutResult result = checkoutService.checkout(
      booking.bookingId, paymentMethodComboBox->currentText());
  if (!result.success) {
    QMessageBox::critical(this, "Checkout failed", result.errorMessage);
    return;
  }

  QMessageBox::information(this, "Checkout Success",
                           QString("Checkout processed successfully. Bill #%1 "
                                   "was created and room %2 is now available.")
                               .arg(result.billId)
                               .arg(result.booking.roomNumber));
  loadBookings();
  populateBookingTable(searchEdit->text());
}
