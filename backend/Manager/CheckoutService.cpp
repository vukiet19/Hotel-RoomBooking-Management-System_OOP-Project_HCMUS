#include "CheckoutService.h"

#include "DatabaseManager.h"

#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QSet>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

namespace {

bool tableHasColumn(QSqlDatabase db, const QString &tableName,
                    const QString &columnName) {
  QSqlQuery query(db);
  if (!query.exec("PRAGMA table_info(" + tableName + ")"))
    return false;

  while (query.next()) {
    if (query.value("name").toString() == columnName)
      return true;
  }
  return false;
}

bool addColumnIfMissing(QSqlDatabase db, const QString &tableName,
                        const QString &columnDefinition,
                        const QString &columnName, QString *errorMessage) {
  if (tableHasColumn(db, tableName, columnName))
    return true;

  QSqlQuery query(db);
  if (query.exec("ALTER TABLE " + tableName + " ADD COLUMN " +
                 columnDefinition))
    return true;

  if (errorMessage) {
    *errorMessage = QString("Cannot update %1 schema: %2")
                        .arg(tableName, query.lastError().text());
  }
  return false;
}

QDate parseBookingDate(const QString &value) {
  QDate date = QDate::fromString(value.left(10), Qt::ISODate);
  if (!date.isValid())
    date = QDate::fromString(value.left(10), "yyyy-MM-dd");
  return date;
}

} // namespace

bool CheckoutService::ensureSchema(QString *errorMessage) {
  QSqlDatabase db = DatabaseManager::instance().database();
  if (!db.isOpen() && !DatabaseManager::instance().open()) {
    if (errorMessage)
      *errorMessage = "Cannot open the hotel database.";
    return false;
  }

  // Bookings created before the lifecycle update do not contain checkout state.
  if (!addColumnIfMissing(db, "Bookings", "status TEXT DEFAULT 'UNCONFIRMED'",
                          "status", errorMessage) ||
      !addColumnIfMissing(db, "Bookings", "deposit_amount REAL DEFAULT 0.0",
                          "deposit_amount", errorMessage) ||
      !addColumnIfMissing(db, "Bookings", "deposit_status TEXT DEFAULT 'NONE'",
                          "deposit_status", errorMessage)) {
    return false;
  }

  // Bills keeps a checkout snapshot so later price changes cannot alter
  // history.
  return addColumnIfMissing(db, "Bills", "room_charge REAL DEFAULT 0.0",
                            "room_charge", errorMessage) &&
         addColumnIfMissing(db, "Bills", "service_charge REAL DEFAULT 0.0",
                            "service_charge", errorMessage) &&
         addColumnIfMissing(db, "Bills", "discount_amount REAL DEFAULT 0.0",
                            "discount_amount", errorMessage) &&
         addColumnIfMissing(db, "Bills", "deposit_amount REAL DEFAULT 0.0",
                            "deposit_amount", errorMessage) &&
         addColumnIfMissing(db, "Bills", "payment_method TEXT",
                            "payment_method", errorMessage) &&
         addColumnIfMissing(db, "Bills", "checkout_time TEXT", "checkout_time",
                            errorMessage);
}

bool CheckoutService::prepareSchema(QString *errorMessage) {
  return ensureSchema(errorMessage);
}

