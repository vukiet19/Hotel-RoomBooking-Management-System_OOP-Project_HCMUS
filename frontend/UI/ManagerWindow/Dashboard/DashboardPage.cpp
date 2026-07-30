#include "DashboardPage.h"
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

namespace
{
QWidget *createCard(const QString &title, QLabel *value, const QString &color)
{
    auto *card = new QWidget();
    card->setStyleSheet(QString("background-color: white; border-radius: 8px; border-left: 5px solid %1;").arg(color));
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(15, 15, 15, 15);

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setStyleSheet("color: #64748b; font-size: 13px; font-weight: bold;");
    value->setStyleSheet("color: #1e293b; font-size: 20px; font-weight: bold; margin-top: 5px;");
    layout->addWidget(titleLabel);
    layout->addWidget(value);
    return card;
}
}

DashboardPage::DashboardPage(QWidget *parent)
    : QWidget(parent), dashboardTable(new QTableWidget(0, 4, this)),
      todayBookings(new QLabel("0", this)), dailyRevenue(new QLabel("0", this)),
      monthlyRevenue(new QLabel("0", this)), yearlyRevenue(new QLabel("0", this))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(20);

    auto *summaryLayout = new QHBoxLayout();
    summaryLayout->setSpacing(15);
    summaryLayout->addWidget(createCard("Today's Bookings", todayBookings, "#3b82f6"));
    summaryLayout->addWidget(createCard("Daily Revenue", dailyRevenue, "#10b981"));
    summaryLayout->addWidget(createCard("Monthly Revenue", monthlyRevenue, "#f59e0b"));
    summaryLayout->addWidget(createCard("Yearly Revenue", yearlyRevenue, "#ef4444"));

    dashboardTable->setHorizontalHeaderLabels({"Booking ID", "Customer Name", "Revenue", "Check-in Date"});
    dashboardTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    dashboardTable->verticalHeader()->setDefaultSectionSize(40);
    dashboardTable->setAlternatingRowColors(true);
    dashboardTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addLayout(summaryLayout);
    layout->addWidget(dashboardTable, 1);
}

QTableWidget *DashboardPage::table() const { return dashboardTable; }
QLabel *DashboardPage::todayBookingsLabel() const { return todayBookings; }
QLabel *DashboardPage::dailyRevenueLabel() const { return dailyRevenue; }
QLabel *DashboardPage::monthlyRevenueLabel() const { return monthlyRevenue; }
QLabel *DashboardPage::yearlyRevenueLabel() const { return yearlyRevenue; }
