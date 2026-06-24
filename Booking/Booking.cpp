#include "../Booking/Booking.h"

Booking::Booking(Customer* c) : customer(c), status(BookingStatus::UNCONFIRMED) {}

StandardRoomBooking::~StandardRoomBooking() {
    (this->room)->setStatus(Available);
}

//thêm serviceitems
void Booking::addServiceItem(unique_ptr<ServiceItem> serviceItem) {
    if (serviceItem != nullptr) {
        serviceItems.push_back(move(serviceItem));
    }
}

Customer* Booking::getCustomer() const {
    return customer;
}

Room* StandardRoomBooking::getRoom() const{
    return this->room;
}

//deposit <= 0 thì mặc định vẫn None
StandardRoomBooking::StandardRoomBooking(Customer* c, Room* r, QDateTime in, QDateTime out, double depositAmount) 
: Booking(c), room(r), checkInTime(in), checkOutTime(out), depositAmount(depositAmount) {
    if(this->depositAmount > 0.00){
        this->depositStatus = DepositStatus::PAID; 
    }
    if(this->room && this->room->getStatus() == RoomStatus::Available){
        if (this->depositStatus == DepositStatus::PAID || this->depositStatus == DepositStatus::PENDING) {
            this->room->setStatus(RoomStatus::Reserved);
        } 
        else {
            this->room->setStatus(RoomStatus::Occupied);
        }
    }
}

//hàm resolve thực thi khi và set room/deposit status đúng trạng thái
void StandardRoomBooking::resolveDeposit() {
    if(this->room->getStatus() == RoomStatus::Reserved){
        this->room->setStatus(RoomStatus::Occupied);
        this->depositStatus = DepositStatus::PAID; 
    }
}

DepositStatus StandardRoomBooking::getDepositStatus() const {
    return this->depositStatus;
}

int StandardRoomBooking::getNights() const {
    return checkInTime.daysTo(checkOutTime);
}

WalkInTab::WalkInTab(Customer* c,QDateTime in) : Booking(c), dateCreated(in) {}

int WalkInTab::getNights() const {
    return 0;
}


//check in/out

//check in thì resolve deposit (quá hợp lý =))
void StandardRoomBooking::checkIn() {
    this->status = BookingStatus::CHECKED_IN;
    this->resolveDeposit(); 
}

void StandardRoomBooking::checkOut() {
    this->status = BookingStatus::CHECKED_OUT;
    if(this->room) {
        this->room->setStatus(RoomStatus::Available);
    }
}

void WalkInTab::checkIn() {
    this->status = BookingStatus::CHECKED_IN;
}

void WalkInTab::checkOut() {
    this->status = BookingStatus::CHECKED_OUT;
}