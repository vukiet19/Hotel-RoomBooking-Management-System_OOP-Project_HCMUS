#include "CheckoutPage.h"

#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace
{
QString formatMoney(double amount)
{
    return QString("%1 VND").arg(amount, 0, 'f', 0);
}

QLabel *createValueLabel(QWidget *parent)
{
    auto *label = new QLabel("-", parent);
    label->setStyleSheet("color: #1e293b; font-weight: normal;");
    return label;
}
}

CheckoutPage::CheckoutPage(QWidget *parent) : QWidget(parent)
{
    setObjectName("checkoutPage");
    setupUi();
    loadMockBookings();
    populateBookingTable();
    clearBookingDetails();
}

void CheckoutPage::setupUi()
{
    setStyleSheet(R"(
        #checkoutPage {
            background-color: #e8f0fa;
        }

        #checkoutPage QGroupBox {
            background-color: #ffffff;
            color: #334155;
            border: 1px solid #d9e2ec;
            border-radius: 8px;
            margin-top: 12px;
            padding: 14px 10px 10px 10px;
            font-weight: bold;
        }

        #checkoutPage QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
            color: #475569;
            background-color: #ffffff;
        }

        #checkoutPage QScrollArea {
            background-color: transparent;
            border: none;
        }

        #checkoutPage QTableWidget {
            background-color: #ffffff;
            color: #1e293b;
            border: 1px solid #d9e2ec;
            border-radius: 6px;
            gridline-color: #e5edf5;
            selection-background-color: #eff6ff;
            selection-color: #1e3a8a;
            alternate-background-color: #f8fafc;
        }

        #checkoutPage QHeaderView::section {
            background-color: #f8fafc;
            color: #475569;
            border: none;
            border-bottom: 1px solid #d9e2ec;
            padding: 8px;
            font-weight: bold;
        }

        #checkoutPage QTableCornerButton::section {
            background-color: #f8fafc;
            border: none;
            border-bottom: 1px solid #d9e2ec;
        }

        #checkoutPage QComboBox {
            background-color: #ffffff;
            color: #1e293b;
            border: 1px solid #cbd5e1;
            border-radius: 6px;
            padding: 7px 10px;
        }

        #checkoutPage QLabel {
            color: #475569;
        }
    )");

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(12);

    auto *title = new QLabel("Checkout", this);
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #1e3a8a;");
    rootLayout->addWidget(title);

    auto *subtitle = new QLabel(
        "Select a current booking to review the guest information and checkout amount.", this);
    subtitle->setStyleSheet("color: #64748b; font-size: 13px;");
    rootLayout->addWidget(subtitle);

    auto *searchLayout = new QHBoxLayout();
    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Search by booking ID, customer name, phone or room...");
    searchEdit->setStyleSheet(
        "QLineEdit { background: white; border: 1px solid #cbd5e1; border-radius: 6px; padding: 8px; }"
        "QLineEdit:focus { border: 2px solid #3b82f6; }");

    auto *searchButton = new QPushButton("Search", this);
    auto *clearButton = new QPushButton("Clear", this);
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
    bookingTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    bookingTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    bookingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    bookingTable->setMinimumHeight(170);
    bookingTable->setMaximumHeight(220);
    bookingLayout->addWidget(bookingTable);
    rootLayout->addWidget(bookingGroup);

    auto *detailsScroll = new QScrollArea(this);
    detailsScroll->setWidgetResizable(true);
    detailsScroll->setFrameShape(QFrame::NoFrame);
    detailsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    detailsScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    detailsContainer = new QWidget(detailsScroll);
    detailsContainer->setObjectName("checkoutDetailsContainer");
    detailsContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    auto *detailsLayout = new QVBoxLayout(detailsContainer);
    detailsLayout->setContentsMargins(0, 0, 0, 0);
    detailsLayout->setSpacing(12);

    auto *topDetailsLayout = new QHBoxLayout();
    topDetailsLayout->setSpacing(12);

    auto *bookingInfoGroup = new QGroupBox("Booking information", detailsContainer);
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

    bookingInfoLayout->addRow("Booking ID:", bookingIdLabel);
    bookingInfoLayout->addRow("Customer:", customerNameLabel);
    bookingInfoLayout->addRow("Phone:", phoneLabel);
    bookingInfoLayout->addRow("Room:", roomLabel);
    bookingInfoLayout->addRow("Room type:", roomTypeLabel);
    bookingInfoLayout->addRow("Check-in:", checkInLabel);
    bookingInfoLayout->addRow("Expected checkout:", expectedCheckOutLabel);
    bookingInfoLayout->addRow("Number of nights:", nightsLabel);
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

    summaryLayout->addRow("Room charge:", roomChargeLabel);
    summaryLayout->addRow("Service charge:", serviceChargeLabel);
    summaryLayout->addRow("Discount:", discountLabel);
    summaryLayout->addRow("Deposit deducted:", depositLabel);
    summaryLayout->addRow("Total to pay:", totalLabel);
    topDetailsLayout->addWidget(summaryGroup, 1);
    detailsLayout->addLayout(topDetailsLayout);

    auto *serviceGroup = new QGroupBox("Services used", detailsContainer);
    auto *serviceLayout = new QVBoxLayout(serviceGroup);
    serviceLayout->setContentsMargins(10, 16, 10, 10);
    serviceTable = new QTableWidget(0, 3, serviceGroup);
    serviceTable->setHorizontalHeaderLabels({"Service", "Quantity", "Amount"});
    serviceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    serviceTable->verticalHeader()->setVisible(false);
    serviceTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    serviceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    serviceTable->setMinimumHeight(100);
    serviceTable->setMaximumHeight(150);
    serviceLayout->addWidget(serviceTable);
    detailsLayout->addWidget(serviceGroup);

    auto *actionsLayout = new QHBoxLayout();
    actionsLayout->setSpacing(10);
    auto *paymentLabel = new QLabel("Payment method:", detailsContainer);
    paymentMethodComboBox = new QComboBox(detailsContainer);
    paymentMethodComboBox->addItems({"Cash", "Card", "Bank transfer"});
    paymentMethodComboBox->setMinimumWidth(150);
    actionsLayout->addWidget(paymentLabel);
    actionsLayout->addWidget(paymentMethodComboBox);
    actionsLayout->addStretch();

    confirmButton = new QPushButton("Confirm checkout", detailsContainer);
    confirmButton->setMinimumSize(190, 38);
    confirmButton->setStyleSheet(
        "QPushButton { background-color: #10b981; color: white; border: none; border-radius: 6px; "
        "padding: 9px 16px; font-weight: bold; }"
        "QPushButton:hover { background-color: #059669; }"
        "QPushButton:disabled { background-color: #cbd5e1; color: #64748b; }");
    actionsLayout->addWidget(confirmButton);
    detailsLayout->addLayout(actionsLayout);

    detailsScroll->setWidget(detailsContainer);
    rootLayout->addWidget(detailsScroll, 1);

    connect(searchButton, &QPushButton::clicked, this, [this]() {
        populateBookingTable(searchEdit->text());
    });
    connect(searchEdit, &QLineEdit::returnPressed, this, [this]() {
        populateBookingTable(searchEdit->text());
    });
    connect(clearButton, &QPushButton::clicked, this, [this]() {
        searchEdit->clear();
        populateBookingTable();
    });
    connect(bookingTable, &QTableWidget::cellClicked, this,
            [this](int row, int) { showBookingDetails(row); });
    connect(confirmButton, &QPushButton::clicked, this,
            &CheckoutPage::showConfirmDialog);
}

