#include "Manager/DatabaseManager.h"
#include "Manager/DashboardService.h"
#include "Repository/BookingRepository.h"
#include "Repository/CustomerRepository.h"
#include "Repository/RoomRepository.h"
#include "Customer/Customer.h"
#include "Room/Room.h"
#include "Room/RoomStatus.h"
#include "Room/TypeRoom.h"
#include "Room/DerivedRooms.h"
#include "Booking/Booking.h"
#include "Booking/BookingFactory.h"
#include "Booking/BookingType.h"
#include "Service/InventoryService.h"
#include "Repository/InventoryRepository.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <iostream>
#include <iomanip>

using namespace std;

void printRoomStatus(const string &roomName, Room *r)
{
	cout << roomName << " Status: ";
	switch (r->getStatus())
	{
	case RoomStatus::Available:
		cout << "Available\n";
		break;
	case RoomStatus::Reserved:
		cout << "Reserved\n";
		break;
	case RoomStatus::Occupied:
		cout << "Occupied\n";
		break;
	case RoomStatus::Maintenance:
		cout << "Maintenance\n";
		break;
	}
}

void testRoomBooking()
{
	// Khởi tạo customer
	Customer a("Megumin", "1234567890", "1234567890");

	a.display();

	// Khởi tạo phòng
	StandardRoom r1;

	a.setIdroom(r1.getId());
	cout << a.getIdRoom() << '\n';

	// Setbaseprice
	r1.setBasePrice(10000000);

	// Kiểm tra tier hiện tại của khách hàng
	cout << a.getTier() << '\n';

	// Lúc sau hàm này sẽ là checkout, dùng để tính điểm.
	r1.getBill(a);

	cout << a.getPoint() << '\n';

	cout << a.getTier() << '\n';

	for (int i = 0; i < 10; i++)
	{
		r1.getBill(a);
	}

	cout << a.getPoint() << '\n';

	cout << a.getTier() << '\n';
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	std::cout << "--- CHUONG TRINH BAT DAU CHAY ---" << std::endl;

	if (DatabaseManager::instance().open())
	{
		// Khởi tạo repository để chạy verifySchema() tự động cập nhật cấu trúc database
		CustomerRepository custRepo;
		RoomRepository roomRepo;

		DatabaseManager::instance().close();
	}
	else
	{
		qDebug() << "ERROR: Khong mo duoc Database!";
	}

	// Chạy thử demo test room booking của Room_booking_cus
	std::cout << "\n--- DEMO TEST ROOM BOOKING ---" << std::endl;
	testRoomBooking();

	return 0;
}