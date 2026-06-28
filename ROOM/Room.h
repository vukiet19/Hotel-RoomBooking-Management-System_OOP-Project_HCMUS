#pragma once
#include <string>
#include "RoomStatus.h"
#include "Typeroom.h"
#include "observer.h"
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
    vector<Observer *> observers;

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
    void addObserver(Observer *observer);

    void setBasePrice(int baseprice);

    // setstatus
    void setStatus(RoomStatus status);

    // set room name
    void setRoomNumber(string roomNumber);

    // getId
    string getId() const;

    // Hàm này dùng để báo khi có sự thay đổi trạng thái của phòng
    void notify();
};