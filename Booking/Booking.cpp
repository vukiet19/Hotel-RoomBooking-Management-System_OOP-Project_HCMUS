#include "../Booking/Booking.h"
#include "../Customer/Customer 21.56.18.h" 
#include "../Room/Room.h"    

IBooking::IBooking(Customer* c) : customer(c), status(BookingStatus::UNCONFIRMED) {}

IBooking::~IBooking() {
    for (ServiceItem* item : serviceItems) {
        delete item; 
    }
    serviceItems.clear();
}

void IBooking::addServiceItem(ServiceItem* item) {
    if (item != nullptr) {
        serviceItems.push_back(item);
    }
}

Customer* IBooking::getCustomer() const {
    return customer;
}

StandardRoomBooking::StandardRoomBooking(Customer* c, Room* r) 
    : IBooking(c), room(r), depositAmount(0.0) {}

int StandardRoomBooking::getNights() const {
    return checkInTime.daysTo(checkOutTime);
}

WalkInTab::WalkInTab(Customer* c) : IBooking(c) {}

int WalkInTab::getNights() const {
    return 0;
}