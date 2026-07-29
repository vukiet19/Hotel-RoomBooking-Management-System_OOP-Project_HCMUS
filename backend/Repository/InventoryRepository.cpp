// File này dùng để tạo/cập nhật item lên bảng Inventory trên database
// Sử dụng DatabaseManager để quản lý file database

#include "InventoryRepository.h"
#include "../Manager/DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

// Tạo một item mới trong table Inventory trên database
bool InventoryRepository::insertItem(const QString& name, int quantity, double price) {
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("INSERT INTO Inventory (item_name, quantity, price) VALUES (:name, :qty, :price)");
    query.bindValue(":name", name);
    query.bindValue(":qty", quantity);
    query.bindValue(":price", price);

    if (!query.exec()) {
        qDebug() << "ERR: Khong them duoc item vao kho:" << query.lastError().text();
        return false;
    }
    return true;
}

// Cập nhật số lượng của một item có sẵn trong bảng Inventory trên database
bool InventoryRepository::updateQuantity(int itemId, int newQuantity) {
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE Inventory SET quantity = :qty WHERE item_id = :id");
    query.bindValue(":qty", newQuantity);
    query.bindValue(":id", itemId);
    
    return query.exec();
}