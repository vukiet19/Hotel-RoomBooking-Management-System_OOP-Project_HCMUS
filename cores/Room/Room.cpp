#include "Room.h"
#include "backend/Manager/DatabaseManager.h"
#include "frontend/Observers/Observer.h"
#include <QDateTime>
#include <QtSql/QSqlQuery>
#include <QVariant>
#include <string>
using namespace std;
// biến static lưu id
int Room::nextid = 0;

// contructor default
Room::Room() : basePrice(0), number_people(0), status(RoomStatus::Available)
{
    this->id = "R";
    string tmp = to_string(nextid);

    // id có cấu trúc là "R" + 0000 -> 9999

    int zerosNeeded = 4 - static_cast<int>(tmp.size());
    for (int i = 0; i < zerosNeeded; i++)
    {
        this->id += '0';
    }
    this->id += tmp;

    nextid++;

    // Khi khởi tạo phòng thì sẽ là Available
    status = Available;

    // Hàm thông báo khi khởi tạo
    RoomEvent event;
    event.roomId = this->id;
    event.newStatus = this->status;
    event.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();
    HotelEventManager::instance().notifyRoomStatus(event);
}

// contructor (hầu hết giống constructor trên,) chỉ thêm nhập vào roomNumber( tương ứng số phòng)
Room::Room(string roomNumber)
{
    // id có cấu trúc là "R" + 0000 -> 9999
    this->id = "R";
    string tmp = to_string(nextid);

    int zerosNeeded = 4 - static_cast<int>(tmp.size());
    for (int i = 0; i < zerosNeeded; i++)
    {
        this->id += '0';
    }
    this->id += tmp;

    nextid++;

    this->roomNumber = roomNumber;
    this->status = Available; // Phòng được tạo thì sẽ Available

    // Hàm thông báo khi khởi tạo
    RoomEvent event;
    event.roomId = this->id;
    event.newStatus = this->status;
    event.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();
    HotelEventManager::instance().notifyRoomStatus(event);
}

// Destructor

Room::~Room() = default;

// getBasePrice
int Room::getBasePrice() const
{
    if (basePrice > 0)
        return basePrice;

    QSqlDatabase db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        DatabaseManager::instance().open();
    }

    QSqlQuery query(db);

    if (!roomNumber.empty() || !id.empty()) {
        QString rm = QString::fromStdString(!roomNumber.empty() ? roomNumber : id);
        query.prepare("SELECT R.base_price FROM ListRooms L "
                      "JOIN RoomTypeCatalog R ON L.room_type = R.room_type "
                      "WHERE L.room_number = :rm OR L.room_id = :rm");
        query.bindValue(":rm", rm);
        if (query.exec() && query.next() && !query.value(0).isNull() && query.value(0).toInt() > 0) {
            return query.value(0).toInt();
        }
    }

    QString typeStr;
    switch (getType()) {
    case RoomType::Standard:
        typeStr = "Standard";
        break;
    case RoomType::VIP:
        typeStr = "VIP";
        break;
    case RoomType::Presidential:
        typeStr = "Presidential";
        break;
    }

    query.prepare("SELECT base_price FROM RoomTypeCatalog WHERE room_type = :type OR room_type = :upperType");
    query.bindValue(":type", typeStr);
    query.bindValue(":upperType", typeStr.toUpper());
    if (query.exec() && query.next() && !query.value(0).isNull()) {
        return query.value(0).toInt();
    }

    return basePrice;
}

string Room::getRoomNumber() const
{
    return roomNumber;
}
void Room::setRoomNumber(string roomNumber)
{
    this->roomNumber = roomNumber;
}

// setStatus
void Room::setStatus(RoomStatus status)
{
    this->status = status;

    // thêm setStatus để ôm event vào
    RoomEvent event;
    event.roomId = this->id;
    event.newStatus = this->status;
    event.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();
    HotelEventManager::instance().notifyRoomStatus(event);
}

// get status
RoomStatus Room::getStatus() const
{
    return status;
}

// getId
string Room::getId() const
{
    return id;
}

void Room::setId(string id) {
    this->id = id;
}

// void Room::getBill(Customer &a)
//{
//     int room_baseprice = getBasePrice();
//
//     int addition_point = room_baseprice / 1000000;
//     a.setPoint(a.getPoint() + addition_point);
// }
//
// void Room::setBasePrice(int basePrice)
//{
//     this->basePrice = basePrice;
// }

void Room::setBasePrice(int basePrice)
{
    this->basePrice = basePrice;
}

void Room::setNumberPeople(int number_people)
{
    this->number_people = number_people;
}

int Room::getNumberPeople() const { return number_people; }

