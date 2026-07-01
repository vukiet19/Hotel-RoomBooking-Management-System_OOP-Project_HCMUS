//This function is used to check available rooms based on the filter criteria provided. 
// The function returns a QVector of Room objects that match the criteria.
#include "RoomFactory.h"
#include "Room.h"
#include "Typeroom.h"
#include "DerivedRooms.h"
#include <QVector>
#include <QString>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "DatabaseManager.h"

// This function checks for available rooms based on the provided filter criteria.
QVector<Room*> RoomFactory::checkAvailableRooms(const RoomFilter& filter) {
	QVector<Room*> availableRoom;
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	QString sqlQueryStr = "SELECT L.room_number, L.room_type, R.base_price, R.capacity "
		"FROM ListRooms L "
		"JOIN RoomTypeCatalog R ON L.room_type = R.room_type "
		"WHERE L.status = 'Available' AND R.capacity >= :minPeople";

	if (!filter.typeRoom.isEmpty()) {
		sqlQueryStr += " AND L.room_type = :typeRoom"; 
	}
	if (filter.maxPriceRoom > 0) {
		sqlQueryStr += " AND R.base_price <= :maxPrice";
	}
	sqlQueryStr += filter.sortByPriceAscending ? " ORDER BY R.base_price ASC" : " ORDER BY R.base_price DESC";
	query.prepare(sqlQueryStr);
	query.bindValue(":minPeople", filter.numberPeople);

	if (!filter.typeRoom.isEmpty()) 
		query.bindValue(":typeRoom", filter.typeRoom);
	if (filter.maxPriceRoom > 0) 
		query.bindValue(":maxPrice", filter.maxPriceRoom);

	if (query.exec()) {
		while (query.next()) {
			if (query.value("room_type").toString() == "StandardRoom") {
				availableRoom.append(new StandardRoom(query.value("room_number").toString().toStdString()));
			}
			else if (query.value("room_type").toString() == "VIPRoom") {
				availableRoom.append(new VipRoom(query.value("room_number").toString().toStdString()));
			}
			else if (query.value("room_type").toString() == "PresidentialSuite") {
				availableRoom.append(new PresiRoom(query.value("room_number").toString().toStdString()));
			}
		}
	}
	return availableRoom;
}