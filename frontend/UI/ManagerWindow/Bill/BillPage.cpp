#include "BillPage.h"

#include "frontend/UI/ManagerWindow/Common/PageWidgets.h"
#include <QVBoxLayout>

BillPage::BillPage(QWidget *parent)
    : QWidget(parent),
      content(new TablePage({"Bill ID", "Booking ID", "Customer", "Room",
                             "Room Charge", "Service Charge", "Discount",
                             "Deposit", "Total Amount", "Payment Method",
                             "Checkout Time"},
                            this))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(content);
}

QTableWidget *BillPage::table() const { return content->table(); }
