#pragma once
#include <QString>

class InventoryService {
public:
    // 1. Kiểm tra xem kho có đủ hàng không
    bool checkStock(const QString& itemName, int quantity);

    // 2. Trừ hàng trong kho và ghi log ADD_TO_BOOKING
    bool reserveItem(const QString& itemName, int quantity);

    // 3. Cộng lại hàng vào kho và ghi log RELEASE
    bool releaseItem(const QString& itemName, int quantity);

    // 4. Ghi nhận hư hỏng: trừ kho và ghi log DAMAGE
    // linkedInventoryId: item_id trong bảng Inventory của furniture bị hư.
    // Nếu = -1 (mặc định): fallback tìm theo itemName trong Inventory.
    bool damageItem(const QString& itemName, int quantity, int linkedInventoryId = -1);
};