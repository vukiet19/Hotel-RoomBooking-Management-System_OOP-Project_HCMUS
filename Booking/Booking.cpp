#include "../Booking/Booking.h"

Booking::Booking(Customer* c) : customer(c), status(BookingStatus::UNCONFIRMED) {}

Booking::~Booking() {
    for (ServiceItem* item : serviceItems) {
        delete item; 
    }
    serviceItems.clear();
}

StandardRoomBooking::~StandardRoomBooking() {
    (this->room)->setStatus(Available);
}

//thêm serviceitems
void Booking::addServiceItem(ServiceItem* item) {
    if (item != nullptr) {
        serviceItems.push_back(item);
    }
}

Customer* Booking::getCustomer() const {
    return customer;
}

Room* StandardRoomBooking::getRoom() const{
    return this->room;
}

StandardRoomBooking::StandardRoomBooking(Customer* c, Room* r, QDateTime in, QDateTime out,double depositAmount) 
: Booking(c), room(r), checkInTime(in), checkOutTime(out), depositAmount(depositAmount) {
    if (this->room) {
        if (this->depositAmount != 0.00){
            this->room->setStatus(RoomStatus::Reserved);
        }
        else{
            this->room->setStatus(RoomStatus::Occupied);
        }
    }
}

//hàm resolve thực thi khi đã trả đủ deposit
void StandardRoomBooking::resolveDeposit(){
    if(this->room->getStatus() == RoomStatus::Reserved){
        this->room->setStatus(RoomStatus::Occupied);
    }
}

int StandardRoomBooking::getNights() const {
    return checkInTime.daysTo(checkOutTime);
}

WalkInTab::WalkInTab(Customer* c,QDateTime in) : Booking(c), dateCreated(in) {}

int WalkInTab::getNights() const {
    return 0;
}