#include "FoodRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSqlRecord>
#include "../Manager/DatabaseManager.h"

// Constructor khởi tạo
FoodRepository::FoodRepository(QString id, QString n, QString c, double p)
    : foodId(id), name(n), category(c), basePrice(p) {}

// 1. ADD FOOD
bool FoodRepository::add()
{
    QSqlQuery query;

    query.prepare("INSERT INTO FoodOptions (option_id, parent_item_id, option_name, extra_price) "
                  "VALUES (:optId, :parentId, :optName, :extraPrice)");

    query.bindValue(":optId", this->foodId);
    query.bindValue(":parentId", this->category);
    query.bindValue(":optName", this->name);
    query.bindValue(":extraPrice", this->basePrice);

    if (!query.exec())
    {
        qDebug() << "Add Food Error:" << query.lastError().text();
        return false;
    }

    qDebug() << "[SUCCESS] Food added to FoodOptions successfully!";
    return true;
}

bool FoodRepository::update(const QString &bookingId, const QString &foodId, const QString &quantityStr)
{
    if (!DatabaseManager::instance().open())
    {
        qDebug() << "[ERROR] Database connection failed in FoodRepository!";
        return false;
    }

    bool ok = false;
    int newQty = quantityStr.toInt(&ok);
    if (!ok || newQty < 0)
    {
        qDebug() << "[ERROR] Invalid quantity passed:" << quantityStr;
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());

    QString sql = R"(
        UPDATE BookingServiceItems
        SET quantity = :qty,
            final_price = (
                SELECT COALESCE(extra_price, 0.0) 
                FROM FoodOptions 
                WHERE option_id = :fId
            ) * :qty
        WHERE booking_id = :bId 
    )";

    query.prepare(sql);
    query.bindValue(":qty", newQty);
    query.bindValue(":bId", bookingId);
    query.bindValue(":fId", foodId);

    if (!query.exec())
    {
        qDebug() << "[ERROR] Failed to update BookingServiceItems:" << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        qDebug() << "[WARN] No record found for Booking ID (" << bookingId << ") and Food ID (" << foodId << ")";
        return false;
    }

    qDebug() << "[SUCCESS] Updated quantity to" << newQty << "and recalculated total price for Booking:" << bookingId;
    return true;
}

bool FoodRepository::delete_f(const QString &targetId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM FoodOptions WHERE option_id = :id");
    query.bindValue(":id", targetId);

    if (!query.exec())
    {
        qDebug() << "Delete Food Error:" << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        qDebug() << "Delete Food Warning: Food ID not found (" << targetId << ")";
        return false;
    }

    qDebug() << "[SUCCESS] Deleted food successfully with ID:" << targetId;
    return true;
}

// 4. FILTER FOOD BY PRICE RANGE
QVector<Food> FoodRepository::filter(double minPrice, double maxPrice)
{
    QVector<Food> resultList;
    QSqlQuery query;

    query.prepare("SELECT option_id, option_name, parent_item_id, extra_price FROM FoodOptions "
                  "WHERE extra_price BETWEEN :minPrice AND :maxPrice "
                  "ORDER BY extra_price ASC");

    query.bindValue(":minPrice", minPrice);
    query.bindValue(":maxPrice", maxPrice);

    if (!query.exec())
    {
        qDebug() << "Filter Food Error:" << query.lastError().text();
        return resultList;
    }

    while (query.next())
    {
        Food f;
        f.id = query.value("option_id").toString();
        f.name = query.value("option_name").toString();
        f.category = query.value("parent_item_id").toString();
        f.basePrice = query.value("extra_price").toDouble();

        resultList.append(f);
    }

    qDebug() << "[SUCCESS] Filtered" << resultList.size() << "food items.";
    return resultList;
}