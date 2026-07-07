#include "DatabaseManager.h"
#include <QDir>
#include <QFile>

// constructor chay only lan dau tien khi goi instance()

// constructor chay only lan dau tien khi goi instance()
DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");

    // 1. Lấy vị trí file .exe hiện tại đang chạy
    QString exePath = QCoreApplication::applicationDirPath();

    // 2. Lập danh sách các ổ trú ẩn có thể chứa thư mục "Database/hotel.db" hoặc "hotel.db"
    // Bắt đầu từ vị trí hiện tại (exePath) và lùi dần ra ngoài (../)
    QStringList searchPaths = {
        exePath + "/Database/hotel.db",            // Nếu build cùng chỗ
        exePath + "/../Database/hotel.db",         // Lùi 1 cấp
        exePath + "/../../Database/hotel.db",      // Lùi 2 cấp (Cấu trúc hiện tại của bạn)
        exePath + "/../../../Database/hotel.db",   // Lùi 3 cấp (VS Studio/CMake)
        exePath + "/../../../../Database/hotel.db",// Lùi 4 cấp
        exePath + "/hotel.db",                     // Dành cho lúc nén file Release đem nộp
        exePath + "/../hotel.db"
    };

    QString dbPath = "";
    bool dbFound = false;

    // 3. Cho C++ đi tuần tự kiểm tra từng đường dẫn
    for (const QString& path : searchPaths) {
        if (QFile::exists(path)) {
            // Chuẩn hóa đường dẫn cho đẹp (đổi // thành /) trước khi lưu
            dbPath = QDir::cleanPath(path);
            dbFound = true;
            qDebug() << "DA TIM THAY DATABASE TAI:" << dbPath;
            break; // Tìm thấy thì thoát vòng lặp ngay
        }
    }

    // 4. Báo lỗi và dùng đường dẫn dự phòng nếu lỡ mọi thứ thất bại
    if (!dbFound) {
        qDebug() << "!!! LỖI: KHÔNG TÌM THẤY hotel.db TRONG PROJECT !";
        // Fallback: Vẫn gán lại đường dẫn cũ của bạn để thử vận may
        dbPath = QDir::cleanPath(exePath + "/../../Database/hotel.db");
    }

    // Nạp đường dẫn linh hoạt vừa tìm được vào DB
    db.setDatabaseName(dbPath);
}

DatabaseManager& DatabaseManager::instance() {
	// bien tinh do chi co 1 manager duy nhat
	static DatabaseManager manager;
    if (!manager.db.isOpen()) {
        manager.open();
    }
	return manager; // tra ve dia chi toi manager goc
}

bool DatabaseManager::open() {
	if (!db.open()) {
		// .lastError: xem loi cuoi cung xay ra tai db ; .text(): hien thi loi do duoi dang van ban
		qDebug() << "ERROR: Khong the mo file hotel.db" << db.lastError().text();
		return false;
	}
	return true;
}

DatabaseManager::~DatabaseManager() {
    // Destructor should avoid calling Qt internals during static teardown.
    // Prefer explicit shutdown() to be called while QCoreApplication is still alive.
}

void DatabaseManager::close() {
    // If Qt core is already torn down, avoid calling into QSqlDatabase/driver.
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
	return db;
}