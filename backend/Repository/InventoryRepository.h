#pragma once
#include <QString>
#include <QtSql/QSqlDatabase>

class InventoryRepository {
public:
    // Thêm một mặt hàng mới vào kho (legacy)
    bool insertItem(const QString& name, const QString& type, int quantity, double price);

    // Thêm mặt hàng đầy đủ thông tin (item_type + minimum_quantity_required)
    bool insertItemFull(const QString& name, const QString& itemType, int quantity, double price, int minQty);

    // Cập nhật thông tin mặt hàng
    bool updateItem(int itemId, int newQuantity, double newPrice);
};
