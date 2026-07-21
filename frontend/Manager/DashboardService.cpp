#include "DashboardService.h"
#include "DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDateTime>
#include <QDebug>
#include <string>

static QSqlDatabase getOpenDatabase() {
	QSqlDatabase db = DatabaseManager::instance().database();
	if (!db.isOpen()) {
		qDebug() << "[DEBUG] DB is closed inside DashboardService! Re-opening...";
		DatabaseManager::instance().open();
		db = DatabaseManager::instance().database();
	}
	return db;
}

int DashboardService::getTodayBookings() {
	QSqlDatabase db = getOpenDatabase();
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
	QSqlDatabase db = getOpenDatabase();
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
	QSqlDatabase db = getOpenDatabase();
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
	QSqlDatabase db = getOpenDatabase();
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

//bởi vì bookingRevenue không thể hiện một object một trường riêng mà chỉ thể hiện các góc cạnh của các class khác thôi
//nên sẽ không trả về ptr và populate lên UI cx dễ nếu như có sử dụng thông số trực tiếp trên này sẽ thay đổi sau
std::vector<BookingRevenue> DashboardService::getBookingRevenues(const QString &startDate, const QString &endDate) {
	std::vector<BookingRevenue> results;
	QSqlDatabase db = getOpenDatabase();
	QSqlQuery query(db);

	QString startBound = startDate;
	QString endBound = endDate;
	//startBound = 10 nghĩa là đúng kiểu lọc YYYY-MM-DD
	//QDateTime::toString trả về kiểu YYYY-MM-DDTHH::MM::SS nên nếu so với kiểu lọc sẽ luôn lớn hơn nên += T... đẻ so cho đúng
	if (startBound.length() == 10) startBound += "T00:00:00";
	if (endBound.length() == 10) endBound += "T23:59:59";

	query.prepare("SELECT b.id, c.full_name, b.total_price, b.check_in_time "
				  "FROM Bookings b "
				  "LEFT JOIN Customer c ON b.customer_id = c.id_customer "
				  "WHERE b.check_in_time >= :startDate AND b.check_in_time <= :endDate");
	query.bindValue(":startDate", startBound);
	query.bindValue(":endDate", endBound);

	if (query.exec()) {
		while (query.next()) {
			BookingRevenue record;
			record.bookingId = query.value(0).toInt();
			record.customerName = query.value(1).toString();
			record.revenue = query.value(2).toDouble();
			record.checkIn = query.value(3).toString();
			results.push_back(record);
		}
	}
	else qDebug() << "ERROR: Failed to query booking revenues!" << query.lastError().text();
	return results;
}