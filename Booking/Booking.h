#pragma once
#include <QDateTime>
#include <vector>
#include "../ServiceItem/ServiceItem.h"
#include "../CUSTOMER/Customer 21.56.18.h"
#include "../ROOM/Room.h"
#include "../ROOM/Typeroom.h"
#include "BookingStatus.h"

using namespace std;

class Booking {
protected:
    int id;
    Customer* customer; 
    BookingStatus status;
    vector<ServiceItem*> serviceItems; 
    
public:
    Booking(Customer* c);
    virtual ~Booking();
    virtual int getNights() const = 0; 
    void addServiceItem(ServiceItem* item);
    Customer* getCustomer() const;
};

class StandardRoomBooking : public Booking {
private:
    Room* room; 
    QDateTime checkInTime;
    QDateTime checkOutTime;
    double depositAmount;
public:
    StandardRoomBooking(Customer* c, Room* r,QDateTime in,QDateTime out,double depositAmount = 0.00);
    ~StandardRoomBooking();
    Room* getRoom() const;
    void resolveDeposit();
    int getNights() const override;
};


class WalkInTab : public Booking {
private:
    QDateTime dateCreated;
public:
    WalkInTab(Customer* c,QDateTime in);
    int getNights() const override;
};