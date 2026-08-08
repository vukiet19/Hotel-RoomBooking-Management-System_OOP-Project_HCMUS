#include "DatabaseManager.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSqlError>
#include <QtSql/QSqlQuery>

// Constructor chỉ chạy lần đầu tiên khi gọi hàm instance()
DatabaseManager::DatabaseManager() {
  db = QSqlDatabase::addDatabase("QSQLITE");

  QString exePath = QCoreApplication::applicationDirPath();

  QStringList searchPaths = {
      // Relative to executable path (supports macOS bundle & standard build
      // dirs)
      exePath + "/../../../../backend/Database/hotel.db",
      exePath + "/../../../backend/Database/hotel.db",
      exePath + "/../../backend/Database/hotel.db",
      exePath + "/../backend/Database/hotel.db",
      exePath + "/backend/Database/hotel.db",

      exePath + "/../../../../Database/hotel.db",
      exePath + "/../../../Database/hotel.db",
      exePath + "/../../Database/hotel.db", exePath + "/../Database/hotel.db",
      exePath + "/Database/hotel.db", exePath + "/hotel.db",

      // Fallback relative to current working directory
      QDir::currentPath() + "/backend/Database/hotel.db",
      QDir::currentPath() + "/Database/hotel.db",
      QDir::currentPath() + "/hotel.db"};

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
  }

  // ── Migration: đảm bảo các cột mở rộng luôn tồn tại ──────────────────────

  // 1. Thêm linked_inventory_id vào ServiceCatalog nếu chưa có.
  //    Cột này map Damage service với furniture item trong Inventory.
  {
    QSqlQuery colCheck(db);
    bool hasCol = false;
    if (colCheck.exec("PRAGMA table_info(ServiceCatalog)")) {
      while (colCheck.next()) {
        if (colCheck.value("name").toString() == "linked_inventory_id") {
          hasCol = true;
          break;
        }
      }
    }
    if (!hasCol) {
      QSqlQuery alter(db);
      bool ok = alter.exec(
          "ALTER TABLE ServiceCatalog ADD COLUMN linked_inventory_id "
          "INTEGER REFERENCES Inventory(item_id)");
      if (ok)
        qDebug() << "Migration: added linked_inventory_id to ServiceCatalog";
      else
        qDebug() << "Migration ERROR (linked_inventory_id):" << alter.lastError().text();
    }
  }

  // 2. Thêm service_name vào InventoryLog nếu chưa có.
  //    Cột này lưu tên Damage service khi không map được với Inventory item.
  {
    QSqlQuery colCheck(db);
    bool hasCol = false;
    if (colCheck.exec("PRAGMA table_info(InventoryLog)")) {
      while (colCheck.next()) {
        if (colCheck.value("name").toString() == "service_name") {
          hasCol = true;
          break;
        }
      }
    }
    if (!hasCol) {
      QSqlQuery alter(db);
      bool ok = alter.exec(
          "ALTER TABLE InventoryLog ADD COLUMN service_name TEXT");
      if (ok)
        qDebug() << "Migration: added service_name to InventoryLog";
      else
        qDebug() << "Migration ERROR (service_name):" << alter.lastError().text();
    }
  }

  // ─────────────────────────────────────────────────────────────────────────
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