#pragma once

#include "backend/Manager/CheckoutService.h"

#include <QWidget>

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;

class CheckoutPage : public QWidget {
  Q_OBJECT

public:
  explicit CheckoutPage(QWidget *parent = nullptr);

  void showEvent(QShowEvent *event) override;

private:
  void setupUi();
  void loadBookings();
  void populateBookingTable(const QString &filter = QString());
  void showBookingDetails(int row);
  void clearBookingDetails();
  void showConfirmDialog();

  QVector<CheckoutBookingPreview> bookings;

  QLineEdit *searchEdit = nullptr;
  QTableWidget *bookingTable = nullptr;
  QTableWidget *serviceTable = nullptr;
  QWidget *detailsContainer = nullptr;

  QLabel *bookingIdLabel = nullptr;
  QLabel *customerNameLabel = nullptr;
  QLabel *phoneLabel = nullptr;
  QLabel *roomLabel = nullptr;
  QLabel *roomTypeLabel = nullptr;
  QLabel *checkInLabel = nullptr;
  QLabel *expectedCheckOutLabel = nullptr;
  QLabel *nightsLabel = nullptr;

  QLabel *roomChargeLabel = nullptr;
  QLabel *serviceChargeLabel = nullptr;
  QLabel *discountLabel = nullptr;
  QLabel *depositLabel = nullptr;
  QLabel *totalLabel = nullptr;

  QComboBox *paymentMethodComboBox = nullptr;
  QPushButton *confirmButton = nullptr;
};
