#pragma once
#include <string>
#include <iostream>
#include "RoomStatus.h"
using namespace std;

class Observer
{
public:
    virtual ~Observer() = default;

    virtual void Ongoing_status_room(string roomId, RoomStatus newStatus) = 0;
};

class Availablee : public Observer
{
public:
    void Ongoing_status_room(string roomId, RoomStatus newStatus) override
    {
        if (newStatus == 0)
        {
            cout << "Room is avaiable\n";
        }
    }
};

class Reservede : public Observer
{
public:
    void Ongoing_status_room(std::string roomId, RoomStatus newStatus) override
    {
        if (newStatus == 1)
        {
            cout << "Room is reserved\n";
        }
    }
};

class Occupiede : public Observer
{
public:
    void Ongoing_status_room(std::string roomId, RoomStatus newStatus) override
    {
        if (newStatus == 2)
        {
            cout << "Room is Occupied\n";
        }
    }
};

class Maintenancee : public Observer
{
public:
    void Ongoing_status_room(std::string roomId, RoomStatus newStatus) override
    {
        if (newStatus == 3)
        {
            cout << "Room is Maintenance\n";
        }
    }
};