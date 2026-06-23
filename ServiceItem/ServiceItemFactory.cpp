#include "ServiceItemFactory.h"

double PricingDatabaseManager::fetchPrice(ServiceType type, std::string name) {
    if (type == ServiceType::MINIBAR && name == "Coke") return 2.50;
    if (type == ServiceType::FOOD_ORDER && name == "Burger") return 15.00;
    if (type == ServiceType::TRANSPORT && name == "Taxi") return 20.00;
    if (type == ServiceType::FURNITURE && name == "Bed") return 300.00;
    if (type == ServiceType::DAMAGE_PENALTY && name == "Broken Glass") return 50.00;
    return 0.00;
}

ServiceItem* ServiceItemFactory::createServiceItem(ServiceType type, std::string name, int quantity) {
    double price = PricingDatabaseManager::fetchPrice(type, name);
    switch(type) {
        case ServiceType::MINIBAR:
            return new MinibarItem(name, quantity, price);
        case ServiceType::TRANSPORT:
            return new TransportServiceItem(name, quantity, price);
        case ServiceType::FOOD_ORDER:
            return new FoodOrderItem(name, quantity, price);
        case ServiceType::FURNITURE:
            return new FurnitureItem(name, quantity, price);
        case ServiceType::DAMAGE_PENALTY:
            return new DamagePenaltyItem(name, quantity, price);
        default:
            return nullptr;
    }
}