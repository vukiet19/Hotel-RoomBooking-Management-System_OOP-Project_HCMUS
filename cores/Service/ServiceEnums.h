//  File này định nghĩa các enum quản lý trạng thái và loại dịch vụ phòng. Được dùng bởi ServiceItem.h
#pragma once
enum class ServiceStatus {
    Pending,
    Billed,
    Cancelled
};

enum class ServiceType {
    FoodOrderItem,
    MinibarItem,
    FurnitureItem,
    DamagePenaltyItem
};