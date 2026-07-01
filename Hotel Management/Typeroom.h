#pragma once
#include <string>
using namespace std;

enum RoomType {
    Standard,
    VIP,
    Presidential
};

inline string roomTypeToString(RoomType type) {
    switch (type) {
    case Standard:      
        return "Standard Room";
    case VIP:           
        return "VIP Room";
    case Presidential: 
        return "Presidential Suite";
    default:                
        return "Unknown Room Type";
    }
}