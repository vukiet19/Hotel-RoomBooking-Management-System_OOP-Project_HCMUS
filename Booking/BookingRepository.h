// File BookingRepos ~ trạm trung chuyển: kết nối giữa database và back-end: đọc dữ liệu Booking từ file Booking lên database

#pragma once
#include "../Manager/DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QString>
#include <vector>
#include <string>

using namespace std;

class Booking;
class Customer;
class Room;
class ServiceItem;

// Truyen du lieu ve thong tin dat phong vao file hotel.db (for backward compatibility)
struct BookingData {
	int customerId;
	QString roomNumber;
	QString checkInTime;
	QString checkOutTime;
	double totalPrice;
};

// Filter options for Booking search
struct BookingFilter {
	int customerId = -1;             // -1 means no customer filter
	string roomNumber = "";     // Empty means no room number filter
	string status = "";         // Empty means no status filter
	string bookingType = "";    // Empty means no booking type filter
};

class BookingRepository {
	public:
		BookingRepository();

		//các hàm thay đổi database

		bool add(const BookingData& booking); //thêm data cũ (giữ lại để tương thích code cũ)

		int add(Booking* booking); //thêm data mới (chủ yếu dùng hàm này)
		bool update(Booking* booking); //cập nhật lại booking
		bool remove(int bookingId); //xoá booking
		bool addServiceItemToBooking(int bookingId, const string& itemId, int quantity, double finalPrice, const string& note); //thêm serviceItem vào booking
		
		// Reconstructs booking from DB. Note: caller manages the lifecycle of the returned pointer.
		//lấy 1 booking cụ thể từ database
		Booking* getById(int bookingId, Customer* customer, Room* room);
		
		//mình sẽ load lại UI dựa vào vector<Booking*> mới được trả về này(getAll và filter)
		//Trả về tất cả booking, caller quản lý bộ nhớ
		vector<Booking*> getAll(const vector<Customer*>& customers, const vector<Room*>& rooms);

		// Lọc booking dựa trên các điều kiện trong filter
		vector<Booking*> getFiltered(const BookingFilter& filter, const vector<Customer*>& customers, const vector<Room*>& rooms);

	private:
		//kiểm tra schema (cột trong database)
		void verifySchema();
};
