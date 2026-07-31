#include "DatabaseManager.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSqlError>

// Constructor chỉ chạy lần đầu tiên khi gọi hàm instance()
DatabaseManager::DatabaseManager() {
  db = QSqlDatabase::addDatabase("QSQLITE");

  QString exePath = QCoreApplication::applicationDirPath();

  QStringList searchPaths = {
      exePath + "/../../backend/Database/hotel.db",
      exePath + "/../../../backend/Database/hotel.db",
      exePath + "/../backend/Database/hotel.db",
      exePath + "/backend/Database/hotel.db",

      // Các đường dẫn dự phòng khác
      exePath + "/Database/hotel.db", exePath + "/../Database/hotel.db",
      exePath + "/../../Database/hotel.db", exePath + "/hotel.db"};

  QString dbPath = "";
  bool dbFound = false;

  for (const QString &path : searchPaths) {
    if (QFile::exists(path)) {
      dbPath = QDir::cleanPath(path);
      dbFound = true;
      qDebug() << "SUCCESS: DA TIM THAY DATABASE TAI:" << dbPath;
      break;
    }
  }

  if (!dbFound) {
    qDebug() << "ERROR: KHONG TIM THAY hotel.db! File exe dang chay tai:"
             << exePath;
  } else {
    db.setDatabaseName(dbPath);
  }
}

DatabaseManager &DatabaseManager::instance() {
  // Biến tĩnh do chỉ có 1 manager duy nhất
  static DatabaseManager manager;
  return manager; // trả về địa chỉ tới manager gốc
}

bool DatabaseManager::open() {
  if (db.isOpen()) {
    return true;
  }

  if (!db.open()) {
    // .lastError: xem lỗi cuối cùng xảy ra tại db ; .text(): hiển thị lỗi đó
    // dưới dạng văn bản
    qDebug() << "ERROR: Khong the mo file hotel.db" << db.lastError().text();
    return false;
  } else
    return true;
}

DatabaseManager::~DatabaseManager() {
  // Destructor should avoid calling Qt internals during static teardown.
  // Prefer explicit shutdown() to be called while QCoreApplication is still
  // alive.
}

void DatabaseManager::close() {
  // If Qt core is already turn down, avoid calling into QSqlDatabase/driver.
  if (!QCoreApplication::instance())
    return;

  if (db.isOpen()) {
    db.close();
  }

  // Remove the connection properly: release the QSqlDatabase handle first,
  // then call removeDatabase with the connection name.
  QString conn = db.connectionName();
  db = QSqlDatabase();
  if (!conn.isEmpty())
    QSqlDatabase::removeDatabase(conn);
}

QSqlDatabase DatabaseManager::database() {
  if (!db.isOpen()) {
    db.open();
  }
  return db;
}