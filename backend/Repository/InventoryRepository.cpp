// File này dùng để tạo/cập nhật item lên bảng Inventory trên database
// Sử dụng DatabaseManager để quản lý file database

#include "InventoryRepository.h"
#include "../Manager/DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

// Tạo một item mới trong table Inventory trên database
bool InventoryRepository::insertItem(const QString& name, const QString& type, int quantity, double price) {
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("INSERT INTO Inventory (item_name, type, quantity, price) VALUES (:name, :type, :qty, :price)");
    query.bindValue(":name", name);
    query.bindValue(":type", type);
    query.bindValue(":qty", quantity);
    query.bindValue(":price", price);

    if (!query.exec()) {
        qDebug() << "ERR: Khong them duoc item vao kho:" << query.lastError().text();
        return false;
    }
    return true;
}

// Thêm item với đầy đủ thông tin: loại danh mục và số lượng tối thiểu cảnh báo
bool InventoryRepository::insertItemFull(const QString& name, const QString& itemType, int quantity, double price, int minQty) {
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("INSERT INTO Inventory (item_name, item_type, quantity, price, minimum_quantity_required) "
                  "VALUES (:name, :itemType, :qty, :price, :minQty)");
    query.bindValue(":name", name);
    query.bindValue(":itemType", itemType);
    query.bindValue(":qty", quantity);
    query.bindValue(":price", price);
    query.bindValue(":minQty", minQty);

    if (!query.exec()) {
        qDebug() << "ERR: Khong them duoc item (full) vao kho:" << query.lastError().text();
        return false;
    }
    return true;
}


// Cập nhật số lượng của một item có sẵn trong bảng Inventory trên database
//chắc chắn rằng số lượng đưa vào > 0 (vì <= 0 thì không thể có để bán)
bool InventoryRepository::updateQuantity(int itemId, int newQuantity) {
    assert(newQuantity >= 0 && "Số lượng cập nhật vào kho không được nhỏ hơn 0!");

    if (newQuantity < 0) 
        return false;

    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    query.prepare("UPDATE Inventory SET quantity = :newQuantity WHERE item_id = :itemId");
    query.bindValue(":newQuantity", newQuantity);
    query.bindValue(":itemId", itemId);

    return query.exec();
}