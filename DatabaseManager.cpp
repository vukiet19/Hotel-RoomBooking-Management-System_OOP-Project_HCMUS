#include "DatabaseManager.h"

// constructor chay only lan dau tien khi goi instance()
DatabaseManager::DatabaseManager()
{
	db = QSqlDatabase::addDatabase("QSQLITE");
	// Lay duong dan file exe va tro vao file hotel
	QString dbPath = QCoreApplication::applicationDirPath() + "/Database/hotel.db";
	db.setDatabaseName(dbPath);
}

DatabaseManager &DatabaseManager::instance()
{
	// bien tinh do chi co 1 manager duy nhat
	static DatabaseManager manager;
	return manager; // tra ve dia chi toi manager goc
}

bool DatabaseManager::open()
{
	if (!db.open())
	{
		// .lastError: xem loi cuoi cung xay ra tai db ; .text(): hien thi loi do duoi dang van ban
		qDebug() << "ERROR: Khong the mo file hotel.db" << db.lastError().text();
		return false;
	}
	return true;
}

void DatabaseManager::close()
{
	if (db.isOpen())
	{
		db.close();
	}
}

QSqlDatabase DatabaseManager::database()
{
	return db;
}