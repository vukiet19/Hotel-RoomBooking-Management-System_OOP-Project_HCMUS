#include "ServicePage.h"

#include "frontend/UI/ManagerWindow/Common/PageWidgets.h"
#include <QVBoxLayout>

ServicePage::ServicePage(QWidget *parent)
    : QWidget(parent),
      sections(new SplitTablePage(
          "Service Catalog", {"Service ID", "Name", "Category", "Price", "VIP Free"},
          "Food Options", {"Option ID", "Parent Service ID", "Name", "Extra Price"}, this))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(sections);
}

QTableWidget *ServicePage::serviceCatalogTable() const { return sections->firstTable(); }
QTableWidget *ServicePage::foodOptionsTable() const { return sections->secondTable(); }
QPushButton *ServicePage::serviceCatalogTabButton() const { return sections->firstButton(); }
QPushButton *ServicePage::foodOptionsTabButton() const { return sections->secondButton(); }
void ServicePage::setSection(int section) { sections->setCurrentSection(section); }
