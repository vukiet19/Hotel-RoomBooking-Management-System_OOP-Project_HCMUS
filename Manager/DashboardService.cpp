#include "DashboardService.h"
#include "DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDateTime>
#include <QDebug>
#include <string>

int DashboardService::getTodayBookings() {
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	QString todayStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");

	// COUNT(*): đếm xem có tổng cộng bao nhiêu dòng
	// LIKE :today: chỉ đếm xem những dòng có check_in_time là ngày hôm nay
	query.prepare("SELECT COUNT(*) FROM Bookings WHERE check_in_time LIKE :today");
	query.bindValue(":today", todayStr + "%"); // %: giờ phút giây phía sau

	if (query.exec()) {
		if (query.next()) {
			// Lấy ô kết quả đầu tiên (COUNT(*)) và chuyển về int của C++
			return query.value(0).toInt();
		}
	}
	else qDebug() << "ERROR: Khong the thong ke so booking trong ngay!" << query.lastError().text();
	return 0;
}

double getDailyRevenue() {
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	// Lấy ra ngày, tháng, năm hiện tại theo định dạng 
	QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");
	query.prepare("SELECT SUM(total_price) FROM Bookings WHERE check_in_time LIKE :currDate");
	query.bindValue(":currDate", currentDate + "%");

	if (query.exec()) {
		if (query.next()){
			return query.value(0).toDouble();
		}
	}
	else qDebug() << "ERROR: Khong the tinh doanh thu ngay!" << query.lastError().text();
	return 0;
}

double getMonthlyRevenue() {
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	// Lấy ra tháng và năm hiện tại theo định dạng "yyyy-mm"
	QString currentMonthYear = QDateTime::currentDateTime().toString("yyyy-MM");

	query.prepare("SELECT SUM(total_price) FROM Bookings WHERE check_in_time LIKE :currentMonth");
	query.bindValue(":currentMonth", currentMonthYear + "%");

	if (query.exec()) {
		if (query.next()) {
			// Lấy giá trị ô kết quả đầu tiên (SUM(total_price)) và chuyển về double của C++
			return query.value(0).toDouble();
		}
	}
	else qDebug() << "ERROR: Khong the tinh doanh thu thang!" << query.lastError().text();
	return 0;
}

double getYearlyRevenue(){
	QSqlDatabase db = DatabaseManager::instance().database();
	QSqlQuery query(db);

	// Lấy ra năm hiện tại
	QString currentYear = QDateTime::currentDateTime().toString("yyyy");
	
	query.prepare("SELECT SUM(total_price) FROM Bookings WHERE check_in_time LIKE :currYear");
	query.bindValue(":currYear", currentYear + "%");

	if (query.exec()) {
		if (query.next()) {
			// Lấy giá trị ô kết quả đầu tiên - SUM(total_price) và chuyển về double của C++
			return query.value(0).toDouble();
		}
	}
	else qDebug() << "ERROR: Khong the tinh doanh thu nam!" << query.lastError().text();
	return 0;
}

double DashboardService::getRevenue(std::string type){
	double revenue = 0;
	if (type == "day") revenue = getDailyRevenue();
	else if (type == "month") revenue = getMonthlyRevenue();
	else revenue = getYearlyRevenue();
	return revenue;
}