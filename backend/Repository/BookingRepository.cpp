#include "BookingRepository.h"
#include "ServiceItemRepository.h"
#include "cores/Booking/Booking.h"
#include "cores/Customer/Customer.h"
#include "cores/Room/DerivedRooms.h"
#include "cores/Room/Room.h"
#include "cores/Service/ServiceItem.h"
#include "frontend/Observers/Observer.h"
#include <QDateTime>
#include <QDebug>
#include <QSqlQuery>
#include <QVariant>
#include <QtSql/QSqlError>

// hàm để chuyển đổi kiểu enum class sang dạng database string hiểu đc
static QString statusToString(BookingStatus status) {
  switch (status) {
  case BookingStatus::UNCONFIRMED:
    return "UNCONFIRMED";
  case BookingStatus::CHECKED_IN:
    return "CHECKED_IN";
  case BookingStatus::CHECKED_OUT:
    return "CHECKED_OUT";
  default:
    return "UNCONFIRMED";
  }
}

static BookingStatus stringToStatus(const QString &str) {
  if (str == "CHECKED_IN")
    return BookingStatus::CHECKED_IN;
  if (str == "CHECKED_OUT")
    return BookingStatus::CHECKED_OUT;
  return BookingStatus::UNCONFIRMED;
}

static QString depositStatusToString(DepositStatus status) {
  switch (status) {
  case DepositStatus::NONE:
    return "NONE";
  case DepositStatus::HELD:
    return "HELD";
  case DepositStatus::RETURNED:
    return "RETURNED";
  default:
    return "NONE";
  }
}

static DepositStatus stringToDepositStatus(const QString &str) {
  if (str == "HELD")
    return DepositStatus::HELD;
  if (str == "RETURNED")
    return DepositStatus::RETURNED;
  return DepositStatus::NONE;
}

// hàm khởi tạo kết nối đến database và kiểm tra schema
BookingRepository::BookingRepository() { verifySchema(); }

// check coi có tồn tại những cột cần thiết chưa nếu chưa add vào nếu rồi kệ (để
// ko bị lỗi khi add vào database mà thiếu cột)
void BookingRepository::verifySchema() {
  QSqlDatabase db = DatabaseManager::instance().database();
  if (!db.isOpen()) {
    DatabaseManager::instance().open();
  }

  QSqlQuery query(db);
  if (!query.exec("PRAGMA table_info(Bookings)")) {
    qDebug() << "ERROR: Failed to run PRAGMA table_info on Bookings!"
             << query.lastError().text();
    return;
  }

  bool hasType = false;
  bool hasStatus = false;
  bool hasDepositAmount = false;
  bool hasDepositStatus = false;

  // thêm cột để track booking và lifecycle tốt hơn
  while (query.next()) {
    QString name = query.value("name").toString();
    if (name == "booking_type")
      hasType = true;
    else if (name == "status")
      hasStatus = true;
    else if (name == "deposit_amount")
      hasDepositAmount = true;
    else if (name == "deposit_status")
      hasDepositStatus = true;
  }

  QSqlQuery alterQuery(db);
  if (!hasType) {
    if (!alterQuery.exec("ALTER TABLE Bookings ADD COLUMN booking_type TEXT "
                         "DEFAULT 'STANDARD'")) {
      qDebug() << "Failed to add column booking_type:"
               << alterQuery.lastError().text();
    }
  }
  if (!hasStatus) {
    if (!alterQuery.exec("ALTER TABLE Bookings ADD COLUMN status TEXT DEFAULT "
                         "'UNCONFIRMED'")) {
      qDebug() << "Failed to add column status:"
               << alterQuery.lastError().text();
    }
  }
  if (!hasDepositAmount) {
    if (!alterQuery.exec("ALTER TABLE Bookings ADD COLUMN deposit_amount REAL "
                         "DEFAULT 0.0")) {
      qDebug() << "Failed to add column deposit_amount:"
               << alterQuery.lastError().text();
    }
  }
  if (!hasDepositStatus) {
    if (!alterQuery.exec("ALTER TABLE Bookings ADD COLUMN deposit_status TEXT "
                         "DEFAULT 'NONE'")) {
      qDebug() << "Failed to add column deposit_status:"
               << alterQuery.lastError().text();
    }
  }
}

