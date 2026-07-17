// Hàm này chứa định nghĩa class và hàm của room
// Các file liên quan Typeroom.h( Dùng để chứa các kiểu phòng),observer.h(Observer pattern), Customer.h(Chủ yếu xài cho việc demo hàm getBill())

#pragma once

#include "RoomStatus.h"
#include "Typeroom.h"
#include "../Observer.h"
#include "../CUSTOMER/Customer.h"
#include <iostream>
#include <string>

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

    friend class RoomRepository;

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

    // Hàm này sẽ add observer cho Room, vì room có 4 status là Avaliable, reserved, Occupied và Maintance nên sẽ chứa là 4 biến tựng chưng observer của từng trạng thái, nếu có trạng thái đó thì observer sẽ báo.
    // void addObserver(Observer *observer);

    void setBasePrice(int baseprice);

    // setstatus
    void setStatus(RoomStatus status);

    // set room name
    void setRoomNumber(string roomNumber);

    string getRoomNumber() const;

    // set number people
    void setNumberPeople(int number_people);

    int getNumberPeople() const;
    // getId
    string getId() const;

    // void getBill(Customer &a) const;

    RoomStatus getStatus() const;

    // Hàm này dùng để báo khi có sự thay đổi trạng thái của phòng
    void notify();
};