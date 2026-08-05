#include "BookingPage.h"

#include "frontend/UI/ManagerWindow/Common/PageWidgets.h"
#include <QVBoxLayout>

BookingPage::BookingPage(QWidget *parent)
    : QWidget(parent),
      sections(new SplitTablePage(
          "Booking", {"Booking ID", "Customer ID", "Room Number", "Check-in", "Check-out", "Status", "Deposit Amount", "Deposit Status", "Total Price"},
          "Booking Services", {"ID", "Booking ID", "Item ID", "Quantity", "Customer Note", "Final Price"}, this))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(sections);
}

QTableWidget *BookingPage::bookingTable() const { return sections->firstTable(); }
QTableWidget *BookingPage::bookingServicesTable() const { return sections->secondTable(); }
QPushButton *BookingPage::bookingTabButton() const { return sections->firstButton(); }
QPushButton *BookingPage::servicesTabButton() const { return sections->secondButton(); }
void BookingPage::setSection(int section) { sections->setCurrentSection(section); }