// add các thông tin booking cũ không dùng đến nữa
bool BookingRepository::add(const BookingData &booking) {
  QSqlDatabase db = DatabaseManager::instance().database();

  // 1. Kiểm tra phòng có tồn tại và đang Available không
  if (!booking.roomNumber.isEmpty()) {
    QSqlQuery roomCheck(db);
    roomCheck.prepare("SELECT status FROM ListRooms WHERE room_number = :rm OR "
                      "room_id = :rm");
    roomCheck.bindValue(":rm", booking.roomNumber);
    if (!roomCheck.exec() || !roomCheck.next()) {
      qDebug() << "ERROR: Room number" << booking.roomNumber
               << "does not exist in ListRooms database!";
      return false;
    }
    QString rmStatus = roomCheck.value(0).toString();
    if (rmStatus == "Occupied" || rmStatus == "Maintenance") {
      qDebug() << "ERROR: Room" << booking.roomNumber << "is currently"
               << rmStatus << "and not available for booking!";
      return false;
    }
  }

  // 2. Logic thưởng điểm khi khách quay lại (Returning customer bonus point)
  if (booking.customerId > 0) {
    QSqlQuery historyQuery(db);
    historyQuery.prepare(
        "SELECT COUNT(*) FROM Bookings WHERE customer_id = :cid");
    historyQuery.bindValue(":cid", booking.customerId);
    if (historyQuery.exec() && historyQuery.next()) {
      int previousVisits = historyQuery.value(0).toInt();
      if (previousVisits > 0) {
        qDebug() << "[LOYALTY BONUS] Customer ID:" << booking.customerId
                 << "is a returning guest with" << previousVisits
                 << "previous booking(s). Bonus points applied!";
      }
    }
  }

  QSqlQuery query(db);

  QString bookingStatusStr =
      (booking.depositAmount > 0.0 || booking.depositStatus == "HELD")
          ? "CONFIRMED"
          : "UNCONFIRMED";

  query.prepare(
      "INSERT INTO Bookings (customer_id, room_number, "
      "check_in_time, check_out_time, total_price, booking_type, "
      "status, deposit_amount, deposit_status) "
      "VALUES (:customer_id, :room_number, :check_in, :check_out, "
      ":totalPrice, 'STANDARD', :status, :deposit_amount, :deposit_status)");

  query.bindValue(":customer_id", booking.customerId);
  query.bindValue(":room_number", booking.roomNumber);
  query.bindValue(":check_in", booking.checkInTime);
  query.bindValue(":check_out", booking.checkOutTime);
  query.bindValue(":totalPrice", booking.totalPrice);
  query.bindValue(":status", bookingStatusStr);
  query.bindValue(":deposit_amount", booking.depositAmount);
  query.bindValue(":deposit_status", booking.depositStatus.isEmpty()
                                         ? "NONE"
                                         : booking.depositStatus);

  if (!query.exec()) {
    qDebug() << "ERROR: Khong the ghi data Booking!"
             << query.lastError().text();
    return false;
  }

  return true;
}

