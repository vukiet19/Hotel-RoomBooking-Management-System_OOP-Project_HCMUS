#include "frontend/UX/UX.h"
#include "frontend/UI/UI.h"
#include "frontend/usercheck/backend.h"
#include <QPushButton>
#include <QStackedWidget>

// Section 5: Inventory Handler
void MainWindowController::showInventoryTab()
{
    stackedWidget->setCurrentIndex(InventoryIndex);
    inventoryPage->setSection(0);
    setActionBarVisible(true);
    setActiveButton(buttonInventory);
    QString inventoryQuery = R"(
    SELECT 
        item_id AS "ID",
        item_name AS "Item Name",
        item_type AS "Type",
        quantity AS "Qty",
        price AS "Price",
        minimum_quantity_required AS "Min Qty"
    FROM Inventory
)";

    Backend::loadTableData(tableInventory, inventoryQuery);
    btnAdd->setVisible(true);
    btnUpdate->setVisible(true);
    btnDelete->setVisible(true);
    btnFilter->setVisible(true);
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
}
