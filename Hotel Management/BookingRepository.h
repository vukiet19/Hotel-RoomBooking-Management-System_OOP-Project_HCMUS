#pragma once
#include "DatabaseManager.h"
#include <QtSql/QSqlQuery>
#include <QString>

// Truyen du lieu ve thong tin dat phong vao file hotel.db
struct BookingData {
	int customerId;
	QString roomNumber;
	QString checkInTime;
	QString checkOutTime;
	double totalPrice;
};

class BookingRepository {
public:
	// const BookingData&: lay truc tiep du lieu bien booking trong bo nho
	bool add(const BookingData& booking);
};

