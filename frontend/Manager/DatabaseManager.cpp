#include "DatabaseManager.h"
#include <QDir>
#include <QFile>

// Constructor chỉ chạy lần đầu tiên khi gọi hàm instance()
DatabaseManager::DatabaseManager()
{
    db = QSqlDatabase::addDatabase("QSQLITE");

    qDebug() << "Available drivers:" << QSqlDatabase::drivers();

    QString appPath = QCoreApplication::applicationDirPath();

    QString dbPath = appPath + "/Database/hotel.db";

    qDebug() << dbPath << '\n';

    db.setDatabaseName(dbPath);
}

DatabaseManager &DatabaseManager::instance()
{
    // Biến tĩnh do chỉ có 1 manager duy nhất
    static DatabaseManager manager;
    return manager; // trả về địa chỉ tới manager gốc
}

bool DatabaseManager::open()
{
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
    return db;
}