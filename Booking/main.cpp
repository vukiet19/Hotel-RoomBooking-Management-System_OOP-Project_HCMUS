#include <iostream>
#include <string>
#include <memory> 
#include <QDateTime>

#include "BookingFactory.h"
#include "../CUSTOMER/Customer 21.56.18.h"
#include "../ROOM/Room.h"
#include "../ROOM/DerivedRooms.h"
#include "../ROOM/Typeroom.h"

using namespace std; 

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
    cout << "test\n";
    
    //auto cho tiện minh hoạ th chứ lúc làm cứ viết thẳng nó ra ví du unique_ptr<Customer> guest 1;
    auto guest1 = make_unique<Customer>("Alice", "0000000000", "0000000000", MembershipTier::Gold);
    auto guest2 = make_unique<Customer>("Bob", "0000000001", "0000000001", MembershipTier::Sliver); 
        
    auto room101 = make_unique<StandardRoom>("S-101", 500000);
    auto room102 = make_unique<VipRoom>("V-201", 1200000);

    QDateTime checkInDate = QDateTime::currentDateTime();
    QDateTime checkOutDate = checkInDate.addDays(3);

    //status room 101 đầu
    cout<<"Status room 101 pre-book"<<'\n';

    //GET LÀ HÀM CHO MƯỢN(ĐƯỢC NHÌN NHƯNG KO ĐƯỢC XOÁ) (MOVE MỚI LÀ CHUYỂN NHƯỢNG QUYỀN SỞ HỮU)
    printRoomStatus("Room 101 (Pre-book)", room101.get());

    unique_ptr<StandardRoomBooking> booking1(static_cast<StandardRoomBooking*>(
        BookingFactory::createBooking(BookingType::STANDARD, guest1.get(), checkInDate, checkOutDate, room101.get(), 50.00)
    ));
    
    cout<<"Status room 101 sau book"<<'\n';
    //nên hiện status reserved
    printRoomStatus("Room 101 (Post-book)", room101.get());

    cout<<"hoàn tất reservation (giai quyết deposi)"<<'\n';
    //nên hiện occupied 
    booking1->resolveDeposit();
    printRoomStatus("Room 101 (Checked In)", room101.get());

    unique_ptr<Booking> booking2(
        BookingFactory::createBooking(BookingType::STANDARD, guest2.get(), checkInDate, checkOutDate, room102.get(), 0.00)
    );
    printRoomStatus("Room 102", room102.get());

    unique_ptr<Booking> doubleBookingAttempt(
        BookingFactory::createBooking(BookingType::STANDARD, guest1.get(), checkInDate, checkOutDate, room102.get(), 0.00)
    );
        
    if (doubleBookingAttempt == nullptr) {
        cout << "không cho tiếp cận room status occupied\n";
    }


    return 0;
}