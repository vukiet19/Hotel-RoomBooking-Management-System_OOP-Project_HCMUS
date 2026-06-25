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
        
        if (this->status == ServiceStatus::Billed || 
            this->status == ServiceStatus::Cancelled) {
                // thong bao loi tren UI
                return;
            }
            this->quantity = newQuantity;
        }
        
        void changeStatus(ServiceStatus newStatus) {
            if (newStatus == ServiceStatus::Pending) {
                this->status = newStatus;
            }
            
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

class MinibarItem : public ServiceItem {
private:
    bool isRestocked;

public:
    MinibarItem(int id, string name, double price, int quantity, string note = "")
    : ServiceItem(id, name, price, quantity, note) {
        /* Ly do khoi tao false la khi mot mon do duoc khoi tao, tuc la khach hang da su
        dung no roi. Vi the trang thai cua no se la CHUA DUOC restock */
        this->isRestocked = false;
    }

    bool getRestockedStatus() const { return isRestocked; }

    void markAsRestocked() {
        this->isRestocked = true;
    }
};

class FurnitureItem : public ServiceItem {
private:
    bool isReturned;
    bool isDamaged;

public:
    FurnitureItem(int id, string name, double price, int quantity, string note = "")
    : ServiceItem(id, name, price, quantity, note) {
          /* Ly do khoi tao false la khi mot mon do noi that duoc khoi tao tuc la no dang duoc khach hang dung,
          chua duoc tra ve kho. Vi the trang thai cua no se la CHUA DUOC return */
        this->isReturned = false;
        this->isDamaged = false;
    }

    bool getReturnStatus() const { return isReturned; }

    bool getDamagedStatus() const { return isDamaged; }

    void markAsReturned(bool damaged = false /* Neu khong truyen tham so thi mac dinh no la false */) {
        this->isReturned = true;
        this->isDamaged = damaged;
    }

    /* Lien quan toi viec check-out room, khi khach hang check-out, ta se kiem tra qua cac status cua items
    (minibar, furniture,...):
    - Neu phat hien chua RETURN -> yeu cau nhan vien len return lai mon do
    - Neu DA RETURN nhung isDamaged == true -> khoi tao doi tuong DamagePenaltyItem
    */
};

class DamagePenaltyItem : public ServiceItem {
public:
    DamagePenaltyItem(int id, string name, double price, int quantity, string note)
    : ServiceItem(id, name, price, quantity, note) {
        /* Bien note o day hoat dong khac so voi bien note o cac class ServiceItem khac. Note o day la ghi chu
        ly do tai sao lai phat tien. VD: note = "Ban` la` bi mat" */
        if(note.empty()) {
            // Bao loi vi phat tien bat buoc phai co ghi chu ly do ro rang
        }
    }
};

class ServiceItemFactory {
public:
    static unique_ptr<ServiceItem> createServiceItem(
    ServiceType type, int id, string name, double price, int quantity, string note) {
        switch (type) {
            case ServiceType::FoodOrderItem:
                return make_unique<FoodOrderItem>(id, name, price, quantity, note);
            case ServiceType::MinibarItem:
                return make_unique<MinibarItem>(id, name, price, quantity, note);
            case ServiceType::FurnitureItem:
                return make_unique<FurnitureItem>(id, name, price, quantity, note);
            case ServiceType::DamagePenaltyItem:
                return make_unique<DamagePenaltyItem>(id, name, price, quantity, note);
            default:
                return nullptr;
        }
    }
};