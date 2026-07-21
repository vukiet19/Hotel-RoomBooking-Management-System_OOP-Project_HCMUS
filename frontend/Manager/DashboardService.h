#pragma once
#include <QString>
#include <string>

#include <vector>

struct BookingRevenue {
	int bookingId;
	QString customerName;
	double revenue;
	QString checkIn;
};

class DashboardService
{
public:
	int getTodayBookings();
	double getRevenue(std::string type);
	std::vector<BookingRevenue> getBookingRevenues(const QString &startDate, const QString &endDate);
};


