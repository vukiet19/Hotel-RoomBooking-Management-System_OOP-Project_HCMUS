#include "Booking.h"

Booking::Booking(Customer* c) : customer(c), status(BookingStatus::UNCONFIRMED) {}

StandardRoomBooking::~StandardRoomBooking() {
    if(this->room->getStatus() == RoomStatus::Occupied){
        //LƯU THÔNG TIN KHÁCH HÀNG VÀO DATABASE
    }

    //không set khi chưa maintenance xong vì có thể phòng dơ chẳng hạn
    if (this->room->getStatus() == RoomStatus::Occupied || 
        this->room->getStatus() == RoomStatus::Reserved) {
        this->room->setStatus(RoomStatus::Available);
    }
}

int Booking::getId() const{
    return this->id;
}

Customer* Booking::getCustomer() const{
    return this->customer;
}

BookingStatus Booking::getBookingStatus() const{
    return this->status;
};

//thêm serviceitems
void Booking::addServiceItem(unique_ptr<ServiceItem> serviceItem) {
    if (serviceItem != nullptr) {
        serviceItems.push_back(move(serviceItem));
    }
}

void Booking::addDamagePenaltyItems() {
    //ta không thể add trực tiếp vào serviceItems vì
    //cơ chế vector là nếu hết chỗ trong ram sẽ di chuyển sang chỗ mới rộng hơn
    //nên nếu vừa duyệt vừa add thì ta sẽ đọc ô nhớ rác vì đã chuyển
    //nên tạo vector penalties lưu phạt riêng
    vector<unique_ptr<ServiceItem>> penalties; 

    for(auto& item : this->serviceItems) {
        ServiceItem* ptr = item.get();
        FurnitureItem* furnitureItem = dynamic_cast<FurnitureItem*>(ptr);
        
        //cơ chế downcasting an toàn trả về nullptr nếu downcast sai datatype
        if(furnitureItem != nullptr && furnitureItem->getDamagedStatus()) {
            unique_ptr<ServiceItem> newDamageItem = ServiceItemFactory::createServiceItem(
                ServiceType::DamagePenaltyItem, 
                furnitureItem->getId(), 
                furnitureItem->getName(),
                furnitureItem->getUnitPrice(), 
                furnitureItem->getQuantity(), 
                furnitureItem->getNote()
            );
            penalties.push_back(move(newDamageItem)); 
        }
    }

    for(auto& penalty : penalties) {
        //chuyển nhượng quyền sở hữu an toàn cho vector
        this->serviceItems.push_back(move(penalty));
    }
}

const vector<unique_ptr<ServiceItem>>& Booking::getServiceItem() const {
    return serviceItems;
}

Room* StandardRoomBooking::getRoom() const{
    return this->room;
}

DepositStatus StandardRoomBooking::getDepositStatus() const {
    return this->depositStatus;
}

StandardRoomBooking::StandardRoomBooking(Customer* c, Room* r, QDateTime in, QDateTime out, double depositAmount) 
: Booking(c), room(r), checkInTime(in), checkOutTime(out), depositAmount(depositAmount) {
    
    //>0 nghĩa là mình đang giữ tiền cọc của khách
    if(this->depositAmount > 0.00){
        this->depositStatus = DepositStatus::HELD;
        this->room->setStatus(RoomStatus::Reserved);
    }
}

//REFUND

void StandardRoomBooking::cancelBooking() {
    if(this->room->getStatus() == RoomStatus::Reserved){
        this->room->setStatus(RoomStatus::Available);
    }
}
//hàm resolve thực thi khi và set room/deposit status đúng trạng thái
void StandardRoomBooking::resolveDeposit() {
    if(this->room->getStatus() == RoomStatus::Reserved){
        this->depositStatus = DepositStatus::RETURNED; 
    }
}


int StandardRoomBooking::getNights() const {
    return checkInTime.daysTo(checkOutTime);
}

WalkInTab::WalkInTab(Customer* c,QDateTime in) : Booking(c), dateCreated(in) {}

int WalkInTab::getNights() const {
    return 0;
}


//check in/out

//check in thì resolve deposit (quá hợp lý =))
void StandardRoomBooking::checkIn() {
    this->status = BookingStatus::CHECKED_IN;
    this->room->setStatus(RoomStatus::Occupied);
    this->resolveDeposit(); 
}

void StandardRoomBooking::checkOut() {
    this->status = BookingStatus::CHECKED_OUT;
    if(this->room) {
        this->room->setStatus(RoomStatus::Maintenance);
    }
    Booking::addDamagePenaltyItems();
}

//Checkout -> maintenace 
//maintenace() -> ktra dvu + damamge penalty item

void WalkInTab::checkIn() {
    this->status = BookingStatus::CHECKED_IN;
}

void WalkInTab::checkOut() {
    this->status = BookingStatus::CHECKED_OUT;
    Booking::addDamagePenaltyItems();
}