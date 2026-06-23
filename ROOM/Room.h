#pragma once
#include <string>
#include "RoomStatus.h" 
#include "Typeroom.h"

using namespace std;

class Room {
//protected co y nghia la ham con truy cap dc
protected: 
    string id;
    string roomNumber; 
    RoomStatus status;
    double basePrice;
    int maxExtraBeds;

public:
    static int nextid;
    //ham khoi tao
    Room(string roomNumber, double basePrice) {
        // id co cau truc la "R" + 0000-> 9999
        this->id = "R";
        string tmp = to_string(nextid);
        
        int zerosNeeded = 4 - tmp.size();
        for (int i = 0; i < zerosNeeded; i++) {
            this->id += '0';
        }
        this->id += tmp; 
        
        nextid++; 

        this->roomNumber = roomNumber;
        this->basePrice = basePrice;
        this->status = Available;
        this->maxExtraBeds = 0;
    }

    //destructor
    virtual ~Room() = default;

    //get_type
    virtual RoomType getType() const = 0;

    //getbaseprice
    virtual int getBasePrice()  const{
        return basePrice;
    }


    void setStatus(RoomStatus status) {
        this->status = status;
    }
    

    string getId() const {
        return id;
    }
};