#include "CustomerPage.h"

#include "frontend/UI/ManagerWindow/Common/PageWidgets.h"
#include <QVBoxLayout>

CustomerPage::CustomerPage(QWidget *parent)
    : QWidget(parent),
      content(new TablePage({"Customer ID", "ID Number", "Name", "Phone Number", "Type", "Point"}, this))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(content);
}

QTableWidget *CustomerPage::table() const { return content->table(); }
