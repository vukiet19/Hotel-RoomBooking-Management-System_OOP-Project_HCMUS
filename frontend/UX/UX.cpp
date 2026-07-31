// Master MainWindowController Implementation
#include "UX.h"
#include "../UI/UI.h"
#include "backend/Manager/DashboardService.h"
#include "backend/Manager/DatabaseManager.h"
#include "frontend/Observers/QtHotelObserver.h"
#include "frontend/UI/ManagerWindow/Checkout/CheckoutPage.h"
#include "frontend/usercheck/backend.h"
#include <QDialog>
#include <QFormLayout>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>
#include <QVBoxLayout>

MainWindowController::MainWindowController(QWidget *parent)
    : MainWindowUi(parent) {
  // Gọi UI
  MainWindowUi::setupUi();
  initConnections();
}

// Hàm link kết nối
void MainWindowController::initConnections() {
  connect(buttonBooking, &QPushButton::clicked, this,
          &MainWindowController::showBookingTab);
  connect(buttonCustomer, &QPushButton::clicked, this,
          &MainWindowController::showCustomerTab);
  connect(buttonRoom, &QPushButton::clicked, this,
          &MainWindowController::showRoomTab);
  connect(buttonService, &QPushButton::clicked, this,
          &MainWindowController::showFoodTab);
  connect(buttonInventory, &QPushButton::clicked, this,
          &MainWindowController::showInventoryTab);
  connect(buttonBill, &QPushButton::clicked, this,
          &MainWindowController::showBillTab);
  connect(buttonCheckout, &QPushButton::clicked, this,
          &MainWindowController::handleCheckout);
  connect(buttonDashboard, &QPushButton::clicked, this,
          &MainWindowController::handleDashboardTab);

  connect(bookingPage->bookingTabButton(), &QPushButton::clicked, this,
          &MainWindowController::showBookingTab);
  connect(bookingPage->servicesTabButton(), &QPushButton::clicked, this,
          &MainWindowController::showBookingServicesTab);
  connect(roomPage->roomTabButton(), &QPushButton::clicked, this,
          &MainWindowController::showRoomTab);
  connect(roomPage->roomTypeTabButton(), &QPushButton::clicked, this,
          &MainWindowController::showRoomTypeTab);
  connect(servicePage->foodTabButton(), &QPushButton::clicked, this,
          &MainWindowController::showFoodTab);
  connect(servicePage->serviceTabButton(), &QPushButton::clicked, this,
          &MainWindowController::showServiceTab);
  connect(inventoryPage->inventoryTabButton(), &QPushButton::clicked, this,
          &MainWindowController::showInventoryTab);
  connect(inventoryPage->inventoryLogTabButton(), &QPushButton::clicked, this,
          &MainWindowController::showInventoryLogTab);

  // Connect Observer events to Qt UI slots
  connect(&QtHotelObserver::instance(), &QtHotelObserver::roomStatusChanged,
          this, &MainWindowController::onRoomStatusObserved);
  connect(&QtHotelObserver::instance(), &QtHotelObserver::bookingStatusChanged,
          this, &MainWindowController::onBookingStatusObserved);
}

// Hàm này sẽ thể hiện nút mà đang hiện
void MainWindowController::setActiveButton(QPushButton *clickedButton) {
  QList<QPushButton *> buttons = {buttonBooking,  buttonCustomer,  buttonRoom,
                                  buttonService,  buttonInventory, buttonBill,
                                  buttonCheckout, buttonDashboard};

  for (QPushButton *btn : buttons) {
    if (btn == clickedButton) {
      btn->setProperty("active", true);
    } else {
      btn->setProperty("active", false);
    }

    btn->style()->unpolish(btn);
    btn->style()->polish(btn);
  }
}

void MainWindowController::setActionBarVisible(bool visible) {
  if (actionBar) {
    actionBar->setVisible(visible);
  }
}

// Hàm checkout
void MainWindowController::handleCheckout() {
  setActionBarVisible(false);
  stackedWidget->setCurrentWidget(checkoutPage);
  setActiveButton(buttonCheckout);
}

