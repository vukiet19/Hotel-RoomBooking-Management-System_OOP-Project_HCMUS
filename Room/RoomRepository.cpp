// ???: trước mắt là lấy baseRoomPrice từ database
// Hàm này sẽ liên kết với DatabaseManager dể đọc/ghi thông tin lên database

#include "RoomRepository.h"
#include "../Manager/DatabaseManager.h"
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
		// Dịch chuyển con trỏ vào dòng đầu tiên của kết quả tìm được
		if (query.next()) {
			// query.value(0): lấy dữ liệu ở cột đầu tiên - SELECT (base_price)
			// toDouble(): số thực từ SQLite --> double C++
			price = query.value(0).toDouble();
		}
		else {
			qDebug() << "WARNING: Khong tim thay loai phong " << typeRoom;
		}
	}
	else qDebug() << "ERROR: Khong the truy van thong tin gia phong" << query.lastError().text();
	
	return price;
}