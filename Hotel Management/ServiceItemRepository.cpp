#include "ServiceItemRepository.h"
#include "DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include "ServiceItem.h"
#include <QtSql/QSqlError>
#include <QDebug>
#include <QString>
#include <QVector>

//lấy toàn bộ danh sách dịch vụ từ database
QVector<ServiceItem> ServiceItemRepository::getAll() {
	//Vector để chứa toàn bộ services
	QVector<ServiceItem> itemList;

	//Kết nối database
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	query.prepare("SELECT item_id, item_name, category, base_price, vip_free_status FROM ServiceCatalog");

	if (query.exec()) {
		while (query.next()) {
			QString id = query.value("item_id").toString();
			QString name = query.value("item_name").toString();
			QString category = query.value("category").toString();
			double price = query.value("base_price").toDouble();
			bool isVipFree = query.value("vip_free_status").toInt() == 1;
			ServiceItem item(id, name, category, price, isVipFree);
			itemList.append(item);
		}
	}
	else {
		qDebug() << "LỖI: Không thể lấy danh sách ServiceItem!" << query.lastError().text();
	}
	return itemList;
}

//lấy danh sach dịch vụ theo itemId
ServiceItem ServiceItemRepository::findById(const QString& itemId) {
	ServiceItem service;
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);
	query.prepare("SELECT item_id, item_name, category, base_price, vip_free_status FROM ServiceCatalog" "WHERE item_id = :id");
	query.bindValue(":id", itemId);

	if (query.exec() && query.next()) {
		QString id = query.value("item_id").toString();
		QString name = query.value("item_name").toString();
		QString category = query.value("category").toString();
		double price = query.value("base_price").toDouble();
		bool isVipFree = query.value("vip_free_status").toInt() == 1;
		service = ServiceItem(id, name, category, price, isVipFree);
	}
	else {
		qDebug() << "Không tìm thấy dịch vụ nào với ID:" << itemId;
	}
	return service;
}

//lấy danh sách dịch vụ theo category
QVector<ServiceItem> ServiceItemRepository::getByCategory(const QString& category) {
	QVector<ServiceItem> itemList;
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);
	query.prepare("SELECT item_id, item_name, category, base_price, vip_free_status FROM ServiceCatalog" "WHERE category = :ctg");
	query.bindValue(":ctg", category);

	if (query.exec() && query.next()) {
		QString id = query.value("item_id").toString();
		QString name = query.value("item_name").toString();
		QString category = query.value("category").toString();
		double price = query.value("base_price").toDouble();
		bool isVipFree = query.value("vip_free_status").toInt() == 1;
		ServiceItem item(id, name, category, price, isVipFree);
		itemList.append(item);
	}
	else {
		qDebug() << "Không tìm thấy dịch vụ nào với category:" << category;
	}
	return itemList;
}