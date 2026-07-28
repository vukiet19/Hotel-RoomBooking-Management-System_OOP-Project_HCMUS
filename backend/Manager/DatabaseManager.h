#pragma once
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QCoreApplication>
#include <QDebug>

class DatabaseManager {
private:
	QSqlDatabase db;
	// constructor trong private: dam bao chi co 1 trinh quan li (Singleton)
	DatabaseManager();

public:
	// Copy Constructor: dung de tranh mot manager moi duoc tao ra nho cu phap = DatabaseManager::instance()
	// Example: DatabaseManager db2 = DatabaseManager::instance(); --> delete
	DatabaseManager(const DatabaseManager&) = delete;
	// Copy Assignment Operator: khong cho phep gan manager goc cho bat ki manager nao khac
	// Example: db2 = db1 --> delete
	DatabaseManager& operator=(const DatabaseManager&) = delete;

	// reference &: tra ve bien goc trong bo nho (khong copy du lieu)
	static DatabaseManager& instance();
	
	bool open();
	void close();

	// getter db
	QSqlDatabase database();
};

