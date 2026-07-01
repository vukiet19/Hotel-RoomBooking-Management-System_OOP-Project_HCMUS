#pragma once
#include "Room.h"
#include "TypeRoom.h"
using namespace std;
// Note_can_chu_y , Tham khao https://viblo.asia/p/virtual-function-ham-ao-override-va-final-naQZRWRPlvx

// Stadard room
class StandardRoom : public Room
{
public:
    StandardRoom();

    StandardRoom(string roomNum);

    RoomType getType() const override;

    int getBasePrice() const override;
};

class VipRoom : public Room
{
public:
    VipRoom();
    VipRoom(string roomNum);

    RoomType getType() const override;

    int getBasePrice() const override;
};

class PresiRoom : public Room
{
public:
    PresiRoom();

    PresiRoom(string roomNum);

    RoomType getType() const override;

    int getBasePrice() const override;
};