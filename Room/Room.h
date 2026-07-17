// Hàm này chứa định nghĩa class và hàm của room
// Các file liên quan Typeroom.h( Dùng để chứa các kiểu phòng),observer.h(Observer pattern), Customer.h(Chủ yếu xài cho việc demo hàm getBill())

#pragma once

#include "RoomStatus.h"
#include "Typeroom.h"
#include <QDateTime>
#include "../Observer.h"
#include "../CUSTOMER/Customer.h"

#include <string>
#include <vector>

using namespace std;

class Room
{
    // protected co y nghia la ham con truy cap dc
protected:
    string id;
    string roomNumber;
    RoomStatus status;
    int basePrice;
    int number_people;

public:
    static int nextid;
    // ham khoi tao

    Room();
    Room(string roomNumber);

    // destructor
    virtual ~Room();

    // get_type
    virtual RoomType getType() const = 0;

    // getbaseprice
    virtual int getBasePrice() const;

    void setBasePrice(int baseprice);

    // setstatus
    void setStatus(RoomStatus status);

    // set room name
    void setRoomNumber(string roomNumber);

    // set number people
    void setNumberPeople(int number_people);

    int getNumberPeople();
    // getId
    string getId() const;

    void getBill(Customer &a);

    RoomStatus getStatus();
};