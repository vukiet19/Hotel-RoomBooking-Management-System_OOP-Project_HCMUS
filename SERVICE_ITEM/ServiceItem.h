#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include "ServiceStatus.h"

using namespace std;

class ServiceItem {
private:
    int id, quantity;
    string name;
    double unitPrice;
    ServiceStatus status;

public:
    ServiceItem(int id, string name, double price, int qantity) {
        this->id = id;
        this->name = name;
        this->unitPrice = price;
        this-> quantity = quantity;
        // Moi tao ra thi trang thai la Pending
        changeStatus(ServiceStatus::Pending);
    }

    virtual double getSubtotal() const { return quantity * unitPrice; }
    
    int getId() const { return id; }
    string getName() const { return name; }
    double getUnitPrice() const { return unitPrice; }
    int getQuantity() const { return quantity; }
    ServiceStatus getStatus() const { return status; }
    
    void setName(const string& newName) {
        if (newName.empty()) {
            // thong bao loi tren UI
            return;
        }
        this->name = newName;
    }
    
    void setUnitPrice(double newPrice) {
        if (newPrice < 0) {
            // thong bao loi tren UI;
            return;
        }
        
        if (this->status == ServiceStatus::Billed) {
            // thong bao loi tren UI
            return;
        }
        this->unitPrice = newPrice;
    }
    
    void setQuantity(int newQuantity) {
        if (newQuantity <= 0) {
            // thong bao loi tren UI
            return;
        }
        
        if (this->status == ServiceStatus::Delivered || 
            this->status == ServiceStatus::Billed || 
            this->status == ServiceStatus::Cancelled) {
                // thong bao loi tren UI
                return;
            }
            this->quantity = newQuantity;
        }
        
        void changeStatus(ServiceStatus newStatus) {
            // O status Billed coi nhu da xong, khong the chinh sua trang thai
            if (this->status == ServiceStatus::Billed) {
                // thong bao loi tren UI
                return;
            }
            
            // Da Cancelled roi thi khong the chinh sua trang thai
            if (this->status == ServiceStatus::Cancelled) {
                // thong bao loi tren UI
                return;
            }
            this->status = newStatus;
        }

        virtual ~ServiceItem() = default;
    };
    
    class FoodOrderItem : public ServiceItem {
    private:
        // Bien nay de khach hang co ghi chu gi cho dau bep thi them vo
        string note;
    
    public:
        //
        FoodOrderItem(int id, string name, double price, int quantity)
        : ServiceItem(id, name, price, quantity) {}

        FoodOrderItem(int id, string name, double price, int quantity, string note = "")
        : ServiceItem(id, name, price, quantity) {
            this->note = note;
        }

        double getSubtotal() const override {
            double base = ServiceItem::getSubtotal();

            // Vi du neu dung dich vu goi do an thi cong them 5% phi dich vu
            return base * 1.05;
        }
    };