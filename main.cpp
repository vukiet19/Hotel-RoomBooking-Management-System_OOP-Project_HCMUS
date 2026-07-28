#include "backend/Manager/DatabaseManager.h"
#include "backend/Manager/DashboardService.h"
#include "backend/Repository/BookingRepository.h"
#include "backend/Repository/CustomerRepository.h"
#include "backend/Repository/RoomRepository.h"
#include "backend/Repository/InventoryRepository.h"
#include "cores/Room/Room.h"
#include "cores/Room/DerivedRooms.h"
#include "cores/Booking/Booking.h"
#include "cores/Booking/BookingFactory.h"
#include "cores/Booking/BookingType.h"
#include "cores/Service/InventoryService.h"

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

	return 0;
}
