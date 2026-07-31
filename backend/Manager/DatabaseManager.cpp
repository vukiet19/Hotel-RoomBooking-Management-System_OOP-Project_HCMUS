#include "DatabaseManager.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QCoreApplication>
#include <QSqlError>
#include <QDebug>

void DatabaseManager::syncDatabases()
{
    QString projDbPath = QDir::currentPath() + "/backend/Database/hotel.db";
    QString appDbPath = QCoreApplication::applicationDirPath() + "/Database/hotel.db";

    if (QFile::exists(projDbPath) && QFile::exists(appDbPath) && projDbPath != appDbPath)
    {
        QFileInfo projInfo(projDbPath);
        QFileInfo appInfo(appDbPath);

        if (appInfo.lastModified() > projInfo.lastModified())
        {
            QFile::remove(projDbPath);
            if (QFile::copy(appDbPath, projDbPath))
            {
                qDebug() << "[DatabaseManager] Synced newer runtime DB -> source DB";
            }
        }
        else if (projInfo.lastModified() > appInfo.lastModified())
        {
            QFile::remove(appDbPath);
            if (QFile::copy(projDbPath, appDbPath))
            {
                qDebug() << "[DatabaseManager] Synced source DB -> runtime DB";
            }
        }
    }
}

// Constructor chỉ chạy lần đầu tiên khi gọi hàm instance()
DatabaseManager::DatabaseManager()
{
    syncDatabases();

    db = QSqlDatabase::addDatabase("QSQLITE");

    QString projDbPath = QDir::currentPath() + "/backend/Database/hotel.db";
    QString dbPath;

    if (QFile::exists(projDbPath))
    {
        dbPath = projDbPath;
    }
    else
    {
        dbPath = QCoreApplication::applicationDirPath() + "/Database/hotel.db";
    }

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

DatabaseManager::~DatabaseManager()
{
    close();
}

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager manager;
    return manager;
}

bool DatabaseManager::open()
{
    if (db.isOpen())
    {
        return true;
    }

    if (!db.open())
    {
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
    syncDatabases();
}

QSqlDatabase DatabaseManager::database()
{
    if (!db.isOpen())
    {
        db.open();
    }
    return db;
}