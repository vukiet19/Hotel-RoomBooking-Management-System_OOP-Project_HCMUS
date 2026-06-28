#include "DerivedRooms.h"

StandardRoom::StandardRoom()
{
}
StandardRoom::StandardRoom(string roomNum)
    : Room(roomNum)
{
}

RoomType StandardRoom::getType() const
{
    return Standard;
}

int StandardRoom::getBasePrice() const
{
    return basePrice;
}

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