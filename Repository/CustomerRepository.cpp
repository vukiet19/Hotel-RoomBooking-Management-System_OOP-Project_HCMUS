#include "CustomerRepository.h"
#include "../Manager/DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QVariant>

//Chuyển đổi enum MembershipTier sang dạng QString để lưu trữ trong database
static QString tierToString(MembershipTier tier)
{
    switch (tier)
    {
    case MembershipTier::Silver:
        return "SILVER";
    case MembershipTier::Gold:
        return "GOLD";
    case MembershipTier::Platinum:
        return "PLATINUM";
    default:
        return "UNKNOWN";
    }
}

//Chuyển đổi chuỗi text từ database ngược lại thành enum
static MembershipTier stringToTier(const QString &str)
{
    if (str == "SILVER")
        return MembershipTier::Silver;
    if (str == "GOLD")
        return MembershipTier::Gold;
    if (str == "PLATINUM")
        return MembershipTier::Platinum;
    return MembershipTier::Unknown;
}

//verify schema để check có thiếu field nào ko thì add vô
CustomerRepository::CustomerRepository()
{
    verifySchema();
}

//tự động tạo thêm cột 'points' và 'membership_tier' nếu chưa tồn tại
void CustomerRepository::verifySchema()
{
    QSqlDatabase db = DatabaseManager::instance().database();
    if (!db.isOpen())
    {
        DatabaseManager::instance().open();
    }

    QSqlQuery query(db);
    if (!query.exec("PRAGMA table_info(Customer)"))
    {
        qDebug() << "ERROR: Failed to run PRAGMA table_info on Customer!" << query.lastError().text();
        return;
    }

    bool hasPoints = false;
    bool hasTier = false;

    while (query.next())
    {
        QString name = query.value("name").toString();
        if (name == "points")
            hasPoints = true;
        else if (name == "membership_tier")
            hasTier = true;
    }

    QSqlQuery alterQuery(db);
    // Nếu thiếu cột 'points', thực hiện ALTER TABLE để thêm cột mới với giá trị mặc định là 0
    if (!hasPoints)
    {
        if (!alterQuery.exec("ALTER TABLE Customer ADD COLUMN points INTEGER DEFAULT 0"))
        {
            qDebug() << "Failed to add column points:" << alterQuery.lastError().text();
        }
    }
    // Nếu thiếu cột 'membership_tier', thực hiện ALTER TABLE để thêm cột mới với giá trị mặc định là 'UNKNOWN'
    if (!hasTier)
    {
        if (!alterQuery.exec("ALTER TABLE Customer ADD COLUMN membership_tier TEXT DEFAULT 'UNKNOWN'"))
        {
            qDebug() << "Failed to add column membership_tier:" << alterQuery.lastError().text();
        }
    }
}

// Hàm add cũ (giữ lại ko conflict code cũ, nhận tham số dạng Value)
bool CustomerRepository::add(Customer customer)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    query.prepare("INSERT INTO Customer (id_customer, full_name, phone_number, id_card, id_room, points, membership_tier)"
                  "VALUES (:id_customer, :full_name, :phone_number, :id_card, :id_room, :points, :membership_tier)");

    query.bindValue(":id_customer", customer.getId());
    query.bindValue(":full_name", QString::fromStdString(customer.getFullname()));
    query.bindValue(":phone_number", QString::fromStdString(customer.getPhone()));
    query.bindValue(":id_card", QString::fromStdString(customer.getIdcard()));
    query.bindValue(":id_room", QString::fromStdString(customer.getIdRoom()));
    query.bindValue(":points", customer.getPoint());
    query.bindValue(":membership_tier", tierToString(customer.getTier()));

    if (!query.exec())
    {
        qDebug() << "ERROR: Khong the ghi data Customer!" << query.lastError().text();
        return false;
    }

    return true;
}

