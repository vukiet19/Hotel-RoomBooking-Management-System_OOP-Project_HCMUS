#include "BookingRepository.h"
#include "DatabaseManager.h"

// Doc du lieu vao bang Bookings trong hotel.db
bool BookingRepository::add(const BookingData& booking) {
	// Lay cong ket noi duy nhat
	QSqlDatabase db = DatabaseManager::instance().database();
	
	// Tao bien query phu trach chay cac lenh ghi data
	QSqlQuery query(db);

	// Tao cac o trong co dau : phia truoc 
	query.prepare("INSERT INTO Bookings (customer_id, room_number, check_in_time, check_out_time, total_price) "
				  "VALUES (:customer_id, :room_number, :check_in, :check_out, :totalPrice)");
	
	// Dien vao cac o trong da tao
	query.bindValue(":customer_id", booking.customerId);
	query.bindValue(":room_number", booking.roomNumber);
	query.bindValue(":check_in", booking.checkInTime);
	query.bindValue(":check_out", booking.checkOutTime);
	query.bindValue(":totalPrice", booking.totalPrice);

	// .exec(): execute - thuc thi gui data vao file
	if (!query.exec()) {
		// Neu khong gui duoc data
		qDebug() << "ERROR: Khong the ghi data Booking!" << query.lastError().text();
		return false;
	}

	return true;
}