#include "InventoryService.h"
#include "backend/Manager/DatabaseManager.h"
#include <QDateTime>
#include <QDebug>
#include <QVariant>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// Kiểm tra tồn kho
bool InventoryService::checkStock(const QString &itemName, int quantity) {
  QSqlDatabase db = DatabaseManager::instance().database();
  QSqlQuery query(db);

  query.prepare("SELECT quantity FROM Inventory WHERE item_name = :name");
  query.bindValue(":name", itemName);

  if (query.exec() && query.next()) {
    int currentStock = query.value(0).toInt();
    return currentStock >= quantity; // Trả về true nếu kho còn đủ đồ
  }

  qDebug() << "ERR: Item not found or query failed:" << itemName;
  return false;
}

// Reserve inventory item and record log entry in database
bool InventoryService::reserveItem(const QString &itemName, int quantity) {
  if (!checkStock(itemName, quantity)) {
    qDebug() << "ERR: Insufficient inventory quantity for:" << itemName;
    return false;
  }

  QSqlDatabase db = DatabaseManager::instance().database();
  QSqlQuery query(db);

  db.transaction();

  // Lấy item_id của món đồ này để ghi vào InventoryLog trên database
  query.prepare("SELECT item_id FROM Inventory WHERE item_name = :name");
  query.bindValue(":name", itemName);
  int itemId = -1;
  if (query.exec() && query.next()) {
    itemId = query.value(0).toInt();
  } else {
    db.rollback();
    return false;
  }

  // Trừ số lượng trong bảng Inventory trên database
  query.prepare(
      "UPDATE Inventory SET quantity = quantity - :qty WHERE item_id = :id");
  query.bindValue(":qty", quantity);
  query.bindValue(":id", itemId);
  if (!query.exec()) {
    qDebug() << "ERROR: Failed to deduct inventory quantity:"
             << query.lastError().text();
    db.rollback();
    return false;
  }

  // Ghi lịch sử lấy item từ Inventory vào bảng InventoryLog
  QString currentDate =
      QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
  query.prepare(
      "INSERT INTO InventoryLog (item_id, quantity, action_type, date) "
      "VALUES (:id, :qty, 'ADD_TO_BOOKING', :date)");
  query.bindValue(":id", itemId);
  query.bindValue(":qty", -quantity);
  query.bindValue(":date", currentDate);
  if (!query.exec()) {
    qDebug() << "ERROR: Failed to log inventory item removal:"
             << query.lastError().text();
    db.rollback();
    return false;
  }

  // Nếu cả trừ kho và ghi log đều mượt mà, ta mới chốt lưu vĩnh viễn (Commit)
  db.commit();
  qDebug() << "SUCCESS: Inventory item reserved and logged successfully for:"
           << itemName;
  return true;
}

// Hoàn kho và cập nhật lại InventoryLog
bool InventoryService::releaseItem(const QString &itemName, int quantity) {
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
    db.rollback();
    return false;
  }

  // Cộng trả lại số lượng vào Inventory
  query.prepare(
      "UPDATE Inventory SET quantity = quantity + :qty WHERE item_id = :id");
  query.bindValue(":qty", quantity);
  query.bindValue(":id", itemId);
  if (!query.exec()) {
    db.rollback();
    return false;
  }

  // Ghi lịch sử RELEASE vào InventoryLog
  QString currentDate =
      QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
  query.prepare(
      "INSERT INTO InventoryLog (item_id, quantity, action_type, date) "
      "VALUES (:id, :qty, 'RELEASE', :date)");
  query.bindValue(":id", itemId);
  query.bindValue(":qty", quantity);
  query.bindValue(":date", currentDate);
  if (!query.exec()) {
    db.rollback();
    return false;
  }

  db.commit();
  qDebug() << "SUCCESS: Inventory item released and logged successfully for:"
           << itemName;
  return true;
}

