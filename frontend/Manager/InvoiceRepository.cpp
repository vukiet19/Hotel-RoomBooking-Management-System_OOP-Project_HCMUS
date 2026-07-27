#include "Invoice.h"
#include "InvoiceRepository.h"
#include "DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QVariant>
#include <QDebug>

bool InvoiceRepository::add(const Invoice& invoice) {
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	//Chuẩn bị Prepared Query để chống SQL Injection
	query.prepare("INSERT INTO Invoices (booking_id, created_at, total_amount, payment_status) "
                  "VALUES (:bookingId, :createdAt, :totalAmount, :paymentStatus)");
	//bindValue này giúp nạp dữ liệu từ đối tượng invoice vào các biến giải lập đã khai báo ở bước trên
	query.bindValue(":bookingId", invoice.getBookingId());
	query.bindValue(":createdAt", invoice.getCreatedAt().toString(Qt::ISODate));
	query.bindValue(":totalAmount", invoice.getTotalAmount());
	query.bindValue(":paymentStatus", invoice.getPaymentStatus());
	
	if (!query.exec()) {
		qDebug() << "Lỗi InvoiceRepository::add " << query.lastError().text();
		return false;
	}
	return true;
}

bool InvoiceRepository::update(const Invoice& invoice) {
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	query.prepare("UPDATE Invoices SET total_amount = :totalAmount, payment_status = :paymentStatus "
		"WHERE id = :id");

	query.bindValue(":id", invoice.getId());
	query.bindValue(":totalAmount", invoice.getTotalAmount());
	query.bindValue(":paymentStatus", invoice.getPaymentStatus());

	if (!query.exec()) {
		qDebug() << "Lỗi InvoiceRepository::update " << query.lastError().text();
		return false;
	}
	return true;
}

Invoice InvoiceRepository::findByBookingId(int bookingId) {
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);
	Invoice invoice;

	query.prepare("SELECT id, created_at, total_amount, payment_status FROM Invoices "
		"WHERE booking_id = :bookingId");
	query.bindValue(":bookingId", bookingId);

	if (query.exec() && query.next()) {
		int id = query.value("id").toInt();
		QDateTime createdAt = QDateTime::fromString(query.value("created_at").toString(), Qt::ISODate);
		double totalAmount = query.value("total_amount").toDouble();
		QString paymentStatus = query.value("payment_status").toString();
		invoice = Invoice(id, createdAt, totalAmount, paymentStatus, bookingId);
	}
	else {
		qDebug() << "Không tìm thấy hóa đơn nào cho Booking ID:" << bookingId;
	}

	return invoice;
}


