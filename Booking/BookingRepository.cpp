#include "BookingRepository.h"
#include "Booking.h"
#include "../Customer/Customer.h"
#include "../Room/Room.h"
#include "BookingFactory.h"
#include "../Service/ServiceItem.h"
#include <QtSql/QSqlError>
#include <QDebug>
#include <QVariant>

//hàm để chuyển đổi kiểu enum class sang dạng database string hiểu đc
static QString statusToString(BookingStatus status) {
    switch (status) {
        case BookingStatus::UNCONFIRMED: return "UNCONFIRMED";
        case BookingStatus::CHECKED_IN: return "CHECKED_IN";
        case BookingStatus::CHECKED_OUT: return "CHECKED_OUT";
        default: return "UNCONFIRMED";
    }
}

static BookingStatus stringToStatus(const QString& str) {
    if (str == "CHECKED_IN") return BookingStatus::CHECKED_IN;
    if (str == "CHECKED_OUT") return BookingStatus::CHECKED_OUT;
    return BookingStatus::UNCONFIRMED;
}

static QString depositStatusToString(DepositStatus status) {
    switch (status) {
        case DepositStatus::NONE: return "NONE";
        case DepositStatus::HELD: return "HELD";
        case DepositStatus::RETURNED: return "RETURNED";
        default: return "NONE";
    }
}

static DepositStatus stringToDepositStatus(const QString& str) {
    if (str == "HELD") return DepositStatus::HELD;
    if (str == "RETURNED") return DepositStatus::RETURNED;
    return DepositStatus::NONE;
}

//hàm khởi tạo kết nối đến database và kiểm tra schema
BookingRepository::BookingRepository() {
    verifySchema();
}

//check coi có tồn tại những cột cần thiết chưa nếu chưa add vào nếu rồi kệ (để ko bị lỗi khi add vào database mà thiếu cột)
void BookingRepository::verifySchema() {
    QSqlDatabase db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        DatabaseManager::instance().open();
    }
    
    QSqlQuery query(db);
    if (!query.exec("PRAGMA table_info(Bookings)")) {
        qDebug() << "ERROR: Failed to run PRAGMA table_info on Bookings!" << query.lastError().text();
        return;
    }
    
    bool hasType = false;
    bool hasStatus = false;
    bool hasDepositAmount = false;
    bool hasDepositStatus = false;
    
    while (query.next()) {
        QString name = query.value("name").toString();
        if (name == "booking_type") hasType = true;
        else if (name == "status") hasStatus = true;
        else if (name == "deposit_amount") hasDepositAmount = true;
        else if (name == "deposit_status") hasDepositStatus = true;
    }
    
    QSqlQuery alterQuery(db);
    if (!hasType) {
        if (!alterQuery.exec("ALTER TABLE Bookings ADD COLUMN booking_type TEXT DEFAULT 'STANDARD'")) {
            qDebug() << "Failed to add column booking_type:" << alterQuery.lastError().text();
        }
    }
    if (!hasStatus) {
        if (!alterQuery.exec("ALTER TABLE Bookings ADD COLUMN status TEXT DEFAULT 'UNCONFIRMED'")) {
            qDebug() << "Failed to add column status:" << alterQuery.lastError().text();
        }
    }
    if (!hasDepositAmount) {
        if (!alterQuery.exec("ALTER TABLE Bookings ADD COLUMN deposit_amount REAL DEFAULT 0.0")) {
            qDebug() << "Failed to add column deposit_amount:" << alterQuery.lastError().text();
        }
    }
    if (!hasDepositStatus) {
        if (!alterQuery.exec("ALTER TABLE Bookings ADD COLUMN deposit_status TEXT DEFAULT 'NONE'")) {
            qDebug() << "Failed to add column deposit_status:" << alterQuery.lastError().text();
        }
    }
}

