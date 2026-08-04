// File này dùng để tạo/cập nhật item lên bảng Inventory trên database
// Sử dụng DatabaseManager để quản lý file database

#include "InventoryRepository.h"
#include "../Manager/DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QDateTime>

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
        qDebug() << "ERR: Khong them duoc item vao kho:" << query.lastError().text();
        return false;
    }

    int newId = query.lastInsertId().toInt();
    
    // Ghi InventoryLog
    QSqlQuery logQuery(DatabaseManager::instance().database());
    logQuery.prepare("INSERT INTO InventoryLog (item_id, quantity, action_type, date) VALUES (?, ?, 'ADD_NEW', ?)");
    logQuery.addBindValue(newId);
    logQuery.addBindValue(quantity); 
    logQuery.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    logQuery.exec();
    
    // TỰ ĐỘNG ĐĂNG KÝ VÀO MENU SERVICE
    QSqlQuery serviceQuery(DatabaseManager::instance().database());
    serviceQuery.prepare("INSERT INTO ServiceCatalog (item_id, item_name, category, base_price, vip_free_status) VALUES (?, ?, ?, ?, 0)");
    serviceQuery.addBindValue(QString::number(newId));
    serviceQuery.addBindValue(name);
    serviceQuery.addBindValue(itemType);
    serviceQuery.addBindValue(price);
    serviceQuery.exec();

    return true;
}


// Cập nhật thông tin của một item có sẵn trong bảng Inventory trên database
// chắc chắn rằng số lượng và giá đưa vào > 0 (vì <= 0 thì không thể có để bán)
bool InventoryRepository::updateItem(int itemId, int newQuantity, double newPrice) {
    if (newQuantity < 0 || newPrice < 0) return false;

    QSqlDatabase db = DatabaseManager::instance().database();
    
    QSqlQuery getOld(db);
    getOld.prepare("SELECT quantity FROM Inventory WHERE item_id = :id");
    getOld.bindValue(":id", itemId);
    int oldQty = 0;
    if (getOld.exec() && getOld.next()) oldQty = getOld.value(0).toInt();
    int delta = newQuantity - oldQty;

    QSqlQuery query(db);
    query.prepare("UPDATE Inventory SET quantity = :newQuantity, price = :newPrice WHERE item_id = :itemId");
    query.bindValue(":newQuantity", newQuantity);
    query.bindValue(":newPrice", newPrice);
    query.bindValue(":itemId", itemId);

    if (query.exec()) {
        // ĐỒNG BỘ GIÁ MỚI SANG MENU SERVICE
        QSqlQuery updateService(db);
        updateService.prepare("UPDATE ServiceCatalog SET base_price = :newPrice WHERE item_id = :itemId");
        updateService.bindValue(":newPrice", newPrice);
        updateService.bindValue(":itemId", QString::number(itemId));
        updateService.exec();

        // Ghi Log nếu có thay đổi số lượng
        if (delta != 0) {
            QSqlQuery logQuery(db);
            logQuery.prepare("INSERT INTO InventoryLog (item_id, quantity, action_type, date) VALUES (?, ?, 'UPDATE_QTY', ?)");
            logQuery.addBindValue(itemId);
            logQuery.addBindValue(delta); 
            logQuery.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
            logQuery.exec();
        }
        return true;
    }
    return false;
}