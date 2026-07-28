// File BookingRepos ~ trạm trung chuyển: kết nối giữa database và back-end: đọc dữ liệu Booking từ file Booking lên database

#include "BookingRepository.h"
#include "../Manager/DatabaseManager.h"

// Đọc dữ liệu vào bảng Bookings trong hotel.db
bool BookingRepository::add(const BookingData &booking)
{

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
	if (!query.exec())
	{
		// Nếu không ghi được data
		qDebug() << "ERROR: Khong the ghi data Booking!" << query.lastError().text();
		return false;
	}

	// Lấy ra id tự động tăng vừa tạo của Booking này
	QVariant newBookingId = query.lastInsertId();

	// Chèn một dòng dịch vụ/món ăn khởi tạo vào bảng BookingServiceItems
	QSqlQuery serviceQuery(db);
	serviceQuery.prepare("INSERT INTO BookingServiceItems (booking_id, item_id, quantity, customer_note, final_price) "
						 "VALUES (:bId, :itemId, :qty, :note, :fPrice)");

	serviceQuery.bindValue(":bId", newBookingId);
	serviceQuery.bindValue(":itemId", "F001");			// Mã món mặc định (hoặc truyền từ booking)
	serviceQuery.bindValue(":qty", 1);					// Số lượng mặc định
	serviceQuery.bindValue(":note", "Initial Service"); // Ghi chú mặc định
	serviceQuery.bindValue(":fPrice", 0);				// Giá ban đầu

	if (!serviceQuery.exec())
	{
		qDebug() << "ERROR: Khong the ghi data BookingServiceItems!" << serviceQuery.lastError().text();
		// Vẫn trả về true nếu cần, hoặc false nếu muốn bắt buộc phải ghi thành công cả 2 bảng
		return false;
	}

	return true;
}