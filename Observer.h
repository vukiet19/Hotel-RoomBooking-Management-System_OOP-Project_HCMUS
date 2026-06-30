#pragma once
#include <string>
#include <iostream>
#include "RoomStatus.h"
using namespace std;

// Hàm này dùng cho observer pattern cho các phòng nếu có thay đổi status

// class cho observer
class Observer
{
public:
    // destructor
    virtual ~Observer();

    // Hàm để xem thông báo status của phòng( nếu phòng status đó thì sẽ báo) .
    //= 0 trong hàm nghĩa là class con phải tự viết cách hoạt động của hàm đó
    virtual void Ongoing_status_room(string roomId, RoomStatus newStatus) = 0;
};

class Room_Available : public Observer
{
public:
    // Nếu observer của phòng là Available thì sẽ báo status
    void Ongoing_status_room(string roomId, RoomStatus newStatus) override;
};

class Room_Reserved : public Observer
{
public:
    // Nếu observer của phòng là Reserved thì sẽ báo status
    void Ongoing_status_room(string roomId, RoomStatus newStatus) override;
};

class Room_Occupied : public Observer
{
public:
    // Nếu observer của phòng là Occupied thì sẽ báo status
    void Ongoing_status_room(string roomId, RoomStatus newStatus) override;
};

class Room_Maintenance : public Observer
{
public:
    // Nếu observer của phòng là Maintenance  thì sẽ báo status
    void Ongoing_status_room(string roomId, RoomStatus newStatus) override;
};