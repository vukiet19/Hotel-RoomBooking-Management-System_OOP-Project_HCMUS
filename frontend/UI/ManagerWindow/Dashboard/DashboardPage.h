#pragma once

#include <QWidget>

class QTableWidget;
class QLabel;

class DashboardPage : public QWidget
{
public:
    explicit DashboardPage(QWidget *parent = nullptr);

    QTableWidget *table() const;
    QLabel *todayBookingsLabel() const;
    QLabel *dailyRevenueLabel() const;
    QLabel *monthlyRevenueLabel() const;
    QLabel *yearlyRevenueLabel() const;

private:
    QTableWidget *dashboardTable;
    QLabel *todayBookings;
    QLabel *dailyRevenue;
    QLabel *monthlyRevenue;
    QLabel *yearlyRevenue;
};
