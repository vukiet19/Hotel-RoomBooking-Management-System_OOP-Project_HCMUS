#pragma once
#include <QString>
#include <QtSql/QSqlDatabase>

class InventoryRepository {
public:
    // Thêm một mặt hàng mới vào kho
    bool insertItem(const QString& name, int quantity, double price);
    
    // Cập nhật số lượng
    bool updateQuantity(int itemId, int newQuantity);
};