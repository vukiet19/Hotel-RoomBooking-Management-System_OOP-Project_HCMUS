#pragma once
#include "Room.h"
using namespace std;
// Note_can_chu_y , Tham khao https://viblo.asia/p/virtual-function-ham-ao-override-va-final-naQZRWRPlvx

// Stadard room
class StandardRoom : public Room
{
public:
    StandardRoom(string roomNum, double basePrice)
        : Room(roomNum, basePrice)
    {
        this->id = "R";
        string tmp = to_string(nextid);

        int zerosNeeded = 4 - tmp.size();
        for (int i = 0; i < zerosNeeded; i++)
        {
            this->id += '0';
        }
        this->id += tmp;
    }

    RoomType getType() const override
    {
        return Standard;
    }

    int getBasePrice() const override
    {
        return basePrice * 1.0;
    }
};

class VIPROOM : public Room
{
public:
    VIPROOM(string roomNum, double basePrice)
        : Room(roomNum, basePrice)
    {
    }

    RoomType getType() const override
    {
        return VIP;
    }

    int getBasePrice() const override
    {
        return basePrice * 1.0;
    }
};

class PresiROOM : public Room
{
public:
    PresiROOM(string roomNum, double basePrice)
        : Room(roomNum, basePrice)
    {
    }

    RoomType getType() const override
    {
        return Presidential;
    }

    int getBasePrice() const override
    {
        return basePrice * 1.0;
    }
};