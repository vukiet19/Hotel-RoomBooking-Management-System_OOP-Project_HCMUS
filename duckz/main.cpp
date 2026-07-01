#include "DatabaseManager.h"
#include "DashboardService.h"
#include "BookingRepository.h"
#include "Room.h"

#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include <iomanip>

using namespace std;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	if (DatabaseManager::instance().open())
	{
		Room r;
		r.setTypeRoom("PresidentialSuite");
		r.setBasePrice();

		BookingRepository repo;
		BookingData testData;

		testData.customerId = 1001;
		testData.roomNumber = "Presidential Suite";
		testData.checkInTime = "2026-06-29";
		testData.checkOutTime = "2026-06-30";
		testData.totalPrice = r.getBasePrice();

		repo.add(testData);

		DashboardService ds;
		cout << ds.getTodayBookings() << ' ' << fixed << setprecision(0) << ds.getMonthlyRevenue();

		DatabaseManager::instance().close();
	}
	else
	{
		qDebug() << "ERROR: Khong mo duoc Database!";
	}

	return 0;
}