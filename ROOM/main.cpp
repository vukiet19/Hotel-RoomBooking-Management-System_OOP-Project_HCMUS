#include <iostream>
#include "Room.h"
#include "RoomStatus.h"
#include "Typeroom.h"
#include "DerivedRooms.h"
#include "observer.h"

using namespace std;

// khai bao bien static
int Room::nextid = 0;

int main()
{
    StandardRoom r1("101", 10000000.0);

    std::cout << "Room " << r1.getType() << " created.\n";

    Reservede reser;
    Occupiede Occ;
    Availablee Ava;
    Maintenancee mai;

    r1.addObserver(&Occ);
    r1.addObserver(&reser);
    r1.addObserver(&Ava);
    r1.addObserver(&mai);

    r1.setStatus(Reserved);

    r1.setStatus(Occupied);

    r1.setStatus(Maintenance);

    r1.setStatus(Available);

    return 0;
}