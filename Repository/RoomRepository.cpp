// File RoomRepository ~ Trạm trung chuyển: kết nối giữa database và back-end.

#include "../Repository/RoomRepository.h"
#include "../Manager/DatabaseManager.h"
#include "../Room/Room.h"
#include "../Room/DerivedRooms.h"
#include "../Room/Typeroom.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QVariant>

// Helper: Chuyển đổi mã ID dạng số (SQLite INTEGER) sang dạng mã phòng string C++ (ví dụ: 1 -> "R0001")
static string databaseIdToRoomId(int dbId)
{
	string tmp = to_string(dbId);
	string roomId = "R";
	int zerosNeeded = 4 - tmp.size();
	for (int i = 0; i < zerosNeeded; i++)
	{
		roomId += '0';
	}
	roomId += tmp;
	return roomId;
}

// Helper: Chuyển đổi mã phòng string C++ sang dạng số SQLite INTEGER (ví dụ: "R0001" -> 1)
static int roomIdToDatabaseId(const string &roomId)
{
	if (roomId.empty() || roomId[0] != 'R')
		return 0;
	try
	{
		return stoi(roomId.substr(1));
	}
	catch (...)
	{
		return 0;
	}
}

// Helper: Chuyển đổi enum RoomType thành dạng string trong database
static QString typeToString(RoomType type)
{
	switch (type)
	{
	case RoomType::Standard:
		return "StandardRoom";
	case RoomType::VIP:
		return "VIPRoom";
	case RoomType::Presidential:
		return "PresidentialSuite";
	default:
		return "StandardRoom";
	}
}

// Helper: Chuyển đổi string từ database thành enum RoomType
static RoomType stringToType(const QString &str)
{
	if (str == "VIPRoom" || str == "VIP")
		return RoomType::VIP;
	if (str == "PresidentialSuite" || str == "Presidential")
		return RoomType::Presidential;
	return RoomType::Standard;
}

// Helper: Chuyển đổi enum RoomStatus thành dạng string tương ứng trong database
static QString statusToString(RoomStatus status)
{
	switch (status)
	{
	case RoomStatus::Available:
		return "Available";
	case RoomStatus::Reserved:
		return "Reserved";
	case RoomStatus::Occupied:
		return "Occupied";
	case RoomStatus::Maintenance:
		return "Maintenance";
	default:
		return "Available";
	}
}

// Helper: Chuyển đổi string trong database ngược lại thành enum RoomStatus
static RoomStatus stringToStatus(const QString &str)
{
	if (str == "Reserved")
		return RoomStatus::Reserved;
	if (str == "Occupied")
		return RoomStatus::Occupied;
	if (str == "Maintenance")
		return RoomStatus::Maintenance;
	return RoomStatus::Available;
}

// Constructor: Đảm bảo kết nối database và kiểm tra schema
RoomRepository::RoomRepository()
{
	verifySchema();
}

// Kiểm tra schema và trạng thái mở kết nối của database
void RoomRepository::verifySchema()
{
	QSqlDatabase db = DatabaseManager::instance().database();
	if (!db.isOpen())
	{
		DatabaseManager::instance().open();
	}
}

// Hàm lấy giá phòng (giữ lại để tương thích code cũ)
double RoomRepository::getRoomPriceData(const QString &typeRoom)
{
	double price = 0.0;

	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	query.prepare("SELECT base_price FROM RoomTypeCatalog WHERE room_type = :type");
	query.bindValue(":type", typeRoom);

	if (query.exec())
	{
		// Dịch chuyển con trỏ vào dòng đầu tiên của kết quả tìm được
		if (query.next())
		{
			price = query.value(0).toDouble();
		}
		else
		{
			qDebug() << "WARNING: Khong tim thay loai phong " << typeRoom;
		}
	}
	else
	{
		qDebug() << "ERROR: Khong the truy van thong tin gia phong" << query.lastError().text();
	}

	return price;
}

// Thêm một Room mới vào database (bảng ListRooms)
int RoomRepository::add(Room *room)
{
	if (!room)
		return -1;

	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	// Chuẩn bị câu lệnh SQL INSERT sử dụng bind variable
	query.prepare("INSERT INTO ListRooms (room_id, room_number, room_type, status, base_price, number_people) "
				  "VALUES (:room_id, :room_number, :room_type, :status, :base_price, :number_people)");

	query.bindValue(":room_id", roomIdToDatabaseId(room->getId()));
	query.bindValue(":room_number", QString::fromStdString(room->getRoomNumber()));
	query.bindValue(":room_type", typeToString(room->getType()));
	query.bindValue(":status", statusToString(room->getStatus()));
	query.bindValue(":base_price", room->getBasePrice());
	query.bindValue(":number_people", room->getNumberPeople());

	if (!query.exec())
	{
		qDebug() << "ERROR: Failed to add Room!" << query.lastError().text();
		return -1;
	}

	return query.lastInsertId().toInt();
}

