#pragma once
#include <QString>

class InventoryService {
public:
    // 1. Kiểm tra xem kho có đủ hàng không
    bool checkStock(const QString& itemName, int quantity);

    // 2. Trừ hàng trong kho và ghi log
    bool reserveItem(const QString& itemName, int quantity);

    // 3. Cộng lại hàng vào kho và ghi log
    bool releaseItem(const QString& itemName, int quantity);
};