#include "ServicePage.h"
#include "frontend/UI/ManagerWindow/Common/PageWidgets.h"
#include <QVBoxLayout>

ServicePage::ServicePage(QWidget *parent)
    : QWidget(parent),
      content(new TablePage({"Service ID", "Name", "Category", "Price", "Status"}, this))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(content);
}

QTableWidget *ServicePage::serviceCatalogTable() const { 
    return content->table(); 
}