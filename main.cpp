#include "Manager/DatabaseManager.h"
#include "Manager/DashboardService.h"
#include "Repository/BookingRepository.h"
#include "Repository/CustomerRepository.h"
#include "Room/Room.h"
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

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	std::cout << "--- CHUONG TRINH BAT DAU CHAY ---" << std::endl;

	if (DatabaseManager::instance().open())
	{

		DatabaseManager::instance().close();
	}
	else
	{
		qDebug() << "ERROR: Khong mo duoc Database!";
	}

	return 0;
}