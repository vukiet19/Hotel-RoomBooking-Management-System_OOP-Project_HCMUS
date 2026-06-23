#include "ServiceItem.h"

ServiceItem::ServiceItem(std::string n, int q, double price)
    : name(n), quantity(q), unitPrice(price), status(ServiceStatus::PENDING) {}

double ServiceItem::getSubTotal() const {
    return quantity * unitPrice;
}

void ServiceItem::changeStatus(ServiceStatus newStatus) {
    this->status = newStatus;
}

MinibarItem::MinibarItem(std::string n, int q, double price) : ServiceItem(n, q, price) {}
TransportServiceItem::TransportServiceItem(std::string n, int q, double price) : ServiceItem(n, q, price) {}
FoodOrderItem::FoodOrderItem(std::string n, int q, double price) : ServiceItem(n, q, price) {}
FurnitureItem::FurnitureItem(std::string n, int q, double price) : ServiceItem(n, q, price) {}
DamagePenaltyItem::DamagePenaltyItem(std::string n, int q, double price) : ServiceItem(n, q, price) {}