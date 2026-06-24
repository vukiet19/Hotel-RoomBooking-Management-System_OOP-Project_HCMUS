#pragma once
#include "Booking.h"
#include "BookingType.h"

class BookingFactory {
public:
    static Booking* createBooking(BookingType type, Customer* c,QDateTime in,QDateTime out,Room* r = nullptr,double depositAmount = 0.00);
};