std::optional<CheckoutBookingPreview>
CheckoutService::loadBooking(int bookingId, bool activeOnly,
                             QString *errorMessage) {
  QSqlDatabase db = DatabaseManager::instance().database();
  QSqlQuery query(db);
  QString sql = R"(
    SELECT b.id AS booking_id,
           COALESCE(c.id, -1) AS customer_id,
           COALESCE(c.Type, 0) AS customer_type, 
           b.room_number AS booked_room_number,
           b.check_in_time,
           b.check_out_time,
           b.total_price,
           b.deposit_amount,
           b.deposit_status,
           c.full_name,
           COALESCE(c.phone_number, '') AS phone_number,
           r.room_id,
           r.room_number AS room_number,
           COALESCE(r.room_type, 'Standard') AS room_type,
           r.base_price
      FROM Bookings b
      LEFT JOIN Customer c
        ON (c.id_customer = b.customer_id OR c.id = b.customer_id)
      LEFT JOIN ListRooms r
        ON (r.room_number = b.room_number OR r.room_id = b.room_number)
     WHERE b.id = :booking_id
  )";
  if (activeOnly)
    sql += " AND COALESCE(b.status, 'UNCONFIRMED') <> 'CHECKED_OUT'";

  query.prepare(sql);
  query.bindValue(":booking_id", bookingId);
  if (!query.exec()) {
    if (errorMessage)
      *errorMessage = query.lastError().text();
    return std::nullopt;
  }
  if (!query.next())
    return std::nullopt;

  CheckoutBookingPreview booking;
  booking.bookingId = query.value("booking_id").toInt();
  booking.customerId = query.value("customer_id").toInt();
  booking.customerType = query.value("customer_type").toInt();
  booking.customerName = query.value("full_name").toString();
  if (booking.customerName.isEmpty())
    booking.customerName = "Guest #" + QString::number(booking.bookingId);
  booking.phone = query.value("phone_number").toString();
  booking.roomId = query.value("room_id").toString();
  booking.roomNumber = query.value("room_number").toString();
  if (booking.roomNumber.isEmpty())
    booking.roomNumber = query.value("booked_room_number").toString();
  booking.roomType = query.value("room_type").toString();
  if (booking.roomType.isEmpty())
    booking.roomType = "Standard";
  booking.checkInDate = query.value("check_in_time").toString();
  booking.expectedCheckOutDate = query.value("check_out_time").toString();

  const QDate checkIn = parseBookingDate(booking.checkInDate);
  const QDate expectedCheckOut = parseBookingDate(booking.expectedCheckOutDate);
  booking.nights = checkIn.isValid() && expectedCheckOut.isValid()
                       ? qMax(1, checkIn.daysTo(expectedCheckOut))
                       : 1;

  const double currentBasePrice = query.value("base_price").toDouble();
  const double bookedRoomPrice = query.value("total_price").toDouble();
  if (currentBasePrice > 0.0) {
    booking.roomCharge = currentBasePrice * booking.nights;
  } else {
    booking.roomCharge = bookedRoomPrice;
  }

  const QString depositStatus = query.value("deposit_status").toString();
  booking.deposit = depositStatus == "HELD"
                        ? qMax(0.0, query.value("deposit_amount").toDouble())
                        : 0.0;
  // No booking-level discount is persisted in the current schema yet.
  booking.discount = 0.0;

  QSqlQuery servicesQuery(db);
  servicesQuery.prepare(R"(
    SELECT COALESCE(c.item_name, i.item_name, bsi.item_id) AS item_name,
           bsi.quantity,
           bsi.final_price
      FROM BookingServiceItems bsi
      LEFT JOIN ServiceCatalog c ON c.item_id = bsi.item_id
      LEFT JOIN Inventory i ON CAST(i.item_id AS TEXT) = bsi.item_id
     WHERE bsi.booking_id = :booking_id
  )");
  servicesQuery.bindValue(":booking_id", booking.bookingId);
  if (!servicesQuery.exec()) {
    if (errorMessage)
      *errorMessage = servicesQuery.lastError().text();
    return std::nullopt;
  }

  while (servicesQuery.next()) {
    CheckoutServicePreview service;
    service.name = servicesQuery.value("item_name").toString();
    service.quantity = servicesQuery.value("quantity").toInt();

    double finalPrice = servicesQuery.value("final_price").toDouble();
    service.unitPrice =
        service.quantity > 0 ? (finalPrice / service.quantity) : 0.0;

    booking.services.append(service);
    booking.serviceCharge += finalPrice;
  }
  booking.totalAmount = qMax(0.0, booking.roomCharge + booking.serviceCharge -
                                      booking.discount - booking.deposit);

  return booking;
}

QVector<CheckoutBookingPreview>
CheckoutService::getActiveBookings(QString *errorMessage) {
  QVector<CheckoutBookingPreview> bookings;
  if (!ensureSchema(errorMessage))
    return bookings;

  QSqlDatabase db = DatabaseManager::instance().database();
  QSqlQuery idsQuery(db);
  if (!idsQuery.exec(
          "SELECT id FROM Bookings WHERE COALESCE(status, 'UNCONFIRMED') <> "
          "'CHECKED_OUT' ORDER BY id DESC")) {
    if (errorMessage)
      *errorMessage = idsQuery.lastError().text();
    return bookings;
  }

  while (idsQuery.next()) {
    QString bookingError;
    const auto booking =
        loadBooking(idsQuery.value(0).toInt(), true, &bookingError);
    if (booking) {
      bookings.append(*booking);
    } else {
      qDebug() << "Cannot load checkout booking:" << bookingError;
    }
  }
  return bookings;
}

