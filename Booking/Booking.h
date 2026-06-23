#pragma once
#include <QDateTime>
#include <vector>
#include "../ServiceItem/ServiceItem.h"

// Forward declarations to avoid heavy includes in header
class Customer;
class Room;

enum class BookingStatus {
    UNCONFIRMED, CONFIRMED, CHECKED_IN, CHECKED_OUT
};

class IBooking {
protected:
    int id;
    Customer* customer; 
    BookingStatus status;
    std::vector<ServiceItem*> serviceItems; 
    
public:
    IBooking(Customer* c);
    virtual ~IBooking();
    
    virtual int getNights() const = 0; 
    void addServiceItem(ServiceItem* item);
    Customer* getCustomer() const;
};

// đổi cách đặt tên để phân biệt với FacilityBooking sau này
class StandardRoomBooking : public IBooking {
private:
    Room* room; 
    QDateTime checkInTime;
    QDateTime checkOutTime;
    double depositAmount;

public:
    StandardRoomBooking(Customer* c, Room* r);
    int getNights() const override;
};

// bàn thêm phần này tạo lấy sươn thôi
class WalkInTab : public IBooking {
private:
    QDateTime dateCreated;
public:
    WalkInTab(Customer* c);
    int getNights() const override;
};