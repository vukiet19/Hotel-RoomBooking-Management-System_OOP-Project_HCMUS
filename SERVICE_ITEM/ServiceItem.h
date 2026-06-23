#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <memory>
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
    ServiceItem(int id, string name, double price, int quantity, string note) {
        this->id = id;
        this->name = name;
        this->unitPrice = price;
        this->quantity = quantity;
        /* Luu y cho UI: kiem tra neu note == "" thi se khong hien thi.
           Nguoc lai se hien thi Note: <noi dung note> tren bill*/
        this->note = note;
        // Moi tao ra thi trang thai la Pending
        changeStatus(ServiceStatus::Pending);
    }

    virtual double getSubtotal() const { return quantity * unitPrice; }
    
    int getId() const { return id; }
    string getName() const { return name; }
    double getUnitPrice() const { return unitPrice; }
    int getQuantity() const { return quantity; }
    ServiceStatus getStatus() const { return status; }
    string getNote() const { return note; }
    
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

        void setNote(string note) {
            this->note = note;
        }

        virtual ~ServiceItem() = default;
    };
    
    class FoodOrderItem : public ServiceItem {    
    public:
        FoodOrderItem(int id, string name, double price, int quantity, string note = "")
        : ServiceItem(id, name, price, quantity, note) {}

        double getSubtotal() const override {
            double base = ServiceItem::getSubtotal();

            // Vi du neu dung dich vu goi do an thi cong them 5% phi dich vu
            return base * 1.05;
        }
    };

    class ServiceItemFactory {
    public:
        static unique_ptr<ServiceItem> createServiceItem(
        ServiceType type, int id, string name, double price, int quantity, string note) {
            switch (type) {
                case ServiceType::FoodOrderItem:
                    return make_unique<FoodOrderItem>(id, name, price, quantity, note);
                default:
                    return nullptr;
            }
        }
    };