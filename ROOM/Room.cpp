#include "Room.h"
#include <string>

using namespace std;

int Room::nextid = 0;

// contructor default
Room::Room()
{
    this->id = "R";
    string tmp = to_string(nextid);

    int zerosNeeded = 4 - tmp.size();
    for (int i = 0; i < zerosNeeded; i++)
    {
        this->id += '0';
    }
    this->id += tmp;

    nextid++;
}

void Room::setBasePrice(int basePrice)
{
    this->basePrice = basePrice;
}

// contructor
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
}

// Destructor

Room::~Room() = default;

// getBasePrice
int Room::getBasePrice() const
{
    return basePrice;
}

// addObserver
void Room::addObserver(Observer *observer)
{
    observers.push_back(observer);
}

void Room::setRoomNumber(string roomNumber)
{
    this->roomNumber = roomNumber;
}

// setStatus
void Room::setStatus(RoomStatus status)
{
    this->status = status;
    notify();
}

// getId
string Room::getId() const
{
    return id;
}

// dùng này dùng cho observer pattern( dùng cho thay đổi status của phòng)
void Room::notify()
{
    for (auto observer : observers)
    {
        observer->Ongoing_status_room(id, status);
    }
}