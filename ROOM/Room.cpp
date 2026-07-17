// Hàm này dùng để viết các hàm đã được định nghĩa trước từ room.h
//
#include "Room.h"
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

    // Các Observers tương ứng trạng thái phòng

    // Thêm các observer , ở đây xài & tương ứng với địa chỉ cuả Observer và vì hàm addObserver nhận đầu vào là con trỏ (Observer *observer)
    addObserver(new Room_Reserved());
    addObserver(new Room_Occupied());
    addObserver(new Room_Available());
    addObserver(new Room_Maintenance());
    // Hàm thông báo khi có biến đổi
    notify();
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

    addObserver(new Room_Reserved());
    addObserver(new Room_Occupied());
    addObserver(new Room_Available());
    addObserver(new Room_Maintenance());

    notify();
}

// Destructor

Room::~Room()
{
    for (Observer *obs : observers)
    {
        delete obs;
    }
    observers.clear();
}

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

// dùng này dùng cho observer pattern( dùng cho thay đổi status của phòng)
void Room::notify()
{

    for (auto observer : observers)
    {
        if (observer != nullptr)
        {
            observer->Ongoing_status_room(id, status);
        }
    }
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

int Room::getNumberPeople() const { return number_people; }