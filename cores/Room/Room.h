// Hàm này chứa định nghĩa class và hàm của room
// Các file liên quan Typeroom.h( Dùng để chứa các kiểu phòng),observer.h(Observer pattern), Customer.h(Chủ yếu xài cho việc demo hàm getBill())
#pragma once

#include "RoomStatus.h"
#include "TypeRoom.h"
#include "frontend/Observers/Observer.h"
#include "cores/Customer/Customer.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Room
{
    friend class RoomRepository;
    // protected co y nghia la ham con truy cap dc
protected:
    string id;
    string roomNumber;
    RoomStatus status;
    int basePrice;
    int number_people;

    friend class RoomRepository;

public:
    // ham khoi tao
    static int nextid;
    
    //tạo hàm này để tìm nextId lớn nhất từng được khởi tạo để không bị khởi tạo trùng
    //vì biến static nextid này nằm trên RAM, mỗi lần tắt chương trình bật lại thì nextid reset về 0 gây trùng
    static void seedNextId(int maxId);

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