// Hàm add mới (nhận con trỏ Customer*, tương tự BookingRepository::add)
int CustomerRepository::add(Customer *customer)
{
    if (!customer)
        return -1;

    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    // Chuẩn bị câu lệnh SQL INSERT sử dụng bind variable để chống SQL injection
    query.prepare("INSERT INTO Customer (id_customer, full_name, phone_number, id_card, id_room, points, membership_tier) "
                  "VALUES (:id_customer, :full_name, :phone_number, :id_card, :id_room, :points, :membership_tier)");

    query.bindValue(":id_customer", customer->getId());
    query.bindValue(":full_name", QString::fromStdString(customer->getFullname()));
    query.bindValue(":phone_number", QString::fromStdString(customer->getPhone()));
    query.bindValue(":id_card", QString::fromStdString(customer->getIdcard()));
    query.bindValue(":id_room", QString::fromStdString(customer->getIdRoom()));
    query.bindValue(":points", customer->getPoint());
    query.bindValue(":membership_tier", tierToString(customer->getTier()));

    if (!query.exec())
    {
        qDebug() << "ERROR: Failed to add Customer!" << query.lastError().text();
        return -1;
    }

    // Trả về PRIMARY KEY từ database
    return query.lastInsertId().toInt();
}

// Cập nhật lại thông tin của Customer trong database
bool CustomerRepository::update(Customer *customer)
{
    if (!customer)
        return false;

    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    query.prepare("UPDATE Customer SET full_name = :full_name, phone_number = :phone_number, "
                  "id_card = :id_card, id_room = :id_room, points = :points, membership_tier = :membership_tier "
                  "WHERE id_customer = :id_customer");

    query.bindValue(":id_customer", customer->getId());
    query.bindValue(":full_name", QString::fromStdString(customer->getFullname()));
    query.bindValue(":phone_number", QString::fromStdString(customer->getPhone()));
    query.bindValue(":id_card", QString::fromStdString(customer->getIdcard()));
    query.bindValue(":id_room", QString::fromStdString(customer->getIdRoom()));
    query.bindValue(":points", customer->getPoint());
    query.bindValue(":membership_tier", tierToString(customer->getTier()));

    if (!query.exec())
    {
        qDebug() << "ERROR: Failed to update Customer!" << query.lastError().text();
        return false;
    }
    return true;
}

// Xóa một Customer ra khỏi database dựa theo ID
bool CustomerRepository::remove(int customerId)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    query.prepare("DELETE FROM Customer WHERE id_customer = :id_customer");
    query.bindValue(":id_customer", customerId);

    if (!query.exec())
    {
        qDebug() << "ERROR: Failed to remove Customer!" << query.lastError().text();
        return false;
    }
    return true;
}

// Tái tạo đối tượng Customer từ database theo ID
Customer *CustomerRepository::getById(int customerId)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    query.prepare("SELECT id_customer, full_name, phone_number, id_card, id_room, points, membership_tier "
                  "FROM Customer WHERE id_customer = :id_customer");
    query.bindValue(":id_customer", customerId);

    if (!query.exec() || !query.next())
    {
        qDebug() << "ERROR: Failed to fetch Customer by ID!" << query.lastError().text();
        return nullptr;
    }

    string fullname = query.value("full_name").toString().toStdString();
    string phone = query.value("phone_number").toString().toStdString();
    string idcard = query.value("id_card").toString().toStdString();
    string idroom = query.value("id_room").toString().toStdString();
    int points = query.value("points").toInt();
    MembershipTier tier = stringToTier(query.value("membership_tier").toString());

    // Khởi tạo đối tượng Customer sử dụng constructor tham số và gán thủ công các trường dữ liệu còn lại
    Customer *customer = new Customer(fullname, phone, idcard, tier);
    customer->setId(customerId);
    customer->setIdroom(idroom);
    customer->setPoint(points);

    return customer;
}

// Trả về tất cả các Customer hiện có trong database
vector<Customer *> CustomerRepository::getAll()
{
    vector<Customer *> list;
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    // Lấy trước tất cả các ID của Customer trong database
    if (!query.exec("SELECT id_customer FROM Customer"))
    {
        qDebug() << "ERROR: Failed to fetch all Customer IDs!" << query.lastError().text();
        return list;
    }

    vector<int> ids;
    while (query.next())
    {
        ids.push_back(query.value("id_customer").toInt());
    }

    // Tái tạo từng đối tượng Customer qua hàm getById để đảm bảo tính đồng bộ dữ liệu
    for (int id : ids)
    {
        Customer *c = getById(id);
        if (c)
        {
            list.push_back(c);
        }
    }

    return list;
}