CheckoutResult CheckoutService::checkout(int bookingId,
                                         const QString &paymentMethod) {
  CheckoutResult result;
  if (bookingId <= 0 || paymentMethod.trimmed().isEmpty()) {
    result.errorMessage = "A booking and payment method are required.";
    return result;
  }
  if (!ensureSchema(&result.errorMessage))
    return result;

  const auto booking = loadBooking(bookingId, true, &result.errorMessage);
  if (!booking) {
    if (result.errorMessage.isEmpty())
      result.errorMessage =
          "Booking does not exist or has already been checked out.";
    return result;
  }

  QSqlDatabase db = DatabaseManager::instance().database();
  if (!db.transaction()) {
    result.errorMessage = "Cannot start the checkout transaction.";
    return result;
  }

  auto rollbackWithError = [&db, &result](const QString &message) {
    db.rollback();
    result.errorMessage = message;
  };

  QSqlQuery existingBill(db);
  existingBill.prepare(
      "SELECT 1 FROM Bills WHERE booking_id = :booking_id LIMIT 1");
  existingBill.bindValue(":booking_id", bookingId);
  if (!existingBill.exec()) {
    rollbackWithError(existingBill.lastError().text());
    return result;
  }
  if (existingBill.next()) {
    rollbackWithError("A bill already exists for this booking.");
    return result;
  }

  QSqlQuery insertBill(db);
  insertBill.prepare(R"(
    INSERT INTO Bills (booking_id, total_amount, room_charge, service_charge,
                       discount_amount, deposit_amount, payment_method, checkout_time)
    VALUES (:booking_id, :total_amount, :room_charge, :service_charge,
            :discount_amount, :deposit_amount, :payment_method, :checkout_time)
  )");
  insertBill.bindValue(":booking_id", booking->bookingId);
  insertBill.bindValue(":total_amount", booking->totalAmount);
  insertBill.bindValue(":room_charge", booking->roomCharge);
  insertBill.bindValue(":service_charge", booking->serviceCharge);
  insertBill.bindValue(":discount_amount", booking->discount);
  insertBill.bindValue(":deposit_amount", booking->deposit);
  insertBill.bindValue(":payment_method", paymentMethod.trimmed());
  insertBill.bindValue(":checkout_time", QDateTime::currentDateTime().toString(
                                             "yyyy-MM-dd HH:mm:ss"));
  if (!insertBill.exec()) {
    rollbackWithError(insertBill.lastError().text());
    return result;
  }

  QSqlQuery updateBooking(db);
  updateBooking.prepare("UPDATE Bookings SET status = 'CHECKED_OUT' "
                        "WHERE id = :booking_id AND COALESCE(status, "
                        "'UNCONFIRMED') <> 'CHECKED_OUT'");
  updateBooking.bindValue(":booking_id", booking->bookingId);
  if (!updateBooking.exec() || updateBooking.numRowsAffected() != 1) {
    rollbackWithError(updateBooking.lastError().isValid()
                          ? updateBooking.lastError().text()
                          : "Booking was already checked out.");
    return result;
  }

  QSqlQuery updateRoom(db);
  updateRoom.prepare("UPDATE ListRooms SET status = 'Available' "
                     "WHERE room_number = :room_number OR room_id = :room_id");
  updateRoom.bindValue(":room_id", booking->roomId);
  updateRoom.bindValue(":room_number", booking->roomId);
  if (!updateRoom.exec() || updateRoom.numRowsAffected() < 1) {
    rollbackWithError(updateRoom.lastError().isValid()
                          ? updateRoom.lastError().text()
                          : "The booked room could not be released.");
    return result;
  }

  // Update customer loyalty points and tier
  if (booking->customerType == -1) {
    QSqlQuery updateBookingCustomer(db);
    updateBookingCustomer.prepare(
        "UPDATE Bookings SET customer_id = 0 WHERE id = :booking_id");
    updateBookingCustomer.bindValue(":booking_id", booking->bookingId);
    if (!updateBookingCustomer.exec()) {
      rollbackWithError(updateBookingCustomer.lastError().text());
      return result;
    }

    QSqlQuery deleteCustomer(db);
    deleteCustomer.prepare("DELETE FROM Customer WHERE id = :customer_id");
    deleteCustomer.bindValue(":customer_id", booking->customerId);
    if (!deleteCustomer.exec()) {
      rollbackWithError(deleteCustomer.lastError().text());
      return result;
    }
  } else if (booking->customerId > 0) {
    int bonusPoints = static_cast<int>(booking->totalAmount / 1000000.0);
    if (bonusPoints > 0) {
      int currentPoints = 0;
      int currentTier = 0;
      {
        QSqlQuery getPoints(db);
        getPoints.prepare(
            "SELECT Point, Type FROM Customer WHERE id = :customer_id");
        getPoints.bindValue(":customer_id", booking->customerId);
        if (getPoints.exec() && getPoints.next()) {
          currentPoints = getPoints.value(0).toInt();
          currentTier = getPoints.value(1).toInt();
        }
      }

      // Tính hạng nhảy cóc dựa trên tổng điểm mới
      int newPoints = currentPoints + bonusPoints;
      int calculatedTier = 0; // Default Unknown
      if (newPoints >= 50)
        calculatedTier = 3; // Platinum
      else if (newPoints >= 20)
        calculatedTier = 2; // Gold
      else if (newPoints >= 5)
        calculatedTier = 1; // Silver

      // BẢO VỆ RỚT HẠNG: Hạng chỉ có tăng lên hoặc giữ nguyên
      int newTier = std::max(currentTier, calculatedTier);

      // Cập nhật vào DB an toàn
      QSqlQuery updatePoints(db);
      updatePoints.prepare("UPDATE Customer SET Point = :newPoints, Type = "
                           ":newTier WHERE id = :customer_id");
      updatePoints.bindValue(":newPoints", newPoints);
      updatePoints.bindValue(":newTier", newTier);
      updatePoints.bindValue(":customer_id", booking->customerId);

      if (!updatePoints.exec()) {
        rollbackWithError(updatePoints.lastError().text());
        return result;
      }
    }
  }
  if (!db.commit()) {
    result.errorMessage = "Cannot commit the checkout transaction.";
    return result;
  }

  result.success = true;
  result.billId = insertBill.lastInsertId().toInt();
  result.booking = *booking;
  return result;
}