void CheckoutPage::loadMockBookings()
{
    CheckoutBookingPreview first;
    first.bookingId = "BK-1001";
    first.customerName = "Nguyen Van An";
    first.phone = "0901234567";
    first.roomNumber = "203";
    first.roomType = "Deluxe Room";
    first.checkInDate = "2026-07-15";
    first.expectedCheckOutDate = "2026-07-18";
    first.nights = 3;
    first.roomCharge = 1500000;
    first.discount = 100000;
    first.deposit = 500000;
    first.services = {
        {"Breakfast", 2, 100000},
        {"Minibar", 1, 50000},
    };

    CheckoutBookingPreview second;
    second.bookingId = "BK-1002";
    second.customerName = "Tran Thi Binh";
    second.phone = "0912345678";
    second.roomNumber = "305";
    second.roomType = "VIP Room";
    second.checkInDate = "2026-07-16";
    second.expectedCheckOutDate = "2026-07-20";
    second.nights = 4;
    second.roomCharge = 3200000;
    second.discount = 0;
    second.deposit = 1000000;
    second.services = {
        {"Airport transfer", 1, 300000},
        {"Laundry", 3, 70000},
    };

    CheckoutBookingPreview third;
    third.bookingId = "BK-1003";
    third.customerName = "Le Minh Cuong";
    third.phone = "0987654321";
    third.roomNumber = "401";
    third.roomType = "Standard Room";
    third.checkInDate = "2026-07-17";
    third.expectedCheckOutDate = "2026-07-19";
    third.nights = 2;
    third.roomCharge = 1200000;
    third.discount = 50000;
    third.deposit = 300000;

    bookings = {first, second, third};
}

