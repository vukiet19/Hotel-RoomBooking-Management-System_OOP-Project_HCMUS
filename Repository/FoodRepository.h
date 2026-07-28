#pragma once
#include <QString>
#include <QVector>

struct Food
{
    QString id;
    QString name;
    QString category;
    double basePrice;
};

class FoodRepository
{
private:
    QString foodId;
    QString name;
    QString category;
    double basePrice;

public:
    FoodRepository() = default;
    FoodRepository(QString id, QString n, QString c, double p);

    bool add();

    bool update(const QString &bookingId, const QString &foodId, int category);
    bool delete_f(const QString &targetId);
    QVector<Food> filter(double minPrice, double maxPrice);
};