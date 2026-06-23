#pragma once
#include "ServiceItem.h"
#include <string>

class IServiceItemFactory {
public:
    virtual ~IServiceItemFactory() = default;
    virtual ServiceItem* createServiceItem(ServiceType type, std::string name, int quantity) = 0;
};

class PricingDatabaseManager {
public:
    // giả lập sqlite lookup / query 
    static double fetchPrice(ServiceType type, std::string name);
};

// cơ chế query price, diễn dịch ServiceType cần bàn thêm
class ServiceItemFactory : public IServiceItemFactory {
public:
    ServiceItem* createServiceItem(ServiceType type, std::string name, int quantity) override;
};