//add các thông tin booking cũ không dùng đến nữa 
bool BookingRepository::add(const BookingData& booking) {
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	query.prepare("INSERT INTO Bookings (customer_id, room_number, check_in_time, check_out_time, total_price, booking_type, status, deposit_amount, deposit_status) "
				  "VALUES (:customer_id, :room_number, :check_in, :check_out, :totalPrice, 'STANDARD', 'UNCONFIRMED', 0.0, 'NONE')");
	
	query.bindValue(":customer_id", booking.customerId);
	query.bindValue(":room_number", booking.roomNumber);
	query.bindValue(":check_in", booking.checkInTime);
	query.bindValue(":check_out", booking.checkOutTime);
	query.bindValue(":totalPrice", booking.totalPrice);

	if (!query.exec()) {
		qDebug() << "ERROR: Khong the ghi data Booking!" << query.lastError().text();
		return false;
	}

	return true;
}

// thêm các booking mới 
int BookingRepository::add(Booking* booking) {
    if (!booking) return -1;
    
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);
    
    query.prepare("INSERT INTO Bookings (customer_id, room_number, check_in_time, check_out_time, total_price, booking_type, status, deposit_amount, deposit_status) "
                  "VALUES (:customer_id, :room_number, :check_in, :check_out, :total_price, :booking_type, :status, :deposit_amount, :deposit_status)");
                  
    int customerId = booking->customer ? booking->customer->getId() : 0;
    query.bindValue(":customer_id", customerId);
    
    //sử dụng dynamic cast để check phân biệt standardRoomBooking vs Walk-in tab
    StandardRoomBooking* srb = dynamic_cast<StandardRoomBooking*>(booking);
    if (srb != nullptr) {
        query.bindValue(":room_number", srb->getRoom() ? QString::fromStdString(srb->getRoom()->getId()) : "");
        query.bindValue(":check_in", srb->checkInTime.toString(Qt::ISODate));
        query.bindValue(":check_out", srb->checkOutTime.toString(Qt::ISODate));
        query.bindValue(":booking_type", "STANDARD");
        query.bindValue(":deposit_amount", srb->depositAmount);
        query.bindValue(":deposit_status", depositStatusToString(srb->depositStatus));
    } else {
        WalkInTab* wit = dynamic_cast<WalkInTab*>(booking);
        query.bindValue(":room_number", QVariant(QVariant::String)); // NULL
        if (wit != nullptr) {
            query.bindValue(":check_in", wit->dateCreated.toString(Qt::ISODate));
        } else {
            query.bindValue(":check_in", QDateTime::currentDateTime().toString(Qt::ISODate));
        }
        query.bindValue(":check_out", QVariant(QVariant::String)); // NULL
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
    //lý do là vì mỗi lần thêm hay xoá thì id của booking vào database sẽ dược database generate nên mình đổi lại id phụ thuộc vào database
    
    //Đồng bộ dữ liệu serviceItems
    for (const auto& item : booking->serviceItems) {
        addServiceItemToBooking(newId, item->getId(), item->getQuantity(), item->getUnitPrice(), item->getNote());
    }
     
    return newId;
}

