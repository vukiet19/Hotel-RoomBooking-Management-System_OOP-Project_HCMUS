// Hàm này dùng cho observer( nếu có trạng thái đó thfi sẽ báo)
//  Hàm này có liên quan đến Room.h và được thông báo qua hàm notify() của room.h

#include <string>
#include <iostream>
#include "Room.h"
#include "observer.h"
using namespace std;
Observer::~Observer() = default;

// Hàm này dùng cho observer pattern cho các phòng nếu có thay đổi status

void Room_Available::Ongoing_status_room(string roomId, RoomStatus newStatus)
{
    if (newStatus == 0)
    {
        cout << "Room is avaiable\n";
    }
}

void Room_Reserved::Ongoing_status_room(std::string roomId, RoomStatus newStatus)
{
    if (newStatus == 1)
    {
        cout << "Room is reserved\n";
    }
}

void Room_Occupied::Ongoing_status_room(std::string roomId, RoomStatus newStatus)
{
    if (newStatus == 2)
    {
        cout << "Room is Occupied\n";
    }
}

void Room_Maintenance::Ongoing_status_room(std::string roomId, RoomStatus newStatus)
{
    if (newStatus == 3)
    {
        cout << "Room is Maintenance\n";
    }
}