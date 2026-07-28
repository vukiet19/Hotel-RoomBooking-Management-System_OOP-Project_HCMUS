#include "BillingService.h"
#include "DatabaseManager.h"
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

double BillingService::calculateServiceCharge(int bookingId)
{
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);
	double totalService = 0.0;

	query.prepare("SELECT SUM(b.quantity * s.base_price) AS final_price "
				  "FROM BookingServiceItems b "
				  "JOIN ServiceCatalog s ON b.item_id = s.item_id "
				  "WHERE b.booking_id = :bookingId");
	query.bindValue(":bookingId", bookingId);

	if (query.exec() && query.next())
	{
		totalService = query.value("final_price").toDouble();
	}
	else
	{
		qDebug() << "Không dùng thêm dịch vụ hoặc lỗi khi tính tiền dịch vụ" << query.lastError().text();
	}

	return totalService;
}

Invoice BillingService::generateInvoice(int bookingId, double baseRoomCharge)
{
	double serviceCharge = calculateServiceCharge(bookingId);
	double finalTotalPrice = baseRoomCharge + serviceCharge;

	Invoice newInvoice(0, QDateTime::currentDateTime(), finalTotalPrice, "Paid", bookingId);
	if (invoiceRepo.add(newInvoice))
	{
		qDebug() << "Đã lưu hóa đơn thành công!";
	}
	else
		qDebug() << "Lưu hóa đơn thất bại";

	return newInvoice;
}