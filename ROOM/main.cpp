#include <iostream>
#include "Room.h"
#include "RoomStatus.h"
#include "Typeroom.h"
#include "DerivedRooms.h" 

using namespace std;


//khai bao bien static
int Room::nextid = 0;




int main() {
    Room* r1 = new StandardRoom("S-101", 500000);
    Room* r2 = new VIPROOM("V-201", 1200000);
    Room* r3 = new PresiROOM("P-301", 5000000);

    cout << "ID: " << r1->getId() << " | Room Type: " << r1->getType() << '\n';
    cout << "Price: " << r1->getBasePrice() << " VND\n\n";

    cout << "ID: " << r2->getId() << " | Room Type: " << r2->getType() << '\n';
    cout << "Price: " << r2->getBasePrice() << " VND\n\n";

    cout << "ID: " << r3->getId() << " | Room Type: " << r3->getType() << '\n';
    cout << "Price: " << r3->getBasePrice() << " VND\n";

    delete r1;
    delete r2;
    delete r3;

    return 0;
}