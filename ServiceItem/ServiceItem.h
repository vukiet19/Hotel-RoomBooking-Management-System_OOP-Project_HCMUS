#pragma once
#include <string>

// TẠM THỜI BỎ QUA CÁI FACILITY BOOKING ITEM ĐẶC BIỆT TÍNH SAU

enum class ServiceStatus {
    PENDING, IN_PROGRESS, DELIVERED, BILLED, CANCELLED
};  

enum class ServiceType {
    FOOD_ORDER, MINIBAR, FURNITURE, TRANSPORT, DAMAGE_PENALTY
};

class ServiceItem {
protected: 
    int id;
    std::string name;
    int quantity;
    double unitPrice;
    ServiceStatus status;

public:
    ServiceItem(std::string n, int q, double price);
    virtual ~ServiceItem() = default; 

    virtual double getSubTotal() const;
    void changeStatus(ServiceStatus newStatus);
};

class MinibarItem : public ServiceItem {
public:
    MinibarItem(std::string n, int q, double price);
};

class TransportServiceItem : public ServiceItem {
public:
    TransportServiceItem(std::string n, int q, double price);
};

class FoodOrderItem : public ServiceItem {
public:
    FoodOrderItem(std::string n, int q, double price);
};

class FurnitureItem : public ServiceItem {
public:
    FurnitureItem(std::string n, int q, double price);
};

class DamagePenaltyItem : public ServiceItem {
public:
    DamagePenaltyItem(std::string n, int q, double price);
};