#include "BookingFactory.h"

// không cho thâm nhập nếu Room* == nullptr
Booking *BookingFactory::createBooking(BookingType type, Customer *c, QDateTime in, QDateTime out, Room *r, double depositAmount)
{
    if (type == BookingType::STANDARD && r != nullptr)
    {
        if (r->getStatus() != RoomStatus::Available)
        {
            return nullptr;
        }
        return new StandardRoomBooking(c, r, in, out, depositAmount);
    }
    if (type == BookingType::WALK_IN)
    {
        return new WalkInTab(c, in);
    }
    return nullptr;
}