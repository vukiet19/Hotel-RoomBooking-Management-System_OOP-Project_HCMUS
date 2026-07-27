// Hàm này dùng cho các hàm đc định nghĩa trong DerivedRoom.h
// Liên quan đến room.h( vì đây là class con của class cha room)

#include "DerivedRooms.h"
// Phòng Standard
StandardRoom::StandardRoom()
{
}
StandardRoom::StandardRoom(string roomNum)
    : Room(roomNum)
{
}

// Hàm getType
RoomType StandardRoom::getType() const
{
    return Standard;
}

// Hàm getBasePrice()
int StandardRoom::getBasePrice() const
{
    return basePrice;
}

// Phòng vip
VipRoom::VipRoom(string roomNum)
    : Room(roomNum)
{
}

RoomType VipRoom::getType() const
{
    return VIP;
}

int VipRoom::getBasePrice() const
{

    return basePrice;
}

// Phòng Presi
PresiRoom::PresiRoom(string roomNum)
    : Room(roomNum)
{
}

RoomType PresiRoom::getType() const
{
    return Presidential;
}

int PresiRoom::getBasePrice() const
{
    return basePrice;
}