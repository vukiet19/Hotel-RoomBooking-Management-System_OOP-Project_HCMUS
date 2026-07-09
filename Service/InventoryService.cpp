#include "InventoryService.h"
#include "../Manager/DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDateTime>
#include <QDebug>
#include <QVariant>

// Kiểm tra tồn kho
bool InventoryService::checkStock(const QString& itemName, int quantity) {
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    query.prepare("SELECT quantity FROM Inventory WHERE item_name = :name");
    query.bindValue(":name", itemName);

    if (query.exec() && query.next()) {
        int currentStock = query.value(0).toInt();
        return currentStock >= quantity; // Trả về true nếu kho còn đủ đồ
    }
    
    qDebug() << "ERR: Khong tim thay mon do hoac loi truy van:" << itemName;
    return false; // Không tìm thấy đồ thì báo false luôn
}

// Xuất kho và ghi data lên InventoryLog trên database
bool InventoryService::reserveItem(const QString& itemName, int quantity) {
    // Nếu Inventory không đủ đồ thì từ chối xuất kho
    if (!checkStock(itemName, quantity)) {
        qDebug() << "ERR: Trong kho khong du so luong cho" << itemName;
        return false;
    }

    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);
    
    // Bắt đầu một Transaction (Giao dịch an toàn)
    db.transaction(); 

    // Lấy item_id của món đồ này để ghi vào InventoryLog trên database
    query.prepare("SELECT item_id FROM Inventory WHERE item_name = :name");
    query.bindValue(":name", itemName);
    int itemId = -1;
    if (query.exec() && query.next()) {
        itemId = query.value(0).toInt();
    } else {
        db.rollback(); return false;
    }

    // Trừ số lượng trong bảng Inventory trên database
    query.prepare("UPDATE Inventory SET quantity = quantity - :qty WHERE item_id = :id");
    query.bindValue(":qty", quantity);
    query.bindValue(":id", itemId);
    if (!query.exec()) {
        qDebug() << "ERROR: Khong tru so luong trong bang Inventory duoc!" << query.lastError().text();
        db.rollback(); return false;
    }

    // Ghi lịch sử lấy item từ Inventory vào bảng InventoryLog
    QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    query.prepare("INSERT INTO InventoryLog (item_id, quantity, action_type, date) "
                  "VALUES (:id, :qty, 'RESERVE', :date)");
    query.bindValue(":id", itemId);
    query.bindValue(":qty", quantity);
    query.bindValue(":date", currentDate);
    if (!query.exec()) {
        qDebug() << "ERROR: Khong the ghi lich su lay item tu Inventory len InventoryLog (database)!" << query.lastError().text();
        db.rollback(); return false;
    }

    // Nếu cả trừ kho và ghi log đều mượt mà, ta mới chốt lưu vĩnh viễn (Commit)
    db.commit();
    qDebug() << "SUCCESS: Da xuat kho va ghi log thanh cong cho:" << itemName;
    return true;
}

// Hoàn kho và cập nhật lại InventoryLog
bool InventoryService::releaseItem(const QString& itemName, int quantity) {
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);
    
    db.transaction();

    // Lấy item_id trong bảng Inventory
    query.prepare("SELECT item_id FROM Inventory WHERE item_name = :name");
    query.bindValue(":name", itemName);
    int itemId = -1;
    if (query.exec() && query.next()) {
        itemId = query.value(0).toInt();
    } else {
        db.rollback(); return false;
    }

    // Cộng trả lại số lượng vào Inventory
    query.prepare("UPDATE Inventory SET quantity = quantity + :qty WHERE item_id = :id");
    query.bindValue(":qty", quantity);
    query.bindValue(":id", itemId);
    if (!query.exec()) {
        db.rollback(); return false;
    }

    // Ghi lịch sử RELEASE vào InventoryLog
    QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    query.prepare("INSERT INTO InventoryLog (item_id, quantity, action_type, date) "
                  "VALUES (:id, :qty, 'RELEASE', :date)");
    query.bindValue(":id", itemId);
    query.bindValue(":qty", quantity);
    query.bindValue(":date", currentDate);
    if (!query.exec()) {
        db.rollback(); return false;
    }

    db.commit();
    qDebug() << "SUCCESS: Da hoan tra kho va ghi log thanh cong cho:" << itemName;
    return true;
}