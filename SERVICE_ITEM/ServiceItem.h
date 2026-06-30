#pragma once
/* 
File này phục vụ cho việc quản lý các dịch vụ, hàng hóa, phí phạt trong quá trình sử dụng
khách sạn. ServiceItem sẽ được sử dụng dưới dạng một vector trong class Booking.

Các file liên quan: SerivceEnums.h (chứa trạng thái và kiểu dịch vụ)
*/

#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <memory>
#include <QDateTime>
#include "ServiceEnums.h"

using namespace std;

class ServiceItem {
private:
    int id, quantity;
    string name;
    double unitPrice;
    ServiceStatus status;
    string note;

public:
    ServiceItem(int id, string name, double price, int quantity, string note);

    virtual double getSubtotal() const;

    int getId() const;
    string getName() const;
    double getUnitPrice() const;
    int getQuantity() const;
    ServiceStatus getStatus() const;
    string getNote() const;

    void setName(const string& newName);
    void setUnitPrice(double newPrice);
    void setQuantity(int newQuantity);
    void changeStatus(ServiceStatus newStatus);
    void setNote(string note);

    virtual ~ServiceItem();
};

class FoodOrderItem : public ServiceItem {
public:
    FoodOrderItem(int id, string name, double price, int quantity, string note = "");

    double getSubtotal() const override;
};

class MinibarItem : public ServiceItem {
private:
    bool isRestocked;

public:
    MinibarItem(int id, string name, double price, int quantity, string note = "");

    bool getRestockedStatus() const;
    void markAsRestocked();
};

class FurnitureItem : public ServiceItem {
private:
    bool isReturned;
    bool isDamaged;

public:
    FurnitureItem(int id, string name, double price, int quantity, string note = "");

    bool getReturnStatus() const;
    bool getDamagedStatus() const;
    void markAsReturned(bool damaged = false /* Neu khong truyen tham so thi mac dinh no la false */);

    /* Lien quan toi viec check-out room, khi khach hang check-out, ta se kiem tra qua cac status cua items
    (minibar, furniture,...):
    - Neu phat hien chua RETURN -> yeu cau nhan vien len return lai mon do
    - Neu DA RETURN nhung isDamaged == true -> khoi tao doi tuong DamagePenaltyItem
    */
};

class DamagePenaltyItem : public ServiceItem {
public:
    DamagePenaltyItem(int id, string name, double price, int quantity, string note);
};

class ServiceItemFactory {
public:
    static unique_ptr<ServiceItem> createServiceItem(
        ServiceType type, int id, string name, double price, int quantity, string note);
};
