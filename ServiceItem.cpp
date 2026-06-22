#include <iostream>
#include <string>
#include <vector>

using namespace std;

// TẠM THỜI BỎ QUA CÁI FACILITY BOOKING ITEM ĐẶC BIỆT TÍNH SAU

enum class ServiceStatus{
    PENDING, IN_PROGRESS, DELIVERED, BILLED, CANCELLED
};  
enum class ServiceType{
    FOOD_ORDER, MINIBAR, FURNITURE, TRANSPORT, DAMAGE_PENALTY
};

class ServiceItem{
protected: 
    int id;
    string name;
    int quantity;
    double unitPrice;
    ServiceStatus status;

public:
    ServiceItem(string n, int q, double price): name(n), quantity(q), unitPrice(price), status(ServiceStatus::PENDING){}

    virtual ~ServiceItem() = default; 

    virtual double getSubTotal() const{
        return quantity * unitPrice;
    }
    void changeStatus(ServiceStatus newStatus){
        this->status = newStatus;
    }
};


class MinibarItem:public ServiceItem{
public:
    MinibarItem(string n, int q, double price):ServiceItem(n, q, price){}
};

class TransportServiceItem:public ServiceItem{
public:
    TransportServiceItem(string n, int q, double price):ServiceItem(n, q, price){}
};

class FoodOrderItem:public ServiceItem{
public:
    FoodOrderItem(string n, int q, double price):ServiceItem(n, q, price){}
};

class FurnitureItem:public ServiceItem{
public:
    FurnitureItem(string n, int q, double price):ServiceItem(n, q, price){}
};

class DamagePenaltyItem:public ServiceItem{
public:
    DamagePenaltyItem(string n, int q, double price):ServiceItem(n, q, price){}
};



class IServiceItemFactory{
public:
    virtual ~IServiceItemFactory() = default;
    virtual ServiceItem* createServiceItem(ServiceType type, string name, int quantity) = 0;
};

class PricingDatabaseManager{
public:
    // giả lập sqlite lookup / query 
    static double fetchPrice(ServiceType type, string name){
        if (type == ServiceType::MINIBAR && name == "Coke") return 2.50;
        if (type == ServiceType::FOOD_ORDER && name == "Burger") return 15.00;
        if (type == ServiceType::TRANSPORT && name == "Taxi") return 20.00;
        return 0.00;
    }
};

// cơ chế query price, diễn dịch ServiceType cần bàn thêm
class ServiceItemFactory:public IServiceItemFactory{
public:
    ServiceItem* createServiceItem(ServiceType type, string name, int quantity) override{
        double price = PricingDatabaseManager::fetchPrice(type, name);
        switch(type){
            case ServiceType::MINIBAR:
                return new MinibarItem(name, quantity, price);
            case ServiceType::TRANSPORT:
                return new TransportServiceItem(name, quantity, price);
            case ServiceType::FOOD_ORDER:
                return new FoodOrderItem(name, quantity, price);
            default:
                return nullptr;
        }
    }
};