// Hàm này dùng để viết các hàm đã được định nghĩa trước từ room.h
//

#include "Room.h"
#include "../Observer.h"

#include <string>

using namespace std;
// biến static lưu id
int Room::nextid = 0;

// contructor default
Room::Room()
{
    this->id = "R";
    string tmp = to_string(nextid);

    // id có cấu trúc là "R" + 0000 -> 9999

    int zerosNeeded = 4 - tmp.size();
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

    int zerosNeeded = 4 - tmp.size();
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
    return basePrice;
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
RoomStatus Room::getStatus()
{
    return status;
}

// getId
string Room::getId() const
{
    return id;
}

void Room::getBill(Customer &a)
{
    int room_baseprice = getBasePrice();

    int addition_point = room_baseprice / 1000000;
    a.setPoint(a.getPoint() + addition_point);
}

void Room::setBasePrice(int basePrice)
{
    this->basePrice = basePrice;
}

void Room::setNumberPeople(int number_people)
{
    this->number_people = number_people;
}

int Room::getNumberPeople() { return number_people; }