// thêm các booking mới
int BookingRepository::add(Booking *booking) {
  if (!booking)
    return -1;

  QSqlDatabase db = DatabaseManager::instance().database();

  // 1. Kiểm tra room number có tồn tại và đang Available không
  QString rmNumber;
  StandardRoomBooking *srbCheck = dynamic_cast<StandardRoomBooking *>(booking);
  if (srbCheck != nullptr && srbCheck->getRoom() != nullptr) {
    rmNumber = QString::fromStdString(srbCheck->getRoom()->getId());
  }

  if (!rmNumber.isEmpty()) {
    QSqlQuery roomCheck(db);
    roomCheck.prepare("SELECT status FROM ListRooms WHERE room_number = :rm OR "
                      "room_id = :rm");
    roomCheck.bindValue(":rm", rmNumber);
    if (!roomCheck.exec() || !roomCheck.next()) {
      qDebug() << "ERROR: Room number/ID" << rmNumber
               << "does not exist in ListRooms database!";
      return -1;
    }
    QString rmStatus = roomCheck.value(0).toString();
    if (rmStatus == "Occupied" || rmStatus == "Maintenance") {
      qDebug() << "ERROR: Room" << rmNumber << "is currently" << rmStatus
               << "and not available for booking!";
      return -1;
    }
  }

  // 2. Logic cộng điểm thưởng khi khách hàng quay lại (Returning customer bonus
  // point)s

  QSqlQuery query(db);

  query.prepare("INSERT INTO Bookings (customer_id, room_number, "
                "check_in_time, check_out_time, total_price, booking_type, "
                "status, deposit_amount, deposit_status) "
                "VALUES (:customer_id, :room_number, :check_in, :check_out, "
                ":total_price, :booking_type, :status, :deposit_amount, "
                ":deposit_status)");

  int customerId = booking->customer ? booking->customer->getId() : 0;
  query.bindValue(":customer_id", customerId);

  // sử dụng dynamic cast để check phân biệt standardRoomBooking vs Walk-in tab
  StandardRoomBooking *srb = dynamic_cast<StandardRoomBooking *>(booking);
  if (srb != nullptr) {
    if (srb->depositAmount > 0.0 || srb->depositStatus == DepositStatus::HELD) {
      booking->status = BookingStatus::CONFIRMED;
    }
    query.bindValue(
        ":room_number",
        srb->getRoom() ? QString::fromStdString(srb->getRoom()->getId()) : "");
    query.bindValue(":check_in", srb->checkInTime.toString(Qt::ISODate));
    query.bindValue(":check_out", srb->checkOutTime.toString(Qt::ISODate));
    query.bindValue(":booking_type", "STANDARD");
    query.bindValue(":deposit_amount", srb->depositAmount);
    query.bindValue(":deposit_status",
                    depositStatusToString(srb->depositStatus));

    if (srb->getRoom()) {
      int baseP = srb->getRoom()->getBasePrice();
      int nights = srb->getNights();
      if (nights <= 0)
        nights = 1;
      if (baseP > 0) {
        booking->totalPrice = baseP * nights;
      }
    }
  } else {
    WalkInTab *wit = dynamic_cast<WalkInTab *>(booking);
    query.bindValue(":room_number",
                    QVariant(QMetaType(QMetaType::QString))); // NULL
    if (wit != nullptr) {
      query.bindValue(":check_in", wit->dateCreated.toString(Qt::ISODate));
    } else {
      query.bindValue(":check_in",
                      QDateTime::currentDateTime().toString(Qt::ISODate));
    }
    query.bindValue(":check_out",
                    QVariant(QMetaType(QMetaType::QString))); // NULL
    query.bindValue(":booking_type", "WALK_IN");
    query.bindValue(":deposit_amount", 0.0);
    query.bindValue(":deposit_status", "NONE");
  }

  query.bindValue(":total_price", booking->totalPrice);
  query.bindValue(":status", statusToString(booking->status));

  if (!query.exec()) {
    qDebug() << "ERROR: Failed to add Booking!" << query.lastError().text();
    return -1;
  }

  int newId = query.lastInsertId().toInt();
  booking->id = newId; // đổi id của booking (vì friend function)
  // lý do là vì mỗi lần thêm hay xoá thì id của booking vào database sẽ dược
  // database generate nên mình đổi lại id phụ thuộc vào database

  // Đồng bộ dữ liệu serviceItems
  for (const auto &item : booking->serviceItems) {
    addServiceItemToBooking(newId, item->getId(), item->getQuantity(),
                            item->getUnitPrice(), item->getNote());
  }

  return newId;
}