//cập nhật lại Booking trong database mỗi khi có sự thay đổi
//aka gọi hàm này nếu Booking* có thay đổi (vd: status ...)
bool BookingRepository::update(Booking* booking) {
    if (!booking) return false;
    
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);
    
    query.prepare("UPDATE Bookings SET customer_id = :customer_id, room_number = :room_number, "
                  "check_in_time = :check_in, check_out_time = :check_out, total_price = :total_price, "
                  "status = :status, deposit_amount = :deposit_amount, deposit_status = :deposit_status "
                  "WHERE id = :id");
                  
    query.bindValue(":id", booking->getId());
    
    int customerId = booking->customer ? booking->customer->getId() : 0;
    query.bindValue(":customer_id", customerId);
    
    StandardRoomBooking* srb = dynamic_cast<StandardRoomBooking*>(booking);
    if (srb != nullptr) {
        query.bindValue(":room_number", srb->getRoom() ? QString::fromStdString(srb->getRoom()->getId()) : "");
        query.bindValue(":check_in", srb->checkInTime.toString(Qt::ISODate));
        query.bindValue(":check_out", srb->checkOutTime.toString(Qt::ISODate));
        query.bindValue(":deposit_amount", srb->depositAmount);
        query.bindValue(":deposit_status", depositStatusToString(srb->depositStatus));
    } else {
        WalkInTab* wit = dynamic_cast<WalkInTab*>(booking);
        query.bindValue(":room_number", QVariant(QVariant::String));
        if (wit != nullptr) {
            query.bindValue(":check_in", wit->dateCreated.toString(Qt::ISODate));
        } else {
            query.bindValue(":check_in", QDateTime::currentDateTime().toString(Qt::ISODate));
        }
        query.bindValue(":check_out", QVariant(QVariant::String));
        query.bindValue(":deposit_amount", 0.0);
        query.bindValue(":deposit_status", "NONE");
    }
    
    query.bindValue(":total_price", booking->totalPrice);
    query.bindValue(":status", statusToString(booking->status));
    
    if (!query.exec()) {
        qDebug() << "ERROR: Failed to update Booking!" << query.lastError().text();
        return false;
    }
    
    //Đồng bộ bằng cách xoá đi hết và thêm vào lại tại vì sửa trên từng phần tử sẽ phiền và dễ sai
    QSqlQuery delQuery(db);
    delQuery.prepare("DELETE FROM BookingServiceItems WHERE booking_id = :booking_id");
    delQuery.bindValue(":booking_id", booking->getId());
    if (!delQuery.exec()) {
        qDebug() << "ERROR: Failed to clear old service items for booking!" << delQuery.lastError().text();
        return false;
    }
    
    for (const auto& item : booking->serviceItems) {
        if (!addServiceItemToBooking(booking->getId(), item->getId(), item->getQuantity(), item->getUnitPrice(), item->getNote())) {
            return false;
        }
    }
    
    return true;
}

//thường sẽ không remove trừ khi khách 1 lần qua đường
bool BookingRepository::remove(int bookingId) {
    QSqlDatabase db = DatabaseManager::instance().database();
    
    //xoá bookingServiceItems trước vì là khoá ngoại(để tránh lỗi liên kết vì Booking và ServiceItem có liên kết)
    QSqlQuery delItemsQuery(db);
    delItemsQuery.prepare("DELETE FROM BookingServiceItems WHERE booking_id = :booking_id");
    delItemsQuery.bindValue(":booking_id", bookingId);
    delItemsQuery.exec();
    
    //xoá booking
    QSqlQuery query(db);
    query.prepare("DELETE FROM Bookings WHERE id = :id");
    query.bindValue(":id", bookingId);
    
    if (!query.exec()) {
        qDebug() << "ERROR: Failed to remove Booking!" << query.lastError().text();
        return false;
    }
    return true;
}

