// File BookingRepos ~ trạm trung chuyển: kết nối giữa database và back-end: đọc dữ liệu Booking từ file Booking lên database

#include "BookingRepository.h"
#include "../Manager/DatabaseManager.h"

// Đọc dữ liệu vào bảng Bookings trong hotel.db
bool BookingRepository::add(const BookingData& booking) {
	// Lấy ra cổng kết nối duy nhất
	QSqlDatabase db = DatabaseManager::instance().database();
	
	// Tạo biến query phụ trách chạy các lệnh ghi data
	QSqlQuery query(db);

	// Tạo các ô trống có dấu : phía trước 
	query.prepare("INSERT INTO Bookings (customer_id, room_number, check_in_time, check_out_time, total_price) "
				  "VALUES (:customer_id, :room_number, :check_in, :check_out, :totalPrice)");
	
	// Điền vào các ô trống đã tạo
	query.bindValue(":customer_id", booking.customerId);
	query.bindValue(":room_number", booking.roomNumber);
	query.bindValue(":check_in", booking.checkInTime);
	query.bindValue(":check_out", booking.checkOutTime);
	query.bindValue(":totalPrice", booking.totalPrice);

	// .exec(): execute - thực thi lệnh ghi data vào file database
	if (!query.exec()) {
		// Nếu không ghi được data
		qDebug() << "ERROR: Khong the ghi data Booking!" << query.lastError().text();
		return false;
	}

	return true;
}