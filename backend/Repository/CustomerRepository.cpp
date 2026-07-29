// File BookingRepos ~ trạm trung chuyển: kết nối giữa database và back-end: đọc dữ liệu Customer từ file Customer lên database
// Liên hệ với file DatabaseManager để kết nối với manager (Singleton)

#include "CustomerRepository.h"
#include "../Manager/DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QString>

// Đọc dữ liệu vào bảng Customer trong hotel.db
bool CustomerRepository::add(Customer customer)
{
    // Lấy ra cổng kết nối duy nhất (manager)
    QSqlDatabase db = DatabaseManager::instance().database();

    QSqlQuery query(db);

    // Tạo các ô trống có dấu : phía trước ở các column
    query.prepare("INSERT INTO Customer (id_customer, full_name, phone_number,Type, id_room)"
                  "VALUES (:id_customer, :full_name, :phone_number,:Type, :id_room)");
    // Điền vào các ô trống đã tạo
    query.bindValue(":id_customer", QString::fromStdString(customer.getIdcard()));
    query.bindValue(":full_name", QString::fromStdString(customer.getFullname()));
    query.bindValue(":phone_number", QString::fromStdString(customer.getPhone()));
    query.bindValue(":Type", customer.getTier());
    query.bindValue(":id_room", QString::fromStdString(customer.getIdRoom()));

    // Thực thi ghi data lên database
    if (!query.exec())
    {
        // Nếu không ghi được data
        qDebug() << "ERROR: Khong the ghi data Customer!" << query.lastError().text();
        return false;
    }

    return true;
}

bool CustomerRepository::update(Customer customer)
{
    // Lấy ra cổng kết nối duy nhất
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    // Dùng UPDATE ... SET ... WHERE để cập nhật thông tin
    query.prepare("UPDATE Customer SET full_name = :full_name, phone_number = :phone_number, "
                  "Type = :Type, id_room = :id_room WHERE id_customer = :id_customer");

    // Điền vào các ô trống đã tạo
    query.bindValue(":id_customer", QString::fromStdString(customer.getIdcard()));
    query.bindValue(":full_name", QString::fromStdString(customer.getFullname()));
    query.bindValue(":phone_number", QString::fromStdString(customer.getPhone()));
    query.bindValue(":Type", customer.getTier());
    query.bindValue(":id_room", QString::fromStdString(customer.getIdRoom()));

    // Thực thi ghi data lên database
    if (!query.exec())
    {
        qDebug() << "ERROR: Khong the cap nhat data Customer!" << query.lastError().text();
        return false;
    }

    return true;
}

// remove theo id
bool CustomerRepository::remove(std::string id_customer)
{
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    // Xoá dòng có id_customer tương ứng
    query.prepare("DELETE FROM Customer WHERE id_customer = :id_customer");
    query.bindValue(":id_customer", QString::fromStdString(id_customer));

    // Kiểm tra có thực thi được không
    if (!query.exec())
    {
        qDebug() << "ERROR: Khong the xoa Customer!" << query.lastError().text();
        return false;
    }

    return true;
}

// Dùng để filter
std::vector<Customer> CustomerRepository::filter(QString columnName, QString searchValue)
{
    std::vector<Customer> resultList;
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    QString sqlString = QString("SELECT * FROM Customer WHERE %1 LIKE :value").arg(columnName);

    query.prepare(sqlString);

    query.bindValue(":value", "%" + searchValue + "%");

    if (query.exec())
    {
        while (query.next())
        {
            // Lấy dữ liệu từ database
            std::string id = query.value("id_customer").toString().toStdString();
            std::string name = query.value("full_name").toString().toStdString();
            std::string phone = query.value("phone_number").toString().toStdString();
            int tier = query.value("Type").toInt();
            std::string roomId = query.value("id_room").toString().toStdString();

            // Khởi tạo đối tươgj tạm
            Customer tmp(name, phone, id, static_cast<MembershipTier>(tier));
            tmp.setIdroom(roomId);

            // Them vao danh sach
            resultList.push_back(tmp);
        }
    }
    else
    {
        qDebug() << "ERROR: Khong the filter Customer!" << query.lastError().text();
    }

    return resultList;
}