// Dashboard
void MainWindowController::handleDashboardTab() {
  qDebug() << "[DEBUG] handleDashboardTab - Switched to Dashboard tab";
  setActionBarVisible(false);
  stackedWidget->setCurrentIndex(DashboardIndex);
  setActiveButton(buttonDashboard);
  btnAdd->setVisible(false);
  btnUpdate->setVisible(false);
  btnDelete->setVisible(false);
  btnFilter->setVisible(false);
  btnAdd->disconnect();
  btnUpdate->disconnect();
  btnDelete->disconnect();
  btnFilter->disconnect();

  connect(btnFilter, &QPushButton::clicked, this,
          &MainWindowController::showFilterDashboardDialog);

  DashboardService ds;

  int todayBookings = ds.getTodayBookings();
  double dailyRevenue = ds.getRevenue("day");
  double monthlyRevenue = ds.getRevenue("month");
  double yearlyRevenue = ds.getRevenue("year");

  qDebug() << "[DEBUG] Today's Bookings:" << todayBookings
           << "| Daily:" << dailyRevenue << "| Monthly:" << monthlyRevenue
           << "| Yearly:" << yearlyRevenue;

  lblTodayBookings->setText(QString::number(todayBookings));
  lblDailyRevenue->setText(QString::number(dailyRevenue, 'f', 2) + " VND");
  lblMonthlyRevenue->setText(QString::number(monthlyRevenue, 'f', 2) + " VND");
  lblYearlyRevenue->setText(QString::number(yearlyRevenue, 'f', 2) + " VND");

  std::vector<BookingRevenue> data =
      ds.getBookingRevenues("2026-01-01", "2026-12-31");
  qDebug() << "[DEBUG] Revenues table row count:" << data.size();

  tableDashboard->setRowCount(0);
  int row = 0;
  for (const auto &record : data) {
    tableDashboard->insertRow(row);
    tableDashboard->setItem(
        row, 0, new QTableWidgetItem(QString::number(record.bookingId)));
    tableDashboard->setItem(row, 1, new QTableWidgetItem(record.customerName));
    tableDashboard->setItem(
        row, 2, new QTableWidgetItem(QString::number(record.revenue, 'f', 2)));
    tableDashboard->setItem(row, 3, new QTableWidgetItem(record.checkIn));
    row++;
  }
}

void MainWindowController::showFilterDashboardDialog() {
  QDialog *dialog = new QDialog(this);
  dialog->setWindowTitle("Filter Dashboard Revenues");
  dialog->setFixedSize(400, 220);

  dialog->setStyleSheet("QDialog { background-color: white; }"
                        "QLabel { color: #333333; font-weight: bold; }");

  QVBoxLayout *layout = new QVBoxLayout(dialog);
  QFormLayout *form = new QFormLayout();

  QString inputStyle = "QLineEdit { border: 1px solid #cbd5e1; border-radius: "
                       "4px; padding: 6px; }";

  QLineEdit *txtStartDate = new QLineEdit(dialog);
  txtStartDate->setPlaceholderText("YYYY-MM-DD");
  txtStartDate->setText("2026-01-01");
  txtStartDate->setStyleSheet(inputStyle);

  QLineEdit *txtEndDate = new QLineEdit(dialog);
  txtEndDate->setPlaceholderText("YYYY-MM-DD");
  txtEndDate->setText("2026-12-31");
  txtEndDate->setStyleSheet(inputStyle);

  form->addRow("Start Date:", txtStartDate);
  form->addRow("End Date:", txtEndDate);
  layout->addLayout(form);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  QPushButton *btnApply = new QPushButton("Apply", dialog);
  QPushButton *btnCancel = new QPushButton("Cancel", dialog);

  btnApply->setStyleSheet(
      "background-color: #3b82f6; color: white; border-radius: 6px; padding: "
      "10px; font-weight: bold;");
  btnCancel->setStyleSheet(
      "background-color: #94a3b8; color: white; border-radius: 6px; padding: "
      "10px; font-weight: bold;");

  buttonLayout->addWidget(btnCancel);
  buttonLayout->addWidget(btnApply);
  layout->addLayout(buttonLayout);

  connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);

  connect(btnApply, &QPushButton::clicked, [=]() {
    qDebug() << "[DEBUG] Filter Dialog - Apply clicked";
    QString startStr = txtStartDate->text();
    QString endStr = txtEndDate->text();

    if (startStr.isEmpty() || endStr.isEmpty()) {
      QMessageBox::warning(this, "Error",
                           "Please enter both Start Date and End Date!");
      return;
    }

    DashboardService ds;
    std::vector<BookingRevenue> data = ds.getBookingRevenues(startStr, endStr);

    tableDashboard->setRowCount(0);
    int row = 0;
    for (const auto &record : data) {
      tableDashboard->insertRow(row);
      tableDashboard->setItem(
          row, 0, new QTableWidgetItem(QString::number(record.bookingId)));
      tableDashboard->setItem(row, 1,
                              new QTableWidgetItem(record.customerName));
      tableDashboard->setItem(
          row, 2,
          new QTableWidgetItem(QString::number(record.revenue, 'f', 2)));
      tableDashboard->setItem(row, 3, new QTableWidgetItem(record.checkIn));
      row++;
    }

    QMessageBox::information(dialog, "Success",
                             QString("Filtered %1 records!").arg(data.size()));
    dialog->accept();
  });

  dialog->exec();
  dialog->deleteLater();
}

