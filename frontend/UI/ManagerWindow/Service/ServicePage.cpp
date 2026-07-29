#include "ServicePage.h"

#include "frontend/UI/ManagerWindow/Common/PageWidgets.h"
#include <QVBoxLayout>

ServicePage::ServicePage(QWidget *parent)
    : QWidget(parent),
      sections(new SplitTablePage(
          "Food Options", {"Food ID", "Category", "Name", "Price"},
          "Service", {"Service ID", "Name", "Category", "Price"}, this))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(sections);
}

QTableWidget *ServicePage::foodTable() const { return sections->firstTable(); }
QTableWidget *ServicePage::serviceTable() const { return sections->secondTable(); }
QPushButton *ServicePage::foodTabButton() const { return sections->firstButton(); }
QPushButton *ServicePage::serviceTabButton() const { return sections->secondButton(); }
void ServicePage::setSection(int section) { sections->setCurrentSection(section); }
