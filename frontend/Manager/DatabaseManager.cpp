#include "DatabaseManager.h"
#include <QDir>
#include <QFile>
#include <QCoreApplication>
#include <QSqlError>
#include <QDebug>

// Constructor chỉ chạy lần đầu tiên khi gọi hàm instance()
DatabaseManager::DatabaseManager()
{
    // Sử dụng trực tiếp biến thành viên 'db' thay vì khai báo biến cục bộ 'QSqlDatabase db'
    db = QSqlDatabase::addDatabase("QSQLITE");

    QString dbPath = QCoreApplication::applicationDirPath() + "/Database/hotel.db";

    db.setDatabaseName(dbPath);

    if (!db.open())
    {
        qDebug() << "ERROR: Khong the mo file hotel.db:" << db.lastError().text();
    }
    else
    {
        qDebug() << "Database connected successfully at:" << dbPath;
    }
}

DatabaseManager &DatabaseManager::instance()
{
    // Biến tĩnh do chỉ có 1 manager duy nhất
    static DatabaseManager manager;
    return manager; // trả về địa chỉ tới manager gốc
}

bool DatabaseManager::open()
{
    if (db.isOpen())
    {
        return true;
    }

    if (!db.open())
    {
        // .lastError: xem lỗi cuối cùng xảy ra tại db ; .text(): hiển thị lỗi đó dưới dạng văn bản
        qDebug() << "ERROR: Khong the mo file hotel.db" << db.lastError().text();
        return false;
    }
    else
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
    if (!db.isOpen())
    {
        db.open();
    }
    return db;
}