void MainWindowController::showToastNotification(const QString &title,
                                                 const QString &message) {
  QWidget *toast = new QWidget(this);
  toast->setAttribute(Qt::WA_DeleteOnClose);
  toast->setStyleSheet("background-color: #1e1b4b; "
                       "color: #ffffff; "
                       "border: 2px solid #6366f1; "
                       "border-radius: 10px; "
                       "padding: 10px;");

  QVBoxLayout *layout = new QVBoxLayout(toast);
  layout->setContentsMargins(12, 10, 12, 10);
  layout->setSpacing(4);

  QLabel *lblTitle = new QLabel(QString("🔔  %1").arg(title), toast);
  lblTitle->setStyleSheet(
      "font-weight: bold; font-size: 14px; color: #818cf8;");
  layout->addWidget(lblTitle);

  QLabel *lblMsg = new QLabel(message, toast);
  lblMsg->setStyleSheet("font-size: 13px; color: #f1f5f9;");
  layout->addWidget(lblMsg);

  toast->adjustSize();

  int x = this->width() - toast->width() - 25;
  int y = this->height() - toast->height() - 25;
  toast->move(x, y);
  toast->show();

  QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(toast);
  toast->setGraphicsEffect(effect);

  QTimer::singleShot(3000, [toast, effect]() {
    QPropertyAnimation *anim = new QPropertyAnimation(effect, "opacity", toast);
    anim->setDuration(500);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    QObject::connect(anim, &QPropertyAnimation::finished, toast,
                     &QWidget::close);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
  });
}

void MainWindowController::onRoomStatusObserved(const QString &roomId,
                                                int newStatusInt,
                                                const QString &timestamp) {
  QString statusStr;
  switch (static_cast<RoomStatus>(newStatusInt)) {
  case RoomStatus::Available:
    statusStr = "Available";
    break;
  case RoomStatus::Reserved:
    statusStr = "Reserved";
    break;
  case RoomStatus::Occupied:
    statusStr = "Occupied";
    break;
  case RoomStatus::Maintenance:
    statusStr = "Maintenance";
    break;
  }

  showToastNotification("Room Event Observed",
                        QString("Room %1 is now: %2").arg(roomId, statusStr));

  if (tableRoom) {
    QString queryStr = R"(
            SELECT 
                room_id AS "Room ID",
                room_number AS "Room Number",
                room_type AS "Type",
                status AS "Status",
                base_price AS "Price",
                number_people AS "Number People"
            FROM ListRooms
        )";
    Backend::loadTableData(tableRoom, queryStr);
  }
}

void MainWindowController::onBookingStatusObserved(
    int bookingId, const QString &customerName, const QString &roomNumber,
    int newStatusInt, double totalPrice, const QString &timestamp) {
  QString statusStr;
  switch (static_cast<BookingStatus>(newStatusInt)) {
  case BookingStatus::UNCONFIRMED:
    statusStr = "UNCONFIRMED";
    break;
  case BookingStatus::CONFIRMED:
    statusStr = "CONFIRMED";
    break;
  case BookingStatus::CHECKED_IN:
    statusStr = "CHECKED_IN";
    break;
  case BookingStatus::CHECKED_OUT:
    statusStr = "CHECKED_OUT";
    break;
  }

  showToastNotification("Booking Event Observed",
                        QString("Booking #%1 (%2, Room %3) status: %4")
                            .arg(bookingId)
                            .arg(customerName)
                            .arg(roomNumber)
                            .arg(statusStr));

  if (tableDashboard) {
    tableDashboard->insertRow(0);
    tableDashboard->setItem(0, 0,
                            new QTableWidgetItem(QString::number(bookingId)));
    tableDashboard->setItem(0, 1, new QTableWidgetItem(customerName));
    tableDashboard->setItem(
        0, 2, new QTableWidgetItem(QString::number(totalPrice, 'f', 2)));
    tableDashboard->setItem(0, 3, new QTableWidgetItem(timestamp));

    tableDashboard->selectRow(0);
  }

  DashboardService ds;
  int todayBookings = ds.getTodayBookings();
  double dailyRevenue = ds.getRevenue("day");
  double monthlyRevenue = ds.getRevenue("month");
  double yearlyRevenue = ds.getRevenue("year");

  if (lblTodayBookings)
    lblTodayBookings->setText(QString::number(todayBookings));
  if (lblDailyRevenue)
    lblDailyRevenue->setText(QString::number(dailyRevenue, 'f', 2) + " VND");
  if (lblMonthlyRevenue)
    lblMonthlyRevenue->setText(QString::number(monthlyRevenue, 'f', 2) +
                               " VND");
  if (lblYearlyRevenue)
    lblYearlyRevenue->setText(QString::number(yearlyRevenue, 'f', 2) + " VND");

  if (tableBooking) {
    Backend::loadTableData(tableBooking, "SELECT * FROM Bookings");
  }
}
