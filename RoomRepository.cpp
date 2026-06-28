#include "RoomRepository.h"
#include "DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

double RoomRepository::getRoomPriceData(const QString& typeRoom) {
	double price = 0.0;

	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	query.prepare("SELECT base_price FROM RoomTypeCatalog WHERE room_type = :type");
	query.bindValue(":type", typeRoom);

	if (query.exec()) {
		// Dich chuyen con tro vao dong ket qua dau tien tim duoc
		if (query.next()) {
			// query.value(0): lay du lieu o cot dau tien SELECT (base_price)
			// toDouble(): so thuc tu SQLite --> double C++
			price = query.value(0).toDouble();
		}
		else {
			qDebug() << "WARNING: Khong tim thay loai phong " << typeRoom;
		}
	}
	else qDebug() << "ERROR: Khong the truy van thong tin gia phong" << query.lastError().text();
	
	return price;
}