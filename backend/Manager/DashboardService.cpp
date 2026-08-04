#include "DashboardService.h"
#include "DatabaseManager.h"
#include <QDateTime>
#include <QDebug>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <string>

// khi xử lý UI cho dashboard có làm việc với pointer nên có interuptions hay
// disconnections sẽ sinh ra undefined behavior nên sẽ sập có lỗi phát sinh nên
// tạo helper function để check coi có connection ko, ko thì reinitialize
// connection để ko sập
static QSqlDatabase getOpenDatabase() {
  QSqlDatabase db = DatabaseManager::instance().database();
  if (!db.isOpen()) {
    qDebug() << "[DEBUG] DB is closed inside DashboardService! Re-opening...";
    DatabaseManager::instance().open();
    db = DatabaseManager::instance().database();
  }
  return db;
}

// XUYÊN SUỐT FILE CÓ GỌI HÀM TRÊN ĐỂ TRÁNH LỖI Ở DASHBOARD =((

int DashboardService::getTodayBookings() {
  QSqlDatabase db = getOpenDatabase();
  QSqlQuery query(db);

  QString todayStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");

  // COUNT(*): đếm xem có tổng cộng bao nhiêu dòng
  // LIKE :today: chỉ đếm xem những dòng có check_in_time là ngày hôm nay
  query.prepare(
      "SELECT COUNT(*) FROM Bookings WHERE check_in_time LIKE :today");
  query.bindValue(":today", todayStr + "%"); // %: giờ phút giây phía sau

  if (query.exec()) {
    if (query.next()) {
      // Lấy ô kết quả đầu tiên (COUNT(*)) và chuyển về int của C++
      return query.value(0).toInt();
    }
  } else
    qDebug() << "ERROR: Khong the thong ke so booking trong ngay!"
             << query.lastError().text();
  return 0;
}

double getDailyRevenue() {
  QSqlDatabase db = getOpenDatabase();
  QSqlQuery query(db);

  QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");
  query.prepare(
      "SELECT SUM(total_amount + deposit_amount) FROM Bills WHERE checkout_time LIKE :currDate");
  query.bindValue(":currDate", currentDate + "%");

  if (query.exec() && query.next() && !query.value(0).isNull()) {
    return query.value(0).toDouble();
  }

  // Fallback to Bookings table for checked-out bookings
  QSqlQuery fallback(db);
  fallback.prepare("SELECT SUM(total_price) FROM Bookings WHERE status = "
                   "'CHECKED_OUT' AND check_out_time LIKE :currDate");
  fallback.bindValue(":currDate", currentDate + "%");
  if (fallback.exec() && fallback.next()) {
    return fallback.value(0).toDouble();
  }

  return 0;
}

double getMonthlyRevenue() {
  QSqlDatabase db = getOpenDatabase();
  QSqlQuery query(db);

  QString currentMonthYear = QDateTime::currentDateTime().toString("yyyy-MM");
  query.prepare("SELECT SUM(total_amount + deposit_amount) FROM Bills WHERE "
                "checkout_time LIKE :currentMonth");
  query.bindValue(":currentMonth", currentMonthYear + "%");

  if (query.exec() && query.next() && !query.value(0).isNull()) {
    return query.value(0).toDouble();
  }

  QSqlQuery fallback(db);
  fallback.prepare("SELECT SUM(total_price) FROM Bookings WHERE status = "
                   "'CHECKED_OUT' AND check_out_time LIKE :currentMonth");
  fallback.bindValue(":currentMonth", currentMonthYear + "%");
  if (fallback.exec() && fallback.next()) {
    return fallback.value(0).toDouble();
  }

  return 0;
}

double getYearlyRevenue() {
  QSqlDatabase db = getOpenDatabase();
  QSqlQuery query(db);

  QString currentYear = QDateTime::currentDateTime().toString("yyyy");
  query.prepare("SELECT SUM(total_amount + deposit_amount) FROM Bills WHERE "
                "checkout_time LIKE :currYear");
  query.bindValue(":currYear", currentYear + "%");

  if (query.exec() && query.next() && !query.value(0).isNull()) {
    return query.value(0).toDouble();
  }

  QSqlQuery fallback(db);
  fallback.prepare("SELECT SUM(total_price) FROM Bookings WHERE status = "
                   "'CHECKED_OUT' AND check_out_time LIKE :currYear");
  fallback.bindValue(":currYear", currentYear + "%");
  if (fallback.exec() && fallback.next()) {
    return fallback.value(0).toDouble();
  }

  return 0;
}

double DashboardService::getRevenue(std::string type) {
  double revenue = 0;
  if (type == "day")
    revenue = getDailyRevenue();
  else if (type == "month")
    revenue = getMonthlyRevenue();
  else
    revenue = getYearlyRevenue();
  return revenue;
}

std::vector<BookingRevenue>
DashboardService::getBookingRevenues(const QString &startDate,
                                     const QString &endDate) {
  std::vector<BookingRevenue> results;
  QSqlDatabase db = getOpenDatabase();
  QSqlQuery query(db);

  QString startBound = startDate;
  QString endBound = endDate;
  if (startBound.length() == 10)
    startBound += " 00:00:00";
  if (endBound.length() == 10)
    endBound += " 23:59:59";

  query.prepare(R"(
		SELECT b.id, COALESCE(c.full_name, 'Guest #' || b.id), (bill.total_amount + bill.deposit_amount), bill.checkout_time
		FROM Bills bill
		JOIN Bookings b ON bill.booking_id = b.id
		LEFT JOIN Customer c ON b.customer_id = c.id
		WHERE bill.checkout_time >= :startDate AND bill.checkout_time <= :endDate
		ORDER BY bill.checkout_time DESC
	)");
  query.bindValue(":startDate", startBound);
  query.bindValue(":endDate", endBound);

  if (query.exec() && query.size() != 0) {
    while (query.next()) {
      BookingRevenue record;
      record.bookingId = query.value(0).toInt();
      record.customerName = query.value(1).toString();
      record.revenue = query.value(2).toDouble();
      record.checkIn = query.value(3).toString();
      results.push_back(record);
    }
  }

  // Fallback to Bookings table if Bills query returned no items
  if (results.empty()) {
    QSqlQuery fallback(db);
    fallback.prepare(R"(
			SELECT b.id, COALESCE(c.full_name, 'Guest #' || b.id), b.total_price, b.check_in_time
			FROM Bookings b
			LEFT JOIN Customer c ON b.customer_id = c.id
			WHERE b.check_in_time >= :startDate AND b.check_in_time <= :endDate
			ORDER BY b.id DESC
		)");
    fallback.bindValue(":startDate", startBound);
    fallback.bindValue(":endDate", endBound);

    if (fallback.exec()) {
      while (fallback.next()) {
        BookingRevenue record;
        record.bookingId = fallback.value(0).toInt();
        record.customerName = fallback.value(1).toString();
        record.revenue = fallback.value(2).toDouble();
        record.checkIn = fallback.value(3).toString();
        results.push_back(record);
      }
    }
  }

  return results;
}