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
vector<ServiceCatalogData> ServiceItemRepository::getAllCatalogItems()
{
	vector<ServiceCatalogData> items;

	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	query.prepare(
		"SELECT item_id, item_name, category, base_price, vip_free_status "
		"FROM ServiceCatalog");

	if (!query.exec())
	{
		qDebug() << "ERROR: Khong the doc ServiceCatalog!" << query.lastError().text();
		return items;
	}

	while (query.next())
	{
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
optional<ServiceCatalogData> ServiceItemRepository::findCatalogItemById(const string &itemId)
{
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	query.prepare(
		"SELECT item_id, item_name, category, base_price, vip_free_status "
		"FROM ServiceCatalog "
		"WHERE item_id = :item_id");

	query.bindValue(":item_id", QString::fromStdString(itemId));

	if (!query.exec())
	{
		qDebug() << "ERROR: Khong the tim ServiceCatalog item!" << query.lastError().text();
		return nullopt; // nullopt là kiểu giá trị trả về rỗng của optional
	}

	if (!query.next())
	{
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
hàm này dùng để lọc các dịch vụ có trong menu ServiceCatalog theo nhiều điều kiện khác nhau
Nếu một field trong filter mang giá trị mặc định thì mình bỏ qua điều kiện đó

ví dụ:
- id rỗng thì không lọc theo item_id
- name rỗng thì không lọc theo item_name
- category rỗng thì không lọc theo category
- minBasePrice/maxBasePrice < 0 thì không lọc theo giá
- vipFreeStatus = -1 thì không lọc theo trạng thái miễn phí cho VIP

cách làm giống BookingRepository::getFiltered:
gom các điều kiện cần dùng vào vector conditions, ghép thành câu WHERE bằng AND,
sau đó bindValue từng giá trị để tránh nối trực tiếp input vào SQL
*/
vector<ServiceCatalogData> ServiceItemRepository::getFilteredCatalogItems(const ServiceCatalogFilter &filter)
{
	vector<ServiceCatalogData> items;

	QSqlDatabase db = DatabaseManager::instance().database();

	QString sql =
		"SELECT item_id, item_name, category, base_price, vip_free_status "
		"FROM ServiceCatalog";
	vector<QString> conditions;

	if (!filter.id.empty())
	{
		conditions.push_back("item_id = :item_id");
	}
	if (!filter.name.empty())
	{
		conditions.push_back("item_name LIKE :item_name");
	}
	if (!filter.category.empty())
	{
		conditions.push_back("category = :category");
	}
	if (filter.minBasePrice >= 0.0)
	{
		conditions.push_back("base_price >= :min_base_price");
	}
	if (filter.maxBasePrice >= 0.0)
	{
		conditions.push_back("base_price <= :max_base_price");
	}
	if (filter.vipFreeStatus != -1)
	{
		conditions.push_back("vip_free_status = :vip_free_status");
	}

	if (!conditions.empty())
	{
		sql += QString(" WHERE ") + conditions[0];
		for (size_t i = 1; i < conditions.size(); ++i)
		{
			sql += QString(" AND ") + conditions[i];
		}
	}

	QSqlQuery query(db);
	query.prepare(sql);

	if (!filter.id.empty())
	{
		query.bindValue(":item_id", QString::fromStdString(filter.id));
	}
	if (!filter.name.empty())
	{
		query.bindValue(":item_name", QString("%") + QString::fromStdString(filter.name) + "%");
	}
	if (!filter.category.empty())
	{
		query.bindValue(":category", QString::fromStdString(filter.category));
	}
	if (filter.minBasePrice >= 0.0)
	{
		query.bindValue(":min_base_price", filter.minBasePrice);
	}
	if (filter.maxBasePrice >= 0.0)
	{
		query.bindValue(":max_base_price", filter.maxBasePrice);
	}
	if (filter.vipFreeStatus != -1)
	{
		query.bindValue(":vip_free_status", filter.vipFreeStatus);
	}

	if (!query.exec())
	{
		qDebug() << "ERROR: Khong the loc ServiceCatalog!" << query.lastError().text();
		return items;
	}

	while (query.next())
	{
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
Hàm này là để ghi thông tin dịch vụ được khách hàng chọn vào booking của họ
vào trong database

Lưu ý: đừng có nhầm cái hàm này với cái đề cập thêm object dịch vụ vào
vector<unique_ptr<ServiceItem>> của hàm trên nha, cái này là thêm vào db
*/
int ServiceItemRepository::addBookingServiceItem(const BookingServiceItemData &item)
{
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	query.prepare(
		"INSERT INTO BookingServiceItems "
		"(booking_id, item_id, quantity, customer_note, final_price) "
		"VALUES (:booking_id, :item_id, :quantity, :customer_note, :final_price)");

	query.bindValue(":booking_id", item.bookingId);
	query.bindValue(":item_id", QString::fromStdString(item.itemId));
	query.bindValue(":quantity", item.quantity);
	query.bindValue(":customer_note", QString::fromStdString(item.customerNote));
	query.bindValue(":final_price", item.finalPrice);

	if (!query.exec())
	{
		qDebug() << "ERROR: Khong the them BookingServiceItem!" << query.lastError().text();
		return -1; // trả về -1 nghĩa là insert thất bại
	}

	QVariant newId = query.lastInsertId();
	if (!newId.isValid())
	{
		qDebug() << "ERROR: Khong lay duoc id BookingServiceItem vua tao!";
		return -1;
	}

	return newId.toInt();
}

/*
Hàm này để lấy ra một vector chứa các dịch vụ thuộc một booking cụ thể trong db
Hiểu đơn giản là nó sẽ lấy những cái được ghi vào db ở hàm phía trên á
*/
vector<BookingServiceItemData> ServiceItemRepository::getItemsByBookingId(int bookingId)
{
	vector<BookingServiceItemData> items;

	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	query.prepare(
		"SELECT id, booking_id, item_id, quantity, customer_note, final_price "
		"FROM BookingServiceItems "
		"WHERE booking_id = :booking_id");

	query.bindValue(":booking_id", bookingId);

	if (!query.exec())
	{
		qDebug() << "ERROR: Khong the doc BookingServiceItems!" << query.lastError().text();
		return items;
	}

	while (query.next())
	{
		BookingServiceItemData item;

		item.id = query.value("id").toInt();
		item.bookingId = query.value("booking_id").toInt();
		item.itemId = query.value("item_id").toString().toStdString();
		item.quantity = query.value("quantity").toInt();
		item.customerNote = query.value("customer_note").toString().toStdString();
		item.finalPrice = query.value("final_price").toDouble();

		items.push_back(item);
	}

	return items;
}

/*
hàm này dùng để lọc các dịch vụ đã được thêm vào một booking trong bảng BookingServiceItems
Nó khác ServiceCatalog ở chỗ bảng này lưu dịch vụ phát sinh thực tế của khách,
nên filter sẽ dựa trên booking_id, item_id, quantity, final_price và customer_note

nếu một field trong BookingServiceItemFilter mang giá trị mặc định thì mình bỏ qua điều kiện đó
Ví dụ bookingId = -1 thì không lọc theo booking_id, itemId rỗng thì không lọc theo item_id,
minQuantity/maxQuantity = -1 thì không lọc theo số lượng,
minFinalPrice/maxFinalPrice < 0 thì không lọc theo tiền cuối cùng

cách làm giống filter của BookingRepository:
tạo câu SELECT gốc, thêm các điều kiện cần lọc vào vector conditions,
ghép WHERE bằng AND, rồi bindValue các tham số trước khi query.exec()
*/
vector<BookingServiceItemData> ServiceItemRepository::getFilteredBookingServiceItems(const BookingServiceItemFilter &filter)
{
	vector<BookingServiceItemData> items;

	QSqlDatabase db = DatabaseManager::instance().database();

	QString sql =
		"SELECT id, booking_id, item_id, quantity, customer_note, final_price "
		"FROM BookingServiceItems";
	vector<QString> conditions;

	if (filter.id != -1)
	{
		conditions.push_back("id = :id");
	}
	if (filter.bookingId != -1)
	{
		conditions.push_back("booking_id = :booking_id");
	}
	if (!filter.itemId.empty())
	{
		conditions.push_back("item_id = :item_id");
	}
	if (filter.minQuantity != -1)
	{
		conditions.push_back("quantity >= :min_quantity");
	}
	if (filter.maxQuantity != -1)
	{
		conditions.push_back("quantity <= :max_quantity");
	}
	if (filter.minFinalPrice >= 0.0)
	{
		conditions.push_back("final_price >= :min_final_price");
	}
	if (filter.maxFinalPrice >= 0.0)
	{
		conditions.push_back("final_price <= :max_final_price");
	}
	if (!filter.customerNote.empty())
	{
		conditions.push_back("customer_note LIKE :customer_note");
	}

	if (!conditions.empty())
	{
		sql += QString(" WHERE ") + conditions[0];
		for (size_t i = 1; i < conditions.size(); ++i)
		{
			sql += QString(" AND ") + conditions[i];
		}
	}

	QSqlQuery query(db);
	query.prepare(sql);

	if (filter.id != -1)
	{
		query.bindValue(":id", filter.id);
	}
	if (filter.bookingId != -1)
	{
		query.bindValue(":booking_id", filter.bookingId);
	}
	if (!filter.itemId.empty())
	{
		query.bindValue(":item_id", QString::fromStdString(filter.itemId));
	}
	if (filter.minQuantity != -1)
	{
		query.bindValue(":min_quantity", filter.minQuantity);
	}
	if (filter.maxQuantity != -1)
	{
		query.bindValue(":max_quantity", filter.maxQuantity);
	}
	if (filter.minFinalPrice >= 0.0)
	{
		query.bindValue(":min_final_price", filter.minFinalPrice);
	}
	if (filter.maxFinalPrice >= 0.0)
	{
		query.bindValue(":max_final_price", filter.maxFinalPrice);
	}
	if (!filter.customerNote.empty())
	{
		query.bindValue(":customer_note", QString("%") + QString::fromStdString(filter.customerNote) + "%");
	}

	if (!query.exec())
	{
		qDebug() << "ERROR: Khong the loc BookingServiceItems!" << query.lastError().text();
		return items;
	}

	while (query.next())
	{
		BookingServiceItemData item;

		item.id = query.value("id").toInt();
		item.bookingId = query.value("booking_id").toInt();
		item.itemId = query.value("item_id").toString().toStdString();
		item.quantity = query.value("quantity").toInt();
		item.customerNote = query.value("customer_note").toString().toStdString();
		item.finalPrice = query.value("final_price").toDouble();

		items.push_back(item);
	}

	return items;
}

// xóa dòng của một dịch vụ dựa trên id của dịch vụ đó
bool ServiceItemRepository::removeBookingServiceItem(int id)
{
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	query.prepare(
		"DELETE FROM BookingServiceItems "
		"WHERE id = :id");

	query.bindValue(":id", id);

	if (!query.exec())
	{
		qDebug() << "ERROR: Khong the xoa BookingServiceItem!" << query.lastError().text();
		return false;
	}

	return query.numRowsAffected() > 0;
}

// xóa toàn bộ dịch vụ có trong bookingId được truyền vào
bool ServiceItemRepository::removeItemsByBookingId(int bookingId)
{
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	query.prepare(
		"DELETE FROM BookingServiceItems "
		"WHERE booking_id = :booking_id");

	query.bindValue(":booking_id", bookingId);

	if (!query.exec())
	{
		qDebug() << "ERROR: Khong the xoa BookingServiceItems theo booking_id!" << query.lastError().text();
		return false;
	}
	// trường hợp một booking chưa dùng dịch vụ gì hết thì vẫn trả về true
	return true;
}
