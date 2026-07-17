#pragma once
#include <string>

#include <iostream>
#include <vector>
#include "Booking/BookingStatus.h"
#include "Room/RoomStatus.h"
using namespace std;
// TÓM TẮT:
// CÓ MANAGER ĐỂ QUẢN LÝ CÁC EVENT ĐƯỢC NOTIFY VÀ CHECK COI CÓ OBSERVER NÀO ĐẢM NHẬN
// VIỆC BÁO LÊN UI VỀ EVENT NÀY HAY KHÔNG . NẾU CÓ THÌ MANAGER GỌI HÀM TƯƠNG ỨNG
// VIỆC PHÒNG NÀO CẦN OBSERVER HAY BOOKING NÀO CẦN MÌNH SẼ MANUALLY TẠO VÀ THÊM OBSERVER ĐÓ VÀO HOTELEVENTMANAGER
// SETSTATUS() -> TẠO VÀ TRUYỀN EVENT VỀ MANAGER -> OBSERVER CÓ THÌ BÁO LÊN UI

// các struct này sẽ quy định những thông tin observer hiển thị trên dashboard
struct RoomEvent
{
    string roomId;
    RoomStatus newStatus;
    string timestamp;
};

struct BookingEvent
{
    int bookingId;
    string customerName;
    string roomNumber;
    BookingStatus newStatus;
    double totalPrice;
    string timestamp;
};

// Interfaces for observing Room and Booking changes
class RoomObserver
{
public:
    virtual ~RoomObserver() = default;
    virtual void onRoomStatusChanged(const RoomEvent &event) = 0;
};

class BookingObserver
{
public:
    virtual ~BookingObserver() = default;
    virtual void onBookingStatusChanged(const BookingEvent &event) = 0;
};

// class này sẽ nhận thông tin từ các events và xuất ra console
// coi này như placeholder thôi để test
class StatusLogger : public RoomObserver, public BookingObserver
{
public:
    void onRoomStatusChanged(const RoomEvent &event) override;
    void onBookingStatusChanged(const BookingEvent &event) override;
};

// Event Manager
class HotelEventManager
{
private:
    HotelEventManager();
    ~HotelEventManager() = default;

    // cấm copy hay copy constructor vì chỉ có 1 manager duy nhất
    HotelEventManager(const HotelEventManager &) = delete;
    HotelEventManager &operator=(const HotelEventManager &) = delete;

    vector<RoomObserver *> roomObservers;
    vector<BookingObserver *> bookingObservers;

public:
    static HotelEventManager &instance();

    void addRoomObserver(RoomObserver *observer);
    void removeRoomObserver(RoomObserver *observer);
    void notifyRoomStatus(const RoomEvent &event);

    void addBookingObserver(BookingObserver *observer);
    void removeBookingObserver(BookingObserver *observer);
    void notifyBookingStatus(const BookingEvent &event);
};