// cập nhật lại Booking trong database mỗi khi có sự thay đổi
// aka gọi hàm này nếu Booking* có thay đổi (vd: status ...)
bool BookingRepository::update(Booking *booking) {
  if (!booking)
    return false;

  QSqlDatabase db = DatabaseManager::instance().database();
  QSqlQuery query(db);

  query.prepare("UPDATE Bookings SET customer_id = :customer_id, room_number = "
                ":room_number, "
                "check_in_time = :check_in, check_out_time = :check_out, "
                "total_price = :total_price, "
                "status = :status, deposit_amount = :deposit_amount, "
                "deposit_status = :deposit_status "
                "WHERE id = :id");

  query.bindValue(":id", booking->getId());

  int customerId = booking->customer ? booking->customer->getId() : 0;
  query.bindValue(":customer_id", customerId);

  StandardRoomBooking *srb = dynamic_cast<StandardRoomBooking *>(booking);
  if (srb != nullptr) {
    query.bindValue(
        ":room_number",
        srb->getRoom() ? QString::fromStdString(srb->getRoom()->getId()) : "");
    query.bindValue(":check_in", srb->checkInTime.toString(Qt::ISODate));
    query.bindValue(":check_out", srb->checkOutTime.toString(Qt::ISODate));
    query.bindValue(":deposit_amount", srb->depositAmount);
    query.bindValue(":deposit_status",
                    depositStatusToString(srb->depositStatus));

    if (srb->getRoom()) {
      int baseP = srb->getRoom()->getBasePrice();
      int nights = srb->getNights();
      if (nights <= 0)
        nights = 1;
      if (baseP > 0) {
        booking->totalPrice = baseP * nights;
      }
    }
  } else {
    WalkInTab *wit = dynamic_cast<WalkInTab *>(booking);
    query.bindValue(":room_number", QVariant(QMetaType(QMetaType::QString)));
    if (wit != nullptr) {
      query.bindValue(":check_in", wit->dateCreated.toString(Qt::ISODate));
    } else {
      query.bindValue(":check_in",
                      QDateTime::currentDateTime().toString(Qt::ISODate));
    }
    query.bindValue(":check_out", QVariant(QMetaType(QMetaType::QString)));
    query.bindValue(":deposit_amount", 0.0);
    query.bindValue(":deposit_status", "NONE");
  }

  query.bindValue(":total_price", booking->totalPrice);
  query.bindValue(":status", statusToString(booking->status));

  if (!query.exec()) {
    qDebug() << "ERROR: Failed to update Booking!" << query.lastError().text();
    return false;
  }

  // Đồng bộ bằng cách xoá đi hết và thêm vào lại tại vì sửa trên từng phần tử
  // sẽ phiền và dễ sai
  QSqlQuery delQuery(db);
  delQuery.prepare(
      "DELETE FROM BookingServiceItems WHERE booking_id = :booking_id");
  delQuery.bindValue(":booking_id", booking->getId());
  if (!delQuery.exec()) {
    qDebug() << "ERROR: Failed to clear old service items for booking!"
             << delQuery.lastError().text();
    return false;
  }

  for (const auto &item : booking->serviceItems) {
    if (!addServiceItemToBooking(booking->getId(), item->getId(),
                                 item->getQuantity(), item->getUnitPrice(),
                                 item->getNote())) {
      return false;
    }
  }

  return true;
}

bool BookingRepository::updateBooking(int bookingId, int customerId,
                                      const QString &roomNumber,
                                      const QDateTime &checkIn,
                                      const QDateTime &checkOut,
                                      const QString &statusStr) {
  return updateBooking(bookingId, customerId, roomNumber, checkIn, checkOut,
                       0.0, 0.0, "NONE", statusStr);
}

bool BookingRepository::updateBooking(int bookingId, int customerId,
                                      const QString &roomNumber,
                                      const QDateTime &checkIn,
                                      const QDateTime &checkOut,
                                      double depositInput,
                                      const QString &statusStr) {
  QString depStatus = (depositInput > 0.0) ? "HELD" : "NONE";
  return updateBooking(bookingId, customerId, roomNumber, checkIn, checkOut,
                       0.0, depositInput, depStatus, statusStr);
}

