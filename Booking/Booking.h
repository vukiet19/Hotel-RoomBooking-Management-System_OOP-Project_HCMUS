#pragma once
#include <QDateTime>
#include <vector>
#include "../SERVICE_ITEM/ServiceItem.h"
#include "../CUSTOMER/Customer 21.56.18.h"
#include "../ROOM/Room.h"
#include "../ROOM/Typeroom.h"
#include "BookingStatus.h"
#include "DepositStatus.h"

using namespace std;

class Booking {
protected:
    int id;
    Customer* customer; 
    BookingStatus status;
    vector<unique_ptr<ServiceItem>> serviceItems;
    
public:
    Booking(Customer* c);
    virtual ~Booking() = default;
    virtual int getNights() const = 0; 
    virtual void checkIn() = 0;
    virtual void checkOut() = 0;
    void addServiceItem(unique_ptr<ServiceItem> serviceItem);
    Customer* getCustomer() const;
    void addDamagePenaltyItems();
};

class StandardRoomBooking : public Booking {
private:
    Room* room; 
    QDateTime checkInTime;
    QDateTime checkOutTime;
    double depositAmount;
    DepositStatus depositStatus = DepositStatus::NONE;

public:
    StandardRoomBooking(Customer* c, Room* r,QDateTime in,QDateTime out,double depositAmount = 0.00);
    ~StandardRoomBooking();
    Room* getRoom() const;
    void cancelBooking();
    void resolveDeposit();
    DepositStatus getDepositStatus() const;
    int getNights() const override;
    void checkIn() override;
    void checkOut() override;
};


class WalkInTab : public Booking {
private:
    QDateTime dateCreated;
public:
    WalkInTab(Customer* c,QDateTime in);
    int getNights() const override;
    void checkIn() override;
    void checkOut() override;
};