void CheckoutPage::populateBookingTable(const QString &filter)
{
    bookingTable->setRowCount(0);
    clearBookingDetails();

    const QString needle = filter.trimmed().toLower();
    for (int index = 0; index < bookings.size(); ++index)
    {
        const auto &booking = bookings.at(index);
        const QString searchable = QString("%1 %2 %3 %4")
                                        .arg(booking.bookingId,
                                             booking.customerName,
                                             booking.phone,
                                             booking.roomNumber)
                                        .toLower();
        if (!needle.isEmpty() && !searchable.contains(needle))
            continue;

        const int row = bookingTable->rowCount();
        bookingTable->insertRow(row);

        auto *bookingIdItem = new QTableWidgetItem(booking.bookingId);
        bookingIdItem->setData(Qt::UserRole, index);
        bookingTable->setItem(row, 0, bookingIdItem);
        bookingTable->setItem(row, 1, new QTableWidgetItem(booking.customerName));
        bookingTable->setItem(row, 2, new QTableWidgetItem(booking.roomNumber));
        bookingTable->setItem(row, 3, new QTableWidgetItem(booking.checkInDate));
        bookingTable->setItem(row, 4, new QTableWidgetItem(booking.expectedCheckOutDate));
    }

    if (bookingTable->rowCount() > 0)
        bookingTable->selectRow(0);
}

void CheckoutPage::showBookingDetails(int row)
{
    if (row < 0 || !bookingTable->item(row, 0))
        return;

    const int index = bookingTable->item(row, 0)->data(Qt::UserRole).toInt();
    if (index < 0 || index >= bookings.size())
        return;

    const auto &booking = bookings.at(index);
    detailsContainer->setEnabled(true);

    bookingIdLabel->setText(booking.bookingId);
    customerNameLabel->setText(booking.customerName);
    phoneLabel->setText(booking.phone);
    roomLabel->setText(booking.roomNumber);
    roomTypeLabel->setText(booking.roomType);
    checkInLabel->setText(booking.checkInDate);
    expectedCheckOutLabel->setText(booking.expectedCheckOutDate);
    nightsLabel->setText(QString::number(booking.nights));

    serviceTable->setRowCount(0);
    for (const auto &service : booking.services)
    {
        const int serviceRow = serviceTable->rowCount();
        serviceTable->insertRow(serviceRow);
        serviceTable->setItem(serviceRow, 0, new QTableWidgetItem(service.name));
        serviceTable->setItem(serviceRow, 1, new QTableWidgetItem(QString::number(service.quantity)));
        serviceTable->setItem(serviceRow, 2,
                              new QTableWidgetItem(formatMoney(service.quantity * service.unitPrice)));
    }

    roomChargeLabel->setText(formatMoney(booking.roomCharge));
    serviceChargeLabel->setText(formatMoney(serviceCharge(booking)));
    discountLabel->setText(formatMoney(booking.discount));
    depositLabel->setText(formatMoney(booking.deposit));
    totalLabel->setText(formatMoney(totalCharge(booking)));
}

void CheckoutPage::clearBookingDetails()
{
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

double CheckoutPage::serviceCharge(const CheckoutBookingPreview &booking) const
{
    double total = 0.0;
    for (const auto &service : booking.services)
        total += service.quantity * service.unitPrice;
    return total;
}

double CheckoutPage::totalCharge(const CheckoutBookingPreview &booking) const
{
    const double total = booking.roomCharge + serviceCharge(booking) -
                         booking.discount - booking.deposit;
    return qMax(0.0, total);
}

void CheckoutPage::showConfirmDialog()
{
    if (!bookingTable->currentItem())
        return;

    const int row = bookingTable->currentRow();
    const int index = bookingTable->item(row, 0)->data(Qt::UserRole).toInt();
    if (index < 0 || index >= bookings.size())
        return;

    const auto &booking = bookings.at(index);
    const QString message = QString(
                                "Customer: %1\nRoom: %2\nTotal amount: %3\nPayment method: %4\n\n"
                                "This is a UI prototype. No database will be changed.")
                                .arg(booking.customerName,
                                     booking.roomNumber,
                                     formatMoney(totalCharge(booking)),
                                     paymentMethodComboBox->currentText());

    if (QMessageBox::question(this, "Confirm checkout", message,
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        QMessageBox::information(this, "Checkout preview",
                                 "Checkout confirmation simulated successfully.\n"
                                 "The booking data has not been saved.");
    }
}