bool BookingRepository::updateBooking(int bookingId, const BookingData &data) {
  QDateTime inDT = QDateTime::fromString(data.checkInTime, Qt::ISODate);
  if (!inDT.isValid()) {
    inDT = QDateTime::fromString(data.checkInTime, "yyyy-MM-dd hh:mm:ss");
  }
  QDateTime outDT = QDateTime::fromString(data.checkOutTime, Qt::ISODate);
  if (!outDT.isValid()) {
    outDT = QDateTime::fromString(data.checkOutTime, "yyyy-MM-dd hh:mm:ss");
  }

  return updateBooking(bookingId, data.customerId, data.roomNumber, inDT, outDT,
                       data.totalPrice, data.depositAmount, data.depositStatus,
                       "");
}

bool BookingRepository::updateBooking(int bookingId, int customerId,
                                      const QString &roomNumber,
                                      const QDateTime &checkIn,
                                      const QDateTime &checkOut,
                                      double totalPrice, double depositAmount,
                                      const QString &depositStatusStr,
                                      const QString &statusStr) {
  QSqlDatabase db = DatabaseManager::instance().database();

  // 1. Resolve real integer Customer ID if customerId string/number was passed
  int realCustomerId = customerId;
  if (customerId > 0) {
    QSqlQuery cq(db);
    cq.prepare("SELECT id FROM Customer WHERE id = :cid OR id_customer = :cid");
    cq.bindValue(":cid", customerId);
    if (cq.exec() && cq.next()) {
      realCustomerId = cq.value(0).toInt();
    }
  }

  // 2. Determine booking type
  QString rmNumber = roomNumber.trimmed();
  QString bookingType = rmNumber.isEmpty() ? "WALK_IN" : "STANDARD";

  // 3. Calculate live current total price (room rate * nights + service total - deposit)
  double roomRate = 0.0;
  if (!rmNumber.isEmpty()) {
    QSqlQuery rq(db);
    rq.prepare("SELECT base_price FROM ListRooms WHERE room_number = :rm OR room_id = :rm");
    rq.bindValue(":rm", rmNumber);
    if (rq.exec() && rq.next()) {
      roomRate = rq.value(0).toDouble();
    }
  }
  int nights = checkIn.daysTo(checkOut);
  if (nights <= 0) nights = 1;
  double roomTotal = roomRate * nights;

  double serviceTotal = 0.0;
  QSqlQuery sq(db);
  sq.prepare("SELECT SUM(final_price) FROM BookingServiceItems WHERE booking_id = :id");
  sq.bindValue(":id", bookingId);
  if (sq.exec() && sq.next()) {
    serviceTotal = sq.value(0).toDouble();
  }

  double finalTotalPrice = qMax(0.0, roomTotal + serviceTotal - depositAmount);

  // 4. Resolve status
  QString finalStatus = statusStr.trimmed();
  if (finalStatus.isEmpty()) {
    QSqlQuery sq(db);
    sq.prepare("SELECT status FROM Bookings WHERE id = :id");
    sq.bindValue(":id", bookingId);
    if (sq.exec() && sq.next()) {
      finalStatus = sq.value(0).toString();
    } else {
      finalStatus = "UNCONFIRMED";
    }
  }
  if ((finalStatus.isEmpty() || finalStatus == "UNCONFIRMED") &&
      (depositAmount > 0.0 || depositStatusStr == "HELD")) {
    finalStatus = "CONFIRMED";
  }

  // 5. Execute SQL Update
  QSqlQuery query(db);
  query.prepare("UPDATE Bookings SET "
                "customer_id = :cid, "
                "room_number = :rm, "
                "check_in_time = :in, "
                "check_out_time = :out, "
                "total_price = :price, "
                "booking_type = :btype, "
                "status = :status, "
                "deposit_amount = :dep_amt, "
                "deposit_status = :dep_st "
                "WHERE id = :id");

  query.bindValue(":cid", realCustomerId > 0
                              ? QVariant(realCustomerId)
                              : QVariant(QMetaType(QMetaType::Int)));
  if (rmNumber.isEmpty()) {
    query.bindValue(":rm", QVariant(QMetaType(QMetaType::QString)));
  } else {
    query.bindValue(":rm", rmNumber);
  }
  query.bindValue(":in",
                  checkIn.isValid()
                      ? checkIn.toString(Qt::ISODate)
                      : QDateTime::currentDateTime().toString(Qt::ISODate));
  query.bindValue(
      ":out",
      checkOut.isValid()
          ? checkOut.toString(Qt::ISODate)
          : QDateTime::currentDateTime().addDays(1).toString(Qt::ISODate));
  query.bindValue(":price", finalTotalPrice);
  query.bindValue(":btype", bookingType);
  query.bindValue(":status", finalStatus);
  query.bindValue(":dep_amt", depositAmount);
  query.bindValue(":dep_st",
                  depositStatusStr.isEmpty() ? "NONE" : depositStatusStr);
  query.bindValue(":id", bookingId);

  if (!query.exec()) {
    qDebug() << "ERROR: Failed to update booking!" << query.lastError().text();
    return false;
  }

  return true;
}

