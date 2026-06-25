#include "DatabaseManager.h"
#include "DashboardService.h"
#include "BookingRepository.h"
#include "Room.h"
#include "BillingService.h"
#include "Invoice.h"
#include "InvoiceRepository.h"
#include "ServiceItem.h"
#include "ServiceItemRepository.h"
#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include <iomanip>
#include <QSqlQuery>
#include <QSqlError>

using namespace std;

int main(int argc, char* argv[]) {
	QCoreApplication a(argc, argv);

	if (DatabaseManager::instance().open()) {
		Room r;
		r.setTypeRoom("PresidentalSuite");
		r.setBasePrice();

		BookingRepository repo;
		BookingData testData;

		testData.customerId = 1001;
		testData.roomNumber = "Presidental Suite";
		testData.checkInTime = "2026-06-23";
		testData.checkOutTime = "2026-06-24";
		testData.totalPrice = r.getBasePrice();
		repo.add(testData);

		int currentBookingId = 1;
		double baseRoomCharge = testData.totalPrice;

		qDebug() << "\nKhach hang (Customer ID:" << testData.customerId <<") o phong"
			<< testData.roomNumber << "yeu cau thanh toan!";
		qDebug() << "Khach hang goi them: 2 Mi goi (M_MI) va 1 Coca (M_COCA)";
		qDebug() << "Dang tinh toan....\n";

		QSqlQuery orderQuery(DatabaseManager::instance().database());
		orderQuery.prepare("INSERT INTO BookingServiceItems (booking_id, item_id, quantity, final_price) "
			"VALUES (:bId, :iId, :qty, 0)");

		orderQuery.bindValue(":bId", currentBookingId);
		orderQuery.bindValue(":iId", "M_MI");
		orderQuery.bindValue(":qty", 2);
		if (!orderQuery.exec()) qDebug() << "Loi ghi Mi: " << orderQuery.lastError().text();

		orderQuery.bindValue(":bId", currentBookingId);
		orderQuery.bindValue(":iId", "M_COCA");
		orderQuery.bindValue(":qty", 1);
		if (!orderQuery.exec()) qDebug() << "Loi ghi Coca: " << orderQuery.lastError().text();

		BillingService accountant;
		Invoice finalInvoice = accountant.generateInvoice(currentBookingId, baseRoomCharge);
		DashboardService ds;
		
		qDebug() << "HOA DON THANH TOAN:";
		qDebug() << "Ma hoa don      :" << finalInvoice.getId();
		qDebug() << "Ma dat phong    :" << finalInvoice.getBookingId();
		qDebug() << "Ngay xuat       :" << finalInvoice.getCreatedAt().toString("dd/MM/yyyy HH:mm:ss");
		qDebug() << "Trang thai      :" << finalInvoice.getPaymentStatus();
		qDebug() << "Tien phong      :" << baseRoomCharge << "\n";
		qDebug() << "Tien dich vu    :" << finalInvoice.getTotalAmount() - baseRoomCharge << "\n";
 		qDebug() << "-------------------------------------------------";
		qDebug() << "TONG TIEN CAN THU:" << QString::number(finalInvoice.getTotalAmount(), 'f', 0) << "VND";

		cout << "Hom nay co: " << ds.getTodayBookings() << " booking." << endl;
		cout << "Doanh thu thang: " << fixed << setprecision(0) << ds.getMonthlyRevenue() << " VND" << endl;
		DatabaseManager::instance().close();
	}
	else {
		qDebug() << "ERROR: Khong mo duoc Database!";
	}

	return 0;
}