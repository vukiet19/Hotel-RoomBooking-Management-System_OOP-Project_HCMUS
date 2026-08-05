#include "BillPage.h"

#include "frontend/UI/ManagerWindow/Common/PageWidgets.h"
#include <QHeaderView>
#include <QTableWidget>
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

    auto *billTable = content->table();
    auto *header = billTable->horizontalHeader();
    header->setMinimumSectionSize(70);
    header->setStretchLastSection(true);

    const QList<int> columnWidths = {75, 90, 115, 70, 110,
                                     115, 90, 100, 115, 125};
    for (int column = 0; column < columnWidths.size(); ++column)
        billTable->setColumnWidth(column, columnWidths.at(column));
}

QTableWidget *BillPage::table() const { return content->table(); }