// thường sẽ không remove trừ khi khách 1 lần qua đường
bool BookingRepository::remove(int bookingId) {
  QSqlDatabase db = DatabaseManager::instance().database();

  // Find associated room number before deleting
  QString roomNumber;
  QSqlQuery findRoomQuery(db);
  findRoomQuery.prepare("SELECT room_number FROM Bookings WHERE id = :id");
  findRoomQuery.bindValue(":id", bookingId);
  if (findRoomQuery.exec() && findRoomQuery.next()) {
    roomNumber = findRoomQuery.value(0).toString();
  }

  ServiceItemRepository serviceRepo;
  serviceRepo.removeItemsByBookingId(bookingId);

  // xoá booking
  QSqlQuery query(db);
  query.prepare("DELETE FROM Bookings WHERE id = :id");
  query.bindValue(":id", bookingId);

  if (!query.exec()) {
    qDebug() << "ERROR: Failed to remove Booking!" << query.lastError().text();
    return false;
  }

  // Set room status back to Available
  if (!roomNumber.isEmpty()) {
    QSqlQuery updateRoomQuery(db);
    updateRoomQuery.prepare("UPDATE ListRooms SET status = 'Available' WHERE "
                            "room_id = :rm OR room_number = :rm");
    updateRoomQuery.bindValue(":rm", roomNumber);
    updateRoomQuery.exec();

    HotelEventManager::instance().notifyRoomStatus(RoomEvent{
        roomNumber.toStdString(), RoomStatus::Available,
        QDateTime::currentDateTime().toString(Qt::ISODate).toStdString()});
  }

  return true;
}

