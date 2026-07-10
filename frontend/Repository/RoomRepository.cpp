// ???: trước mắt là lấy baseRoomPrice từ database
// Hàm này sẽ liên kết với DatabaseManager dể đọc/ghi thông tin lên database

#include "Repository/RoomRepository.h"
#include "Room/Room.h"
#include "Manager/DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include "Room/DerivedRooms.h"
#include <QDebug>

double RoomRepository::getRoomPriceData(const QString &typeRoom)
{
	double price = 0.0;

	QSqlDatabase db = DatabaseManager::instance().database();
	qDebug() << db.tables();
	QSqlQuery query(db);

	query.prepare("SELECT base_price FROM RoomTypeCatalog WHERE room_type = :type");
	query.bindValue(":type", typeRoom);

	if (query.exec())
	{
		// Dịch chuyển con trỏ vào dòng đầu tiên của kết quả tìm được
		if (query.next())
		{
			// query.value(0): lấy dữ liệu ở cột đầu tiên - SELECT (base_price)
			// toDouble(): số thực từ SQLite --> double C++
			price = query.value(0).toDouble();
		}
		else
		{
			qDebug() << "WARNING: Khong tim thay loai phong " << typeRoom;
		}
	}
	else
		qDebug() << "ERROR: Khong the truy van thong tin gia phong" << query.lastError().text();

	return price;
}

bool RoomRepository::add(const Room &r)
{
	QSqlDatabase db = DatabaseManager::instance().database();

	qDebug() << db.tables();

	QSqlQuery query(db);

	query.prepare("INSERT INTO ListRooms (room_id, room_number, room_type, status, base_price, number_people) "
				  "VALUES (:room_id, :room_number, :room_type, :status, :base_price, :number_people)");

	// if (!isPrepared)
	//{
	//	qDebug() << "CẢNH BÁO: Lỗi cú pháp SQL thật sự là:" << query.lastError().text();
	//	return false;
	// }

	query.bindValue(":room_id", QString::fromStdString(r.getId()));
	query.bindValue(":room_number", QString::fromStdString(r.getRoomNumber()));
	query.bindValue(":room_type", static_cast<int>(r.getType()));
	query.bindValue(":status", static_cast<int>(r.getStatus()));
	query.bindValue(":base_price", r.getBasePrice());
	query.bindValue(":number_people", r.getNumberPeople());

	if (!query.exec())
	{
		qDebug() << "ERROR: Khong the ghi data Room!" << query.lastError().text();
		return false;
	}

	return true;
}

std::vector<Room *> RoomRepository::getRoomsByCapacity(int numPeople)
{
	std::vector<Room *> availableRooms;
	QSqlQuery query;

	query.prepare("SELECT * FROM ListRooms WHERE number_people >= :numPeople AND status = 'Available'");
	query.bindValue(":numPeople", numPeople);

	if (query.exec())
	{
		while (query.next())
		{
			// Create a pointer to the heap
			StandardRoom *r = new StandardRoom();

			// r->setRoomId(query.value("room_id").toString().toStdString());
			r->setRoomNumber(query.value("room_number").toString().toStdString());
			r->setBasePrice(query.value("price").toInt());
			r->setStatus(RoomStatus::Available);

			availableRooms.push_back(r);
		}
	}
	return availableRooms;
}