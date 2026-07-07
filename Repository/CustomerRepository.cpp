// File BookingRepos ~ trạm trung chuyển: kết nối giữa database và back-end: đọc dữ liệu Customer từ file Customer lên database
// Liên hệ với file DatabaseManager để kết nối với manager (Singleton)

#include "CustomerRepository.h"
#include "../Manager/DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QString>

// Đọc dữ liệu vào bảng Customer trong hotel.db
bool CustomerRepository::add(Customer customer) {
    // Lấy ra cổng kết nối duy nhất (manager)
    QSqlDatabase db = DatabaseManager::instance().database();

    // Tạo query phụ trách chạy các lệnh ghi data
    QSqlQuery query(db);

    // Tạo các ô trống có dấu : phía trước ở các column
    query.prepare("INSERT INTO Customer (id_customer, full_name, phone_number, id_card, id_room)"
                  "VALUES (:id_customer, :full_name, :phone_number, :id_card, :id_room)");
    // Điền vào các ô trống đã tạo
    query.bindValue(":id_customer", customer.getId());
    query.bindValue(":full_name", QString::fromStdString(customer.getFullname()));
    query.bindValue(":phone_number", QString::fromStdString(customer.getPhone()));
    query.bindValue(":id_card", QString::fromStdString(customer.getIdcard()));
    query.bindValue(":id_room", QString::fromStdString(customer.getIdRoom()));

    // Thực thi ghi data lên database
    if (!query.exec()){
        // Nếu không ghi được data
        qDebug() << "ERROR: Khong the ghi data Customer!" << query.lastError().text();
        return false;
    }

    return true;
}