// lấy 1 booking cụ thể từ database
// Tái tạo lại Booking từ database và caller sẽ tự quản lý cái ô nhớ Booking*
// này
Booking *BookingRepository::getById(int bookingId, Customer *customer,
                                    Room *room) {
  QSqlDatabase db = DatabaseManager::instance().database();
  QSqlQuery query(db);

  query.prepare(
      "SELECT customer_id, room_number, check_in_time, check_out_time, "
      "total_price, booking_type, status, deposit_amount, deposit_status "
      "FROM Bookings WHERE id = :id");
  query.bindValue(":id", bookingId);

  if (!query.exec() || !query.next()) {
    qDebug() << "ERROR: Failed to fetch Booking by ID!"
             << query.lastError().text();
    return nullptr;
  }

  QString typeStr = query.value("booking_type").toString();
  QString statusStr = query.value("status").toString();
  double totalPrice = query.value("total_price").toDouble();

  QDateTime checkInVal = QDateTime::fromString(
      query.value("check_in_time").toString(), Qt::ISODate);
  QDateTime checkOutVal = QDateTime::fromString(
      query.value("check_out_time").toString(), Qt::ISODate);

  Booking *booking = nullptr;

  if (typeStr == "STANDARD") {
    double depositAmount = query.value("deposit_amount").toDouble();
    QString depStatusStr = query.value("deposit_status").toString();

    StandardRoomBooking *srb = new StandardRoomBooking(
        customer, room, checkInVal, checkOutVal, depositAmount);
    srb->depositStatus = stringToDepositStatus(depStatusStr);
    booking = srb;
  } else {
    booking = new WalkInTab(customer, checkInVal);
  }

  if (booking) {
    booking->id = bookingId;
    booking->status = stringToStatus(statusStr);
    booking->totalPrice = totalPrice;

    // Tải service item từ database vào Booking
    QSqlQuery itemQuery(db);
    itemQuery.prepare(R"(
        SELECT b.item_id, b.quantity, b.customer_note, b.final_price, 
               COALESCE(s.item_name, i.item_name) AS item_name, 
               COALESCE(s.category, i.type) AS category 
        FROM BookingServiceItems b 
        LEFT JOIN ServiceCatalog s ON b.item_id = s.item_id 
        LEFT JOIN Inventory i ON b.item_id = CAST(i.item_id AS TEXT) 
        WHERE b.booking_id = :booking_id
    )");
    itemQuery.bindValue(":booking_id", bookingId);

    if (itemQuery.exec()) {
      while (itemQuery.next()) {
        string itemId = itemQuery.value("item_id").toString().toStdString();
        int qty = itemQuery.value("quantity").toInt();
        string note = itemQuery.value("customer_note").toString().toStdString();
        double finalPrice = itemQuery.value("final_price").toDouble();
        string name = itemQuery.value("item_name").toString().toStdString();
        string category = itemQuery.value("category").toString().toStdString();

        unique_ptr<ServiceItem> item;
        if (category == "Food") {
          item = ServiceItemFactory::createServiceItem(
              ServiceType::FoodOrderItem, itemId, name, finalPrice, qty, note);
        } else if (category == "Minibar") {
          item = ServiceItemFactory::createServiceItem(
              ServiceType::MinibarItem, itemId, name, finalPrice, qty, note);
        } else if (category == "Furniture") {
          item = ServiceItemFactory::createServiceItem(
              ServiceType::FurnitureItem, itemId, name, finalPrice, qty, note);
        } else if (category == "Damage") {
          item = ServiceItemFactory::createServiceItem(
              ServiceType::DamagePenaltyItem, itemId, name, finalPrice, qty,
              note);
        } else {
          item = std::make_unique<ServiceItem>(itemId, name, finalPrice, qty,
                                               note); // Fallback
        }

        if (item) {
          booking->serviceItems.push_back(std::move(item));
        }
      }
    }
  }

  return booking;
}

vector<Booking *> BookingRepository::getAll(const vector<Customer *> &customers,
                                            const vector<Room *> &rooms) {
  vector<Booking *> list;
  QSqlDatabase db = DatabaseManager::instance().database();
  QSqlQuery query(db);

  if (!query.exec("SELECT id, customer_id, room_number FROM Bookings")) {
    qDebug() << "ERROR: Failed to fetch all Booking IDs!"
             << query.lastError().text();
    return list;
  }

  vector<tuple<int, int, string>> records;
  while (query.next()) {
    records.push_back({query.value("id").toInt(),
                       query.value("customer_id").toInt(),
                       query.value("room_number").toString().toStdString()});
  }

  for (const auto &rec : records) {
    int id = std::get<0>(rec);
    int custId = std::get<1>(rec);
    string rmNum = std::get<2>(rec);

    Customer *matchedCust = nullptr;
    for (auto *c : customers) {
      if (c && c->getId() == custId) {
        matchedCust = c;
        break;
      }
    }

    Room *matchedRoom = nullptr;
    for (auto *r : rooms) {
      if (r && r->getId() == rmNum) {
        matchedRoom = r;
        break;
      }
    }

    Booking *booking = getById(id, matchedCust, matchedRoom);
    if (booking) {
      list.push_back(booking);
    }
  }

  return list;
}