//lấy 1 booking cụ thể từ database
//Tái tạo lại Booking từ database và caller sẽ tự quản lý cái ô nhớ Booking* này
Booking* BookingRepository::getById(int bookingId, Customer* customer, Room* room) {
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);
    
    query.prepare("SELECT customer_id, room_number, check_in_time, check_out_time, total_price, booking_type, status, deposit_amount, deposit_status "
                  "FROM Bookings WHERE id = :id");
    query.bindValue(":id", bookingId);
    
    if (!query.exec() || !query.next()) {
        qDebug() << "ERROR: Failed to fetch Booking by ID!" << query.lastError().text();
        return nullptr;
    }
    
    QString typeStr = query.value("booking_type").toString();
    QString statusStr = query.value("status").toString();
    double totalPrice = query.value("total_price").toDouble();
    
    QDateTime checkInVal = QDateTime::fromString(query.value("check_in_time").toString(), Qt::ISODate);
    QDateTime checkOutVal = QDateTime::fromString(query.value("check_out_time").toString(), Qt::ISODate);
    
    Booking* booking = nullptr;
    
    if (typeStr == "STANDARD") {
        double depositAmount = query.value("deposit_amount").toDouble();
        QString depStatusStr = query.value("deposit_status").toString();
        
        StandardRoomBooking* srb = new StandardRoomBooking(customer, room, checkInVal, checkOutVal, depositAmount);
        srb->depositStatus = stringToDepositStatus(depStatusStr);
        booking = srb;
    } else {
        booking = new WalkInTab(customer, checkInVal);
    }
    
    if (booking) {
        booking->id = bookingId;
        booking->status = stringToStatus(statusStr);
        booking->totalPrice = totalPrice;
        
        //Tải service item từ database vào Booking
        QSqlQuery itemQuery(db);
        itemQuery.prepare("SELECT b.item_id, b.quantity, b.customer_note, b.final_price, s.item_name, s.category "
                          "FROM BookingServiceItems b "
                          "JOIN ServiceCatalog s ON b.item_id = s.item_id "
                          "WHERE b.booking_id = :booking_id");
        itemQuery.bindValue(":booking_id", bookingId);
        
        if (itemQuery.exec()) {
            while (itemQuery.next()) {
                std::string itemId = itemQuery.value("item_id").toString().toStdString();
                int qty = itemQuery.value("quantity").toInt();
                std::string note = itemQuery.value("customer_note").toString().toStdString();
                double finalPrice = itemQuery.value("final_price").toDouble();
                std::string name = itemQuery.value("item_name").toString().toStdString();
                std::string category = itemQuery.value("category").toString().toStdString();
                
                unique_ptr<ServiceItem> item;
                if (category == "Food") {
                    item = ServiceItemFactory::createServiceItem(ServiceType::FoodOrderItem, itemId, name, finalPrice, qty, note);
                } else if (category == "Minibar") {
                    item = ServiceItemFactory::createServiceItem(ServiceType::MinibarItem, itemId, name, finalPrice, qty, note);
                } else if (category == "Furniture") {
                    item = ServiceItemFactory::createServiceItem(ServiceType::FurnitureItem, itemId, name, finalPrice, qty, note);
                } else if (category == "Damage") {
                    item = ServiceItemFactory::createServiceItem(ServiceType::DamagePenaltyItem, itemId, name, finalPrice, qty, note);
                } else {
                    item = std::make_unique<ServiceItem>(itemId, name, finalPrice, qty, note); // Fallback
                }
                
                if (item) {
                    booking->serviceItems.push_back(std::move(item));
                }
            }
        }
    }
    
    return booking;
}

std::vector<Booking*> BookingRepository::getAll(const std::vector<Customer*>& customers, const std::vector<Room*>& rooms) {
    std::vector<Booking*> list;
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);
    
    if (!query.exec("SELECT id, customer_id, room_number FROM Bookings")) {
        qDebug() << "ERROR: Failed to fetch all Booking IDs!" << query.lastError().text();
        return list;
    }
    
    std::vector<std::tuple<int, int, std::string>> records;
    while (query.next()) {
        records.push_back({
            query.value("id").toInt(),
            query.value("customer_id").toInt(),
            query.value("room_number").toString().toStdString()
        });
    }
    
    for (const auto& rec : records) {
        int id = std::get<0>(rec);
        int custId = std::get<1>(rec);
        std::string rmNum = std::get<2>(rec);
        
        Customer* matchedCust = nullptr;
        for (auto* c : customers) {
            if (c && c->getId() == custId) {
                matchedCust = c;
                break;
            }
        }
        
        Room* matchedRoom = nullptr;
        for (auto* r : rooms) {
            if (r && r->getId() == rmNum) {
                matchedRoom = r;
                break;
            }
        }
        
        Booking* booking = getById(id, matchedCust, matchedRoom);
        if (booking) {
            list.push_back(booking);
        }
    }
    
    return list;
}

bool BookingRepository::addServiceItemToBooking(int bookingId, const std::string& itemId, int quantity, double finalPrice, const std::string& note) {
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);
    
    query.prepare("INSERT INTO BookingServiceItems (booking_id, item_id, quantity, customer_note, final_price) "
                  "VALUES (:booking_id, :item_id, :quantity, :customer_note, :final_price)");
                  
    query.bindValue(":booking_id", bookingId);
    query.bindValue(":item_id", QString::fromStdString(itemId));
    query.bindValue(":quantity", quantity);
    query.bindValue(":customer_note", QString::fromStdString(note));
    query.bindValue(":final_price", finalPrice);
    
    if (!query.exec()) {
        qDebug() << "ERROR: Failed to insert ServiceItem to booking!" << query.lastError().text();
        return false;
    }
    return true;
}