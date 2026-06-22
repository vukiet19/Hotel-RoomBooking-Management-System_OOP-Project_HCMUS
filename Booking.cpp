#include <QDateTime>
#include <vector>
#include "ServiceItem.cpp"
#include "Customer.cpp"
#include "Room.cpp"

enum class BookingStatus{
    UNCONFIRMED, CONFIRMED, CHECKED_IN, CHECKED_OUT
};



class IBooking{
protected:
    int id;
    Customer* customer; 
    BookingStatus status;
    vector<ServiceItem*> serviceItems; 
public:
    IBooking(Customer* c) : customer(c), status(BookingStatus::UNCONFIRMED){}
    
    virtual ~IBooking(){
        for (ServiceItem* item : serviceItems){
            delete item; 
        }
        serviceItems.clear();
    }
    virtual int getNights() const = 0; 

    void addServiceItem(ServiceItem* item){
        if (item != nullptr) {
            serviceItems.push_back(item);
        }
    }

    Customer* getCustomer() const{
        return customer;
    }
};

// đổi cách đặt tên để phân biệt với FacilityBooking sau này
class StandardRoomBooking : public IBooking{
private:
    Room* room; 
    QDateTime checkInTime;
    QDateTime checkOutTime;
    double depositAmount;

public:
    StandardRoomBooking(Customer* c, Room* r) :IBooking(c), room(r), depositAmount(0.0){}

    int getNights() const override{
        //logic checkout-checkin sau convert
        return 3; 
    }
};


// bàn thêm phần này tạo lấy sươn thôi
class WalkInTab : public IBooking{
private:
    QDateTime dateCreated;
public:
    WalkInTab(Customer* c):IBooking(c){}
    int getNights() const override{
        return 0;
    }
};