// update phòng bằng cách thay đổi những thông số như status,base_price và number_peope
bool RoomRepository::update(Room *room)
{
	if (!room)
		return false;

	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	query.prepare("UPDATE ListRooms SET "
				  "status = :status, base_price = :base_price, number_people = :number_people "
				  "WHERE room_number = :room_number");

	query.bindValue(":status", statusToString(room->getStatus()));
	query.bindValue(":base_price", room->getBasePrice());
	query.bindValue(":number_people", room->getNumberPeople());

	query.bindValue(":room_number", QString::fromStdString(room->getRoomNumber()));

	if (!query.exec())
	{
		qDebug() << "ERROR: Failed to update Room!" << query.lastError().text();
		return false;
	}

	if (query.numRowsAffected() == 0)
	{
		qDebug() << "WARNING: No room found with Room Number:" << QString::fromStdString(room->getRoomNumber());
		return false;
	}

	return true;
}
// Xóa một Room ra khỏi database dựa theo ID dạng chuỗi (ví dụ: "R0001")
bool RoomRepository::remove(const string &roomId)
{
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	query.prepare("DELETE FROM ListRooms WHERE room_number = :room_number");
	query.bindValue(":room_number", QString::fromStdString((roomId)));

	if (!query.exec())
	{
		qDebug() << "ERROR: Failed to remove Room!" << query.lastError().text();
		return false;
	}
	return true;
}

// Tái tạo đối tượng Room (với đúng phân lớp StandardRoom, VipRoom, hoặc PresiRoom) từ database
Room *RoomRepository::getById(const string &roomId)
{
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	query.prepare("SELECT room_id, room_number, room_type, status, base_price, number_people "
				  "FROM ListRooms WHERE room_id = :room_id");
	query.bindValue(":room_id", roomIdToDatabaseId(roomId));

	if (!query.exec() || !query.next())
	{
		qDebug() << "ERROR: Failed to fetch Room by ID!" << query.lastError().text();
		return nullptr;
	}

	string roomNum = query.value("room_number").toString().toStdString();
	QString typeStr = query.value("room_type").toString();
	RoomStatus status = stringToStatus(query.value("status").toString());
	int basePrice = query.value("base_price").toInt();
	int numberPeople = query.value("number_people").toInt();

	Room *room = nullptr;
	// Phân loại kiểu phòng dựa theo room_type để tạo đúng lớp con
	if (typeStr == "StandardRoom" || typeStr == "Standard")
	{
		room = new StandardRoom(roomNum);
	}
	else if (typeStr == "VIPRoom" || typeStr == "VIP")
	{
		room = new VipRoom(roomNum);
	}
	else if (typeStr == "PresidentialSuite" || typeStr == "Presidential")
	{
		room = new PresiRoom(roomNum);
	}
	else
	{
		room = new StandardRoom(roomNum); // Fallback mặc định
	}

	if (room)
	{
		// Gán thủ công giá trị ID tương ứng từ database để duy trì tính nhất quán
		room->setRoomNumber(roomId);
		room->setStatus(status);
		room->setBasePrice(basePrice);
		room->setNumberPeople(numberPeople);
	}

	return room;
}

// Trả về tất cả các Room hiện có trong database
vector<Room *> RoomRepository::getAll()
{
	vector<Room *> list;
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	// Lấy tất cả các khóa room_id trong ListRooms
	if (!query.exec("SELECT room_id FROM ListRooms"))
	{
		qDebug() << "ERROR: Failed to fetch all Room IDs!" << query.lastError().text();
		return list;
	}

	vector<int> ids;
	while (query.next())
	{
		ids.push_back(query.value("room_id").toInt());
	}

	// Lần lượt gọi getById để tái tạo đầy đủ đối tượng
	for (int id : ids)
	{
		string roomIdStr = databaseIdToRoomId(id);
		Room *r = getById(roomIdStr);
		if (r)
		{
			list.push_back(r);
		}
	}

	return list;
}