#pragma once

#include <QString>
#include <QVector>

#include <optional>

struct CheckoutServicePreview {
  QString name;
  int quantity = 0;
  double unitPrice = 0.0;
};

struct CheckoutBookingPreview {
  int bookingId = -1;
  int customerId = -1;
  int customerType = 0;
  QString customerName;
  QString phone;
  QString roomId;
  QString roomNumber;
  QString roomType;
  QString checkInDate;
  QString expectedCheckOutDate;
  int nights = 0;
  double roomCharge = 0.0;
  double serviceCharge = 0.0;
  double discount = 0.0;
  double deposit = 0.0;
  double totalAmount = 0.0;
  QVector<CheckoutServicePreview> services;
};

struct CheckoutResult {
  bool success = false;
  QString errorMessage;
  int billId = -1;
  CheckoutBookingPreview booking;
};

class CheckoutService {
public:
  bool prepareSchema(QString *errorMessage = nullptr);
  QVector<CheckoutBookingPreview> getActiveBookings(QString *errorMessage = nullptr);
  CheckoutResult checkout(int bookingId, const QString &paymentMethod);

private:
  bool ensureSchema(QString *errorMessage = nullptr);
  std::optional<CheckoutBookingPreview> loadBooking(int bookingId,
                                                     bool activeOnly,
                                                     QString *errorMessage = nullptr);
};
