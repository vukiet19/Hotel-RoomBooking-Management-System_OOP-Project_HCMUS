#pragma once
#include <QString>
#include <string>

class DashboardService
{
public:
	int getTodayBookings();
	double getRevenue(std::string type);
};

