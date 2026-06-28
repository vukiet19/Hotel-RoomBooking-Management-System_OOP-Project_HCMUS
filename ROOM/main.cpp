#include <iostream>
#include "Room.h"
#include "RoomStatus.h"
#include "Typeroom.h"
#include "DerivedRooms.h"
#include "observer.h"

using namespace std;

// khai bao bien static

int main()
{
    StandardRoom r1;

    std::cout << "Room " << r1.getType() << " created.\n";

    Room_Reserved reser;
    Room_Occupied Occ;
    Room_Available Ava;
    Room_Maintenance mai;

    r1.addObserver(&Occ);
    r1.addObserver(&reser);
    r1.addObserver(&Ava);
    r1.addObserver(&mai);

    r1.setStatus(Reserved);

    r1.setStatus(Occupied);

    r1.setStatus(Maintenance);

    r1.setStatus(Available);

    r1.setBasePrice(1000000);
    cout << r1.getBasePrice();

    return 0;
}