// method để lọc dựa trên điều kiện (loại filter, vector chứa customer và room)
vector<Booking *>
BookingRepository::getFiltered(const BookingFilter &filter,
                               const vector<Customer *> &customers,
                               const vector<Room *> &rooms) {

  vector<Booking *> list;
  QSqlDatabase db = DatabaseManager::instance().database();

  QString sql = "SELECT id, customer_id, room_number FROM Bookings";
  vector<QString> conditions;

  // AI kiểm tra thì kêu đổi từ các if riêng lẻ r làm như dưới thì gắn vào
  // vector conditions để ko lỗi hoặc hacking(SQL injection)
  if (filter.customerId != -1) {
    conditions.push_back("customer_id = :customer_id");
  }
  if (!filter.roomNumber.empty()) {
    conditions.push_back("room_number = :room_number");
  }
  if (!filter.status.empty()) {
    conditions.push_back("status = :status");
  }
  if (!filter.bookingType.empty()) {
    conditions.push_back("booking_type = :booking_type");
  }

  if (!conditions.empty()) {
    sql += " WHERE " + conditions[0];
    for (size_t i = 1; i < conditions.size(); ++i) {
      sql += " AND " + conditions[i];
    }
  }

  // nếu lọc bằng customerId
  // sql(Qstring khai báo trên thành)
  //  : SELECT id, customer_id, room_number FROM Bookings WHERE customer_id =
  //  :customer_id nếu còn tiếp thì sẽ thêm AND và thêm điều kiện tiếp

  QSqlQuery query(db);
  query.prepare(sql);

  if (filter.customerId != -1) {
    query.bindValue(":customer_id", filter.customerId);
  }
  if (!filter.roomNumber.empty()) {
    query.bindValue(":room_number", QString::fromStdString(filter.roomNumber));
  }
  if (!filter.status.empty()) {
    query.bindValue(":status", QString::fromStdString(filter.status));
  }
  if (!filter.bookingType.empty()) {
    query.bindValue(":booking_type",
                    QString::fromStdString(filter.bookingType));
  }

  if (!query.exec()) {
    qDebug() << "ERROR: Failed to fetch filtered Bookings!"
             << query.lastError().text();
    return list;
  }

  vector<tuple<int, int, string>> records;
  while (query.next()) {
    records.push_back({query.value("id").toInt(),
                       query.value("customer_id").toInt(),
                       // Qvariant -> Qstring -> std::string
                       query.value("room_number").toString().toStdString()});
  }

  for (const auto &rec : records) {

    // syntax của tuple vẫn để namespace std:: để sợ conflict vs get khác
    int id = std::get<0>(rec);
    int custId = std::get<1>(rec);
    string rmNum = std::get<2>(rec);

    Customer *matchedCust = nullptr;
    for (auto *c : customers) {
      if (c && c->getId() == custId) {
        matchedCust = c;
        break;
      }
    }

    Room *matchedRoom = nullptr;
    for (auto *r : rooms) {
      if (r && r->getId() == rmNum) {
        matchedRoom = r;
        break;
      }
    }

    Booking *booking = getById(id, matchedCust, matchedRoom);
    if (booking) {
      list.push_back(booking);
    }
  }

  return list;
}

bool BookingRepository::addServiceItemToBooking(int bookingId,
                                                const string &itemId,
                                                int quantity, double finalPrice,
                                                const string &note) {
  QSqlDatabase db = DatabaseManager::instance().database();
  QSqlQuery query(db);

  query.prepare("INSERT INTO BookingServiceItems (booking_id, item_id, "
                "quantity, customer_note, final_price) "
                "VALUES (:booking_id, :item_id, :quantity, :customer_note, "
                ":final_price)");

  query.bindValue(":booking_id", bookingId);
  query.bindValue(":item_id", QString::fromStdString(itemId));
  query.bindValue(":quantity", quantity);
  query.bindValue(":customer_note", QString::fromStdString(note));
  query.bindValue(":final_price", finalPrice);

  if (!query.exec()) {
    qDebug() << "ERROR: Failed to insert ServiceItem to booking!"
             << query.lastError().text();
    return false;
  }
  return true;
}
