/* 
File này phục vụ cho việc quản lý các dịch vụ, hàng hóa, phí phạt trong quá trình sử dụng
khách sạn. ServiceItem sẽ được sử dụng dưới dạng một vector trong class Booking.

Các file liên quan: SerivceEnums.h (chứa trạng thái và kiểu dịch vụ)
*/
#include "ServiceItem.h"

ServiceItem::ServiceItem(string id, string name, double price, int quantity, string note) {
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

double ServiceItem::getSubtotal() const {
    return quantity * unitPrice;
}

string ServiceItem::getId() const {
    return id;
}

string ServiceItem::getName() const {
    return name;
}

double ServiceItem::getUnitPrice() const {
    return unitPrice;
}

int ServiceItem::getQuantity() const {
    return quantity;
}

ServiceStatus ServiceItem::getStatus() const {
    return status;
}

string ServiceItem::getNote() const {
    return note;
}

void ServiceItem::setId(const string& newId) {
    if (newId.empty()) {
        // thong bao loi tren UI
        return;
    }
    this->id = newId;
}

void ServiceItem::setName(const string& newName) {
    if (newName.empty()) {
        // thong bao loi tren UI
        return;
    }
    this->name = newName;
}

void ServiceItem::setUnitPrice(double newPrice) {
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

void ServiceItem::setQuantity(int newQuantity) {
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

void ServiceItem::changeStatus(ServiceStatus newStatus) {
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

void ServiceItem::setNote(string note) {
    this->note = note;
}

ServiceItem::~ServiceItem() = default;

FoodOrderItem::FoodOrderItem(string id, string name, double price, int quantity, string note)
    : ServiceItem(id, name, price, quantity, note) {}

double FoodOrderItem::getSubtotal() const {
    double base = ServiceItem::getSubtotal();

    // Vi du neu dung dich vu goi do an thi cong them 5% phi dich vu
    return base * 1.05;
}

MinibarItem::MinibarItem(string id, string name, double price, int quantity, string note)
    : ServiceItem(id, name, price, quantity, note) {
    /* Ly do khoi tao false la khi mot mon do duoc khoi tao, tuc la khach hang da su
    dung no roi. Vi the trang thai cua no se la CHUA DUOC restock */
    this->isRestocked = false;
}
// isRestocked quan ly Object tung mon hang
bool MinibarItem::getRestockedStatus() const {
    return isRestocked;
}

void MinibarItem::markAsRestocked() {
    this->isRestocked = true;
}

FurnitureItem::FurnitureItem(string id, string name, double price, int quantity, string note)
    : ServiceItem(id, name, price, quantity, note) {
    /* Ly do khoi tao false la khi mot mon do noi that duoc khoi tao tuc la no dang duoc khach hang dung,
    chua duoc tra ve kho. Vi the trang thai cua no se la CHUA DUOC return */
    this->isReturned = false;
    this->isDamaged = false;
}

bool FurnitureItem::getReturnStatus() const {
    return isReturned;
}

bool FurnitureItem::getDamagedStatus() const {
    return isDamaged;
}
// Neu nhu isDamaged == true --> Khoi tao Object DamagePenaltyItem

void FurnitureItem::markAsReturned(bool damaged /* Neu khong truyen tham so thi mac dinh no la false */) {
    this->isReturned = true;
    this->isDamaged = damaged;
}

DamagePenaltyItem::DamagePenaltyItem(string id, string name, double price, int quantity, string note)
    : ServiceItem(id, name, price, quantity, note) {
    /* Bien note o day hoat dong khac so voi bien note o cac class ServiceItem khac. Note o day la ghi chu
    ly do tai sao lai phat tien. VD: note = "Ban` la` bi mat" */
    if(note.empty()) {
        // Bao loi vi phat tien bat buoc phai co ghi chu ly do ro rang
    } 
    // Can nhac lai bien note o DamagePenaltyItem
}

unique_ptr<ServiceItem> ServiceItemFactory::createServiceItem(
    ServiceType type, string id, string name, double price, int quantity, string note) {
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
