#include "DashboardService.h"
#include "DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDateTime>
#include <QDebug>

int DashboardService::getTodayBookings() {
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	QString todayStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");

	// COUNT(*): dem xem co tong cong bao nhieu dong
	// LIKE :today: chi dem nhung dong chua check_in_time la ngay hom nay
	query.prepare("SELECT COUNT(*) FROM Bookings WHERE check_in_time LIKE :today");
	query.bindValue(":today", todayStr + "%"); // %: gio phut giay phia sau

	if (query.exec()) {
		if (query.next()) {
			// Lay o ket qua dau tien (COUNT(*)) va chuyen ve int cua C++
			return query.value(0).toInt();
		}
	}
	else qDebug() << "ERROR: Khong the thong ke so booking trong ngay!" << query.lastError().text();
	return 0;
}

double DashboardService::getMonthlyRevenue() {
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	// Lay ra thang va nam hien tai theo dinh dang "yyyy-mm"
	QString currentMonthYear = QDateTime::currentDateTime().toString("yyyy-MM");

	query.prepare("SELECT SUM(total_price) FROM Bookings WHERE check_in_time LIKE :currentMonth");
	query.bindValue(":currentMonth", currentMonthYear + "%");

	if (query.exec()) {
		if (query.next()) {
			// Lay gia tri o ket qua dau tien (SUM(total_price)) va chuyen ve double cua C++
			return query.value(0).toDouble();
		}
	}
	else qDebug() << "ERROR: Khong the tinh doanh thu thang!" << query.lastError().text();
	return 0;
}