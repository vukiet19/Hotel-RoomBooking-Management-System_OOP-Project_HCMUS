#pragma once

#include "../Customer/Customer.h"
#include "../Room/DepositStatus.h"
#include "../Room/Room.h"
#include "../Service/ServiceItem.h"
#include "BookingStatus.h"

#include <QDateTime>
#include <memory>
#include <vector>

using namespace std;

class Booking {
  friend class BookingRepository;
protected:
  int id;
  Customer *customer;
  BookingStatus status;
  vector<unique_ptr<ServiceItem>> serviceItems;
  double totalPrice = 0.0;

public:
  int getId() const;
  Customer *getCustomer() const;
  BookingStatus getBookingStatus() const;
  double getTotalPrice() const { return totalPrice; }
  void setTotalPrice(double price) { totalPrice = price; }

  // ko có setters cho status hay customer vì ép vòng đời và hành vi phải tuân
  // theo logic

  Booking(Customer *c);
  virtual ~Booking() = default;
  virtual int getNights() const = 0;
  virtual void checkIn() = 0;
  virtual void checkOut() = 0;
  void addServiceItem(unique_ptr<ServiceItem> serviceItem);
  void addDamagePenaltyItems();

  const vector<unique_ptr<ServiceItem>>& getServiceItems() const;
};

class StandardRoomBooking : public Booking {
  friend class BookingRepository;
private:
  Room *room;
  QDateTime checkInTime;
  QDateTime checkOutTime;
  double depositAmount;
  DepositStatus depositStatus = DepositStatus::NONE;

public:
  StandardRoomBooking(Customer *c, Room *r, QDateTime in, QDateTime out,
                      double depositAmount = 0.00);
  ~StandardRoomBooking();
  Room *getRoom() const;
  void cancelBooking();
  void resolveDeposit();
  DepositStatus getDepositStatus() const;
  int getNights() const override;
  void checkIn() override;
  void checkOut() override;
};

class WalkInTab : public Booking {
  friend class BookingRepository;
private:
  QDateTime dateCreated;

public:
  WalkInTab(Customer *c, QDateTime in);
  int getNights() const override;
  void checkIn() override;
  void checkOut() override;
};