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

struct ServiceCatalogFilter {
    string id = "";
    string name = "";
    string category = "";
    double minBasePrice = -1.0;
    double maxBasePrice = -1.0;
    int vipFreeStatus = -1; // -1 means no filter, 0 means false, 1 means true
};

struct BookingServiceItemFilter {
    int id = -1;
    int bookingId = -1;
    string itemId = "";
    int minQuantity = -1;
    int maxQuantity = -1;
    double minFinalPrice = -1.0;
    double maxFinalPrice = -1.0;
    string customerNote = "";
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

    // lọc các dịch vụ trong menu ServiceCatalog theo id, tên, loại, giá và trạng thái miễn phí VIP
    vector<ServiceCatalogData> getFilteredCatalogItems(const ServiceCatalogFilter& filter);

    // ghi một serviceItem được dùng vào bảng BookingServiceItem trong db
    int addBookingServiceItem(const BookingServiceItemData& item);
    
    // lấy toàn bộ item đã được thêm vào một booking thông qua booking_id
    vector<BookingServiceItemData> getItemsByBookingId(int bookingId);

    // lọc các dịch vụ đã được thêm vào booking theo id, bookingId, itemId, số lượng, giá và ghi chú
    vector<BookingServiceItemData> getFilteredBookingServiceItems(const BookingServiceItemFilter& filter);
    
    // xóa dòng của một dịch vụ dựa trên id của dịch vụ đó
    bool removeBookingServiceItem(int id);

    // xóa toàn bộ dịch vụ có trong bookingId được truyền vào
    bool removeItemsByBookingId(int bookingId);
};
