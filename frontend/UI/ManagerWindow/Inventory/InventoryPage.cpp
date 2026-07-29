#include "InventoryPage.h"

#include "frontend/UI/ManagerWindow/Common/PageWidgets.h"
#include <QVBoxLayout>

InventoryPage::InventoryPage(QWidget *parent)
    : QWidget(parent),
      sections(new SplitTablePage(
          "Inventory", {"Item ID", "Name", "Type", "Quantity", "Price"},
          "Inventory Log", {"Log ID", "Item ID", "Quantity", "Action Type", "Date"}, this))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(sections);
}

QTableWidget *InventoryPage::inventoryTable() const { return sections->firstTable(); }
QTableWidget *InventoryPage::inventoryLogTable() const { return sections->secondTable(); }
QPushButton *InventoryPage::inventoryTabButton() const { return sections->firstButton(); }
QPushButton *InventoryPage::inventoryLogTabButton() const { return sections->secondButton(); }
void InventoryPage::setSection(int section) { sections->setCurrentSection(section); }
