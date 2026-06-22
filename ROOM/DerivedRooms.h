#pragma once
#include "Room.h"
using namespace std;
//Note_can_chu_y , Tham khao https://viblo.asia/p/virtual-function-ham-ao-override-va-final-naQZRWRPlvx


//Stadard room
class StandardRoom : public Room {
public:

    StandardRoom(string roomNum, double basePrice)
        : Room(roomNum, basePrice) {
        maxExtraBeds = 1; 
    }


    RoomType getType() const override {
        return RoomType::Standard;
    }

    int getBasePrice() const override {
        return basePrice * 1.0;
    }
};

class VIPROOM : public Room {
public:
    VIPROOM(string roomNum, double basePrice)
        : Room(roomNum, basePrice) {
        this->maxExtraBeds = 2; 
    }

    RoomType getType() const override {
        return RoomType::VIP;
    }

    int getBasePrice() const override {
        return basePrice * 1.2; 
    }
};

class PresiROOM : public Room {
public:
    PresiROOM(string roomNum, double basePrice)
        : Room(roomNum, basePrice) {
        this->maxExtraBeds = 3; 
    }

    RoomType getType() const override {
        return RoomType::Presidential;
    }

    int getBasePrice() const override {
        return basePrice * 2.0; 
    }
};