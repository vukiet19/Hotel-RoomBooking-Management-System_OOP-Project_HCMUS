#include "BillingService.h"
#include "DatabaseManager.h"
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

/*
Nhận vào một object booking, sau đó gọi hàm đọc dịch vụ được dùng thuộc
booking đó. Tương ứng với mỗi object dịch vụ sẽ có một hàm tính tiền riêng (đa hình)
để tính ra được tổng tiền dịch vụ.
*/
double BillingService::calculateServiceCharge(const Booking &booking)
{
	double total = 0.0;

	for (const auto &item : booking.getServiceItems())
	{
		if (item == nullptr)
		{
			continue;
		}
		if (item->getStatus() == ServiceStatus::Cancelled)
		{
			continue;
		}

		total += item->getSubtotal();
	}

	return total;
}

// Invoice BillingService::generateInvoice(const Booking& booking, double baseRoomCharge) {
// 	double serviceCharge = calculateServiceCharge(booking);
// 	double finalTotalPrice = baseRoomCharge + serviceCharge;

// 	Invoice newInvoice(0, QDateTime::currentDateTime(), finalTotalPrice, "Paid", bookingId);
// 	if (invoiceRepo.add(newInvoice)) {
// 		qDebug() << "Đã lưu hóa đơn thành công!";
// 	}
// 	else qDebug() << "Lưu hóa đơn thất bại";

// 	return newInvoice;
// }