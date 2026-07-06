#pragma once

#include "ServiceItem.h"

#include <string>
#include <optional>
#include <vector>

using namespace std;

// dữ liệu của một dịch vụ trong menu
struct ServiceCatalogData {
    string id;
    string name;
    string category;
    double basePrice;
    bool vipFreeStatus;
};

// dữ liệu của một dịch vụ được khách dùng trong một booking cụ thể
struct BookingServiceItemData {
    int id = -1;
    int bookingId;
    string itemId;
    int quantity;
    string customerNote;
    double finalPrice;
};

class ServiceItemRepository {
public:
    /* 
    lấy toàn bộ item có trong db, phục vụ cho việc làm UI hiển thị ra Menu item.
    Lưu ý là ở đây chỉ lấy thông tin item từ db ra thôi chứ không tạo object nha.
    */
    vector<ServiceCatalogData> getAllCatalogItems();

    // optinal<..> có nghĩa là CHO PHÉP trả về dữ liệu kiểu <..> hoặc không trả về gì cả
    optional<ServiceCatalogData> findCatalogItemById(const string& itemId);

    // ghi một serviceItem được dùng vào bảng BookingServiceItem trong db
    int addBookingServiceItem(const BookingServiceItemData& item);
    
    // lấy toàn bộ item đã được thêm vào một booking thông qua booking_id
    vector<BookingServiceItemData> getItemsByBookingId(int bookingId);
};
