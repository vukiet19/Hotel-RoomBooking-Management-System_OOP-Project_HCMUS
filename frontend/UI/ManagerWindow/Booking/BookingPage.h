#pragma once

#include <QWidget>

class SplitTablePage;
class QTableWidget;
class QPushButton;

class BookingPage : public QWidget
{
public:
    explicit BookingPage(QWidget *parent = nullptr);

    QTableWidget *bookingTable() const;
    QTableWidget *bookingServicesTable() const;
    QPushButton *bookingTabButton() const;
    QPushButton *servicesTabButton() const;
    void setSection(int section);

private:
    SplitTablePage *sections;
};
