#include <iostream>
#include <string>
#include <QDateTime>

#include "BookingFactory.h"
#include "../CUSTOMER/Customer 21.56.18.h"
#include "../ROOM/Room.h"
#include "../ROOM/DerivedRooms.h"
#include "../ROOM/Typeroom.h"

//tại chạy trong local cái lày ko quan trọng
int Room::nextid = 1;
int Customer::nextId = 1;

//hàm hiển thị
void printRoomStatus(const string& roomName, Room* r) {
    cout << roomName << " Status: ";
    switch (r->getStatus()) {
        case RoomStatus::Available:   cout << "Available\n"; break;
        case RoomStatus::Reserved:    cout << "Reserved\n"; break;
        case RoomStatus::Occupied:    cout << "Occupied\n"; break;
        case RoomStatus::Maintenance: cout << "Maintenance\n"; break; 
    }
}

int main() {
    cout << "test" <<'\n';
    Customer* guest1 = new Customer("Alice", "0000000000", "0000000000", MembershipTier::Gold);
    Customer* guest2 = new Customer("Bob", "0000000001", "0000000001", MembershipTier::Sliver); 
        
    Room* room101 = new StandardRoom("S-101", 500000);
    Room* room102 = new VIPROOM("V-201", 1200000);

    //cho 2 thằng sài chung test trường hợp book trùng
    QDateTime checkInDate = QDateTime::currentDateTime();
    QDateTime checkOutDate = checkInDate.addDays(3);

    //status room 101 đầu
    printRoomStatus("Room 101 (Pre-book)", room101);
        
    
    StandardRoomBooking* booking1 = static_cast<StandardRoomBooking*>(BookingFactory::createBooking(BookingType::STANDARD, guest1, checkInDate, checkOutDate, room101, 50.00));
        
    //nên hiện status reserved
    printRoomStatus("Room 101 (Post-book)", room101);

    //nên hiện occupied 
    booking1->resolveDeposit();
    printRoomStatus("Room 101 (Checked In)", room101);


    Booking* booking2 = BookingFactory::createBooking(BookingType::STANDARD, guest2, checkInDate, checkOutDate, room102, 0.00);
    printRoomStatus("Room 102", room102);

    // Attempt to double book room 102
    Booking* doubleBookingAttempt = BookingFactory::createBooking(BookingType::STANDARD, guest1, checkInDate, checkOutDate, room102, 0.00);
        
    if (doubleBookingAttempt == nullptr) {
        cout << "không cho tiếp cận room status occupied" <<'\n';
    }

    delete guest1;
    delete guest2;
    delete room101;
    delete room102;
    delete booking1;
    delete booking2;

    return 0;
}