// Ghi nhận hư hỏng vật phẩm: trừ kho và ghi log DAMAGE
// linkedInventoryId > 0 : dùng trực tiếp item_id này (Damage service được
//                         map sẵn với furniture trong Inventory).
// linkedInventoryId <= 0: fallback tìm theo item_name trong Inventory
//                         (đường dẫn cũ, giữ backward compat).
bool InventoryService::damageItem(const QString &itemName, int quantity,
                                  int linkedInventoryId) {
  QSqlDatabase db = DatabaseManager::instance().database();
  QSqlQuery query(db);

  qDebug() << "[DAMAGE] damageItem() called:"
           << "serviceName=" << itemName
           << "quantity=" << quantity
           << "linkedInventoryId=" << linkedInventoryId;

  db.transaction();

  int itemId = -1;
  bool foundInInventory = false;

  if (linkedInventoryId > 0) {
    // Ưu tiên: dùng linkedInventoryId (Damage service đã map với furniture)
    query.prepare("SELECT item_id, item_name FROM Inventory WHERE item_id = :id");
    query.bindValue(":id", linkedInventoryId);
    if (query.exec() && query.next()) {
      itemId = linkedInventoryId;
      foundInInventory = true;
      qDebug() << "[DAMAGE] Found mapped inventory item:"
               << "item_id=" << itemId
               << "item_name=" << query.value("item_name").toString();
    } else {
      qDebug() << "[WARN] linkedInventoryId" << linkedInventoryId
               << "not found in Inventory — falling back to name lookup."
               << "SQL error:" << query.lastError().text();
    }
  } else {
    qDebug() << "[DAMAGE] No linkedInventoryId provided (value=" << linkedInventoryId
             << "), trying name-based lookup...";
  }

  if (!foundInInventory) {
    // Fallback 1: tìm theo item_name chính xác (backward compat)
    query.prepare("SELECT item_id FROM Inventory WHERE item_name = :name");
    query.bindValue(":name", itemName);
    if (query.exec() && query.next()) {
      itemId = query.value(0).toInt();
      foundInInventory = true;
      qDebug() << "[DAMAGE] Fallback 1: found exact name match, item_id=" << itemId;
    } else {
      qDebug() << "[DAMAGE] Fallback 1: no exact name match for '" << itemName << "'";
    }
  }

  if (!foundInInventory) {
    // Fallback 2: Fuzzy match cho các dịch vụ Damage với Furniture
    // (Xử lý trường hợp data cũ không có linked_inventory_id)
    query.prepare("SELECT item_id, item_name FROM Inventory WHERE category = 'Furniture'");
    if (query.exec()) {
      QString lowerItem = itemName.toLower();
      QString coreItemName = lowerItem;
      // Bỏ tiền tố "hư " hoặc "hu " để match tốt hơn (vd: "Hư máy PS5" -> "máy ps5")
      if (coreItemName.startsWith("hư ")) {
        coreItemName = coreItemName.mid(3).trimmed();
      } else if (coreItemName.startsWith("hu ")) {
        coreItemName = coreItemName.mid(3).trimmed();
      }

      qDebug() << "[DAMAGE] Fallback 2: fuzzy matching with coreItemName='" << coreItemName << "'";

      while (query.next()) {
        QString invName = query.value("item_name").toString();
        QString lowerInv = invName.toLower();

        // Nếu tên Furniture chứa tên Damage (đã bỏ tiền tố) hoặc ngược lại
        if ((!coreItemName.isEmpty() && lowerInv.contains(coreItemName)) || 
            lowerItem.contains(lowerInv)) {
          itemId = query.value("item_id").toInt();
          foundInInventory = true;
          qDebug() << "[DAMAGE] Fuzzy matched damage item '" << itemName 
                   << "' to inventory item '" << invName << "' (item_id=" << itemId << ")";
          break;
        }
      }
      if (!foundInInventory) {
        qDebug() << "[DAMAGE] Fallback 2: no fuzzy match found in Furniture category.";
      }
    }
  }

  if (foundInInventory) {
    // Trừ số lượng trong bảng Inventory (hàng bị hỏng)
    query.prepare(
        "UPDATE Inventory SET quantity = quantity - :qty WHERE item_id = :id");
    query.bindValue(":qty", quantity);
    query.bindValue(":id", itemId);
    if (!query.exec()) {
      qDebug() << "ERROR: Failed to deduct damaged inventory quantity:"
               << query.lastError().text();
      db.rollback();
      return false;
    }
  } else {
    qDebug() << "[INFO] Damage item '" << itemName
             << "' not found in Inventory — ghi log DAMAGE không kèm trừ kho.";
  }

  // Ghi log DAMAGE vào InventoryLog.
  // Luôn ghi service_name để hiển thị matching giữa damage và furniture.
  QString currentDate =
      QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

  if (foundInInventory) {
    query.prepare(
        "INSERT INTO InventoryLog (item_id, quantity, action_type, date, service_name) "
        "VALUES (:id, :qty, 'DAMAGE', :date, :sname)");
    query.bindValue(":id", itemId);
    query.bindValue(":qty", -quantity);
    query.bindValue(":date", currentDate);
    query.bindValue(":sname", itemName);
  } else {
    // Thử ghi với item_id = NULL; ghi tên service vào cột service_name
    query.prepare(
        "INSERT INTO InventoryLog (item_id, quantity, action_type, date, service_name) "
        "VALUES (NULL, :qty, 'DAMAGE', :date, :sname)");
    query.bindValue(":qty", -quantity);
    query.bindValue(":date", currentDate);
    query.bindValue(":sname", itemName);
  }

  if (!query.exec()) {
    qDebug() << "ERROR: Failed to log DAMAGE action:"
             << query.lastError().text();
    db.rollback();
    return false;
  }

  db.commit();
  qDebug() << "SUCCESS: Damage recorded and logged for:" << itemName
           << "x" << quantity
           << (foundInInventory ? "(inventory deducted)" : "(log only, no inventory)");
  return true;
}

