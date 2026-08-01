#include "FoodRepository.h"
#include "../Manager/DatabaseManager.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

// Constructor khởi tạo
FoodRepository::FoodRepository(QString id, QString n, QString c, double p)
    : foodId(id), name(n), category(c), basePrice(p) {}

// 1. ADD FOOD
bool FoodRepository::add() {
  QSqlQuery query;

  query.prepare("INSERT INTO FoodOptions (option_id, parent_item_id, "
                "option_name, extra_price) "
                "VALUES (:optId, :parentId, :optName, :extraPrice)");

  query.bindValue(":optId", this->foodId);
  query.bindValue(":parentId", this->category);
  query.bindValue(":optName", this->name);
  query.bindValue(":extraPrice", this->basePrice);

  if (!query.exec()) {
    qDebug() << "Add Food Error:" << query.lastError().text();
    return false;
  }

  qDebug() << "[SUCCESS] Food added to FoodOptions successfully!";
  return true;
}

bool FoodRepository::update(const QString &bookingId, const QString &foodId,
                            const QString &quantityStr) {
  if (!DatabaseManager::instance().open()) {
    qDebug() << "[ERROR] Database connection failed in FoodRepository!";
    return false;
  }

  bool okQty = false;
  int newQty = quantityStr.toInt(&okQty);
  if (!okQty || newQty < 0) {
    qDebug() << "[ERROR] Invalid quantity passed:" << quantityStr;
    return false;
  }

  bool okBId = false;
  int bIdVal = bookingId.toInt(&okBId);
  if (!okBId || bIdVal <= 0) {
    qDebug() << "[ERROR] Invalid bookingId passed:" << bookingId;
    return false;
  }

  QSqlDatabase db = DatabaseManager::instance().database();

  // 1. Check if food price exists in FoodOptions
  double unitPrice = 0.0;
  QSqlQuery checkFood(db);
  checkFood.prepare("SELECT extra_price FROM FoodOptions WHERE option_id = :fId");
  checkFood.bindValue(":fId", foodId);
  if (checkFood.exec() && checkFood.next()) {
    unitPrice = checkFood.value(0).toDouble();
  }

  // 2. Check if a record already exists in BookingServiceItems for this booking & food item
  QSqlQuery checkExist(db);
  checkExist.prepare(
      "SELECT id FROM BookingServiceItems WHERE booking_id = :bId AND item_id = :fId");
  checkExist.bindValue(":bId", bookingId);
  checkExist.bindValue(":fId", foodId);

  bool exists = (checkExist.exec() && checkExist.next());

  if (exists) {
    if (newQty == 0) {
      // If quantity is set to 0, delete the service item
      QSqlQuery delQuery(db);
      delQuery.prepare(
          "DELETE FROM BookingServiceItems WHERE booking_id = :bId AND item_id = :fId");
      delQuery.bindValue(":bId", bookingId);
      delQuery.bindValue(":fId", foodId);
      return delQuery.exec();
    } else {
      // Update existing record
      QSqlQuery updQuery(db);
      updQuery.prepare(
          "UPDATE BookingServiceItems "
          "SET quantity = :qty, final_price = :finalPrice "
          "WHERE booking_id = :bId AND item_id = :fId");
      updQuery.bindValue(":qty", newQty);
      updQuery.bindValue(":finalPrice", unitPrice * newQty);
      updQuery.bindValue(":bId", bookingId);
      updQuery.bindValue(":fId", foodId);

      if (!updQuery.exec()) {
        qDebug() << "[ERROR] Failed to update BookingServiceItems:" << updQuery.lastError().text();
        return false;
      }
      qDebug() << "[SUCCESS] Updated quantity to" << newQty << "for Booking:" << bookingId;
      return true;
    }
  } else {
    // If it does not exist yet, insert a new record if quantity > 0
    if (newQty == 0) {
      return true;
    }

    QSqlQuery insQuery(db);
    insQuery.prepare(
        "INSERT INTO BookingServiceItems (booking_id, item_id, quantity, customer_note, final_price) "
        "VALUES (:bId, :fId, :qty, '', :finalPrice)");
    insQuery.bindValue(":bId", bookingId);
    insQuery.bindValue(":fId", foodId);
    insQuery.bindValue(":qty", newQty);
    insQuery.bindValue(":finalPrice", unitPrice * newQty);

    if (!insQuery.exec()) {
      qDebug() << "[ERROR] Failed to insert new Food to BookingServiceItems:" << insQuery.lastError().text();
      return false;
    }
    qDebug() << "[SUCCESS] Created new BookingServiceItem for Booking:" << bookingId << "Food:" << foodId;
    return true;
  }
}

bool FoodRepository::delete_f(const QString &targetId) {
  QSqlQuery query;
  query.prepare("DELETE FROM FoodOptions WHERE option_id = :id");
  query.bindValue(":id", targetId);

  if (!query.exec()) {
    qDebug() << "Delete Food Error:" << query.lastError().text();
    return false;
  }

  if (query.numRowsAffected() == 0) {
    qDebug() << "Delete Food Warning: Food ID not found (" << targetId << ")";
    return false;
  }

  qDebug() << "[SUCCESS] Deleted food successfully with ID:" << targetId;
  return true;
}

// 4. FILTER FOOD BY PRICE RANGE
QVector<Food> FoodRepository::filter(double minPrice, double maxPrice) {
  QVector<Food> resultList;
  QSqlQuery query;

  query.prepare("SELECT option_id, option_name, parent_item_id, extra_price "
                "FROM FoodOptions "
                "WHERE extra_price BETWEEN :minPrice AND :maxPrice "
                "ORDER BY extra_price ASC");

  query.bindValue(":minPrice", minPrice);
  query.bindValue(":maxPrice", maxPrice);

  if (!query.exec()) {
    qDebug() << "Filter Food Error:" << query.lastError().text();
    return resultList;
  }

  while (query.next()) {
    Food f{};
    f.id = query.value("option_id").toString();
    f.name = query.value("option_name").toString();
    f.category = query.value("parent_item_id").toString();
    f.basePrice = query.value("extra_price").toDouble();

    resultList.append(f);
  }

  qDebug() << "[SUCCESS] Filtered" << resultList.size() << "food items.";
  return resultList;
}