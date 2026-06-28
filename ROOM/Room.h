#pragma once
#include <string>
#include "RoomStatus.h"
#include "Typeroom.h"
#include "observer.h"

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
    Room(string roomNumber, int basePrice)
    {
        // id co cau truc la "R" + 0000-> 9999
        this->id = "R";
        string tmp = to_string(nextid);

        int zerosNeeded = 4 - tmp.size();
        for (int i = 0; i < zerosNeeded; i++)
        {
            this->id += '0';
        }
        this->id += tmp;

        nextid++;

        this->roomNumber = roomNumber;
        this->basePrice = basePrice;
        this->status = Available;
    }

    // destructor
    virtual ~Room() = default;

    // get_type
    virtual RoomType getType() const = 0;

    // getbaseprice
    virtual int getBasePrice() const
    {
        return basePrice;
    }

    void addObserver(Observer *observer)
    {
        observers.push_back(observer);
    }

    void setStatus(RoomStatus status)
    {
        this->status = status;

        notify();
    }

    string getId() const
    {
        return id;
    }

    void notify()
    {
        for (auto observer : observers)
        {
            observer->Ongoing_status_room(id, status);
        }
    }
};