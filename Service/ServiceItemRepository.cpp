#include "ServiceItemRepository.h"
#include "../Manager/DatabaseManager.h"

#include <QDebug>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QVariant>
#include <QString>
/*
Nhiệm vụ của file repositoty này là kết nối code OOP với database.
Đọc/ghi dữ liệu service item trong database

File này làm việc hoàn toàn trên database, không đụng tới code OOP nha
*/
using namespace std;

/* 
hàm này trả một vector chứa thông tin của toàn bộ dịch vụ trong khách sạn
Phục vụ cho việc làm UI, để hiển thị bảng menu dịch vụ
*/
vector<ServiceCatalogData> ServiceItemRepository::getAllCatalogItems() {
    vector<ServiceCatalogData> items;

    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    query.prepare(
        "SELECT item_id, item_name, category, base_price, vip_free_status "
        "FROM ServiceCalalog"
    );

    while(query.next()) {
        ServiceCatalogData item;
        item.id = query.value("item_id").toString().toStdString();
        item.name = query.value("item_name").toString().toStdString();
        item.category = query.value("category").toString().toStdString();
        item.basePrice = query.value("base_price").toDouble();
        item.vipFreeStatus = query.value("vip_free_status").toBool();

        items.push_back(item);
    }

    return items;
}

/*
khi khách hàng chọn một dịch vụ trong menu ở màn hình UI, mình sẽ lấy cái
item_id đó để mang đi tìm trong database dịch vụ đó.
Nếu không tìm thấy thì trả về kiểu nullopt

Việc tìm thông tin của dịch vụ trong db có 2 ý nghĩa cho sau này:
- Một là để tạo object tương ứng để dùng các hàm nghiệp vụ riêng của nó
- Hai sau khi tạo object của dịch vụ thì ta thêm nó vào 
  vector<unique_ptr<ServiceItem>> của booking chứa dịch vụ đó
*/
optional<ServiceCatalogData> ServiceItemRepository::findCatalogItemById(const string &itemId) {
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    query.prepare(
        "SELECT item_id, item_name, category, base_price, vip_free_status "
        "FROM ServiceCalalog "
        "WHERE item_id = :item_id"
    );

    query.bindValue(":item_id", QString::fromStdString(itemId));

    if(!query.exec()) {
        qDebug() << "ERROR: Khong the tim ServiceCatalog item!" << query.lastError().text();
        return nullopt; // nullopt là kiểu giá trị trả về rỗng của optional
    }

    if(!query.next()) {
        return nullopt;
    }

    ServiceCatalogData item;
    item.id = query.value("item_id").toString().toStdString();
    item.name = query.value("item_name").toString().toStdString();
    item.category = query.value("category").toString().toStdString();
    item.basePrice = query.value("base_price").toDouble();
    item.vipFreeStatus = query.value("vip_free_status").toBool();

    return item;
}


/*
Hàm này là để ghi thông tin dịch vụ được khách hàng chọn vào booking của họ
vào trong database

Lưu ý: đừng có nhầm cái hàm này với cái đề cập thêm object dịch vụ vào
vector<unique_ptr<ServiceItem>> của hàm trên nha, cái này là thêm vào db
*/
int ServiceItemRepository::addBookingServiceItem(const BookingServiceItemData& item) {
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    query.prepare(
        "INSERT INTO BookingServiceItems "
        "(booking_id, item_id, quantity, customer_note, final_price) "
        "VALUES (:booking_id, :item_id, :quantity, :customer_note, :final_price)"
    );

    query.bindValue(":booking_id", item.bookingId);
    query.bindValue(":item_id", QString::fromStdString(item.itemId));
    query.bindValue(":quantity", item.quantity);
    query.bindValue(":customer_note", QString::fromStdString(item.customerNote));
    query.bindValue(":final_price", item.finalPrice);

    if (!query.exec()) {
        qDebug() << "ERROR: Khong the them BookingServiceItem!" << query.lastError().text();
        return -1; // trả về -1 nghĩa là insert thất bại
    }

    QVariant newId = query.lastInsertId();
    if (!newId.isValid()) {
        qDebug() << "ERROR: Khong lay duoc id BookingServiceItem vua tao!";
        return -1;
    }

    return newId.toInt();
}


/*
Hàm này để lấy ra một vector chứa các dịch vụ thuộc một booking cụ thể trong db
Hiểu đơn giản là nó sẽ lấy những cái được ghi vào db ở hàm phía trên á
*/
vector<BookingServiceItemData> ServiceItemRepository::getItemsByBookingId(int bookingId) {
    vector<BookingServiceItemData> items;

    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    query.prepare(
        "SELECT booking_id, item_id, quantity, customer_note, final_price "
        "FROM BookingServiceItems "
        "WHERE booking_id = :booking_id"
    );

    query.bindValue(":booking_id", bookingId);

    if (!query.exec()) {
        qDebug() << "ERROR: Khong the doc BookingServiceItems!" << query.lastError().text();
        return items;
    }

    while (query.next()) {
        BookingServiceItemData item;
        item.bookingId = query.value("booking_id").toInt();
        item.itemId = query.value("item_id").toString().toStdString();
        item.quantity = query.value("quantity").toInt();
        item.customerNote = query.value("customer_note").toString().toStdString();
        item.finalPrice = query.value("final_price").toDouble();

        items.push_back(item);
    }

    return items;
}