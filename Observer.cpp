// Hàm này dùng cho observer( nếu có trạng thái đó thfi sẽ báo)
//  Hàm này có liên quan đến Room.h và được thông báo qua hàm notify() của room.h

#include <string>
#include <iostream>
#include "Room/RoomStatus.h"
#include "Booking/BookingStatus.h"
#include "Observer.h"
using namespace std;
HotelEventManager::HotelEventManager()
{
    // mặc định có 1 observer là StatusLogger khi sinh ra
    static StatusLogger *defaultLogger = new StatusLogger();
    addRoomObserver(defaultLogger);
    addBookingObserver(defaultLogger);
}

HotelEventManager &HotelEventManager::instance()
{
    static HotelEventManager inst;
    return inst;
}

// thêm RoomObserver vào manager
void HotelEventManager::addRoomObserver(RoomObserver *observer)
{
    if (observer)
    {
        roomObservers.push_back(observer);
    }
}

// xoá observer khỏi manager
void HotelEventManager::removeRoomObserver(RoomObserver *observer)
{
    roomObservers.erase(remove(roomObservers.begin(), roomObservers.end(), observer), roomObservers.end());
}

// truyền thông tin thay đổi cho observer
void HotelEventManager::notifyRoomStatus(const RoomEvent &event)
{
    for (auto *obs : roomObservers)
    {
        if (obs)
        {
            obs->onRoomStatusChanged(event);
        }
    }
}

// thêm BookingObserver vào manager
void HotelEventManager::addBookingObserver(BookingObserver *observer)
{
    if (observer)
    {
        bookingObservers.push_back(observer);
    }
}

// xoá BookingObserver khỏi manager
void HotelEventManager::removeBookingObserver(BookingObserver *observer)
{
    bookingObservers.erase(remove(bookingObservers.begin(), bookingObservers.end(), observer), bookingObservers.end());
}

// truyền thông tin thay đổi cho observer
void HotelEventManager::notifyBookingStatus(const BookingEvent &event)
{
    for (auto *obs : bookingObservers)
    {
        if (obs)
        {
            obs->onBookingStatusChanged(event);
        }
    }
}

// xuất ra thay đổi
void StatusLogger::onRoomStatusChanged(const RoomEvent &event)
{
    string statusStr;
    switch (event.newStatus)
    {
    case RoomStatus::Available:
        statusStr = "Available";
        break;
    case RoomStatus::Reserved:
        statusStr = "Reserved";
        break;
    case RoomStatus::Occupied:
        statusStr = "Occupied";
        break;
    case RoomStatus::Maintenance:
        statusStr = "Maintenance";
        break;
    }
    cout << "[" << event.timestamp << "] [LOG] Room " << event.roomId
         << " status changed to: " << statusStr << endl;
}

void StatusLogger::onBookingStatusChanged(const BookingEvent &event)
{
    string statusStr;
    switch (event.newStatus)
    {
    case BookingStatus::UNCONFIRMED:
        statusStr = "UNCONFIRMED";
        break;
    case BookingStatus::CONFIRMED:
        statusStr = "CONFIRMED";
        break;
    case BookingStatus::CHECKED_IN:
        statusStr = "CHECKED_IN";
        break;
    case BookingStatus::CHECKED_OUT:
        statusStr = "CHECKED_OUT";
        break;
    }
    cout << "[" << event.timestamp << "] [LOG] Booking ID " << event.bookingId
         << " (" << event.customerName << ", Room " << event.roomNumber
         << ", Price: " << event.totalPrice << ") status changed to: " << statusStr << endl;
}