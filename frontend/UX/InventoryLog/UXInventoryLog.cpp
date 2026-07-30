#include "frontend/UX/UX.h"
#include "frontend/UI/UI.h"
#include "frontend/usercheck/backend.h"
#include <QPushButton>
#include <QStackedWidget>

// Section 6: Inventory Log Handler
void MainWindowController::handleLogin_6()
{
    stackedWidget->setCurrentIndex(InventoryIndex);
    inventoryPage->setSection(1);
    setActionBarVisible(true);
    setActiveButton(buttonInventory);
    QString inventoryLogQuery = R"(
    SELECT 
        log_id AS "Log ID",
        item_id AS "Item ID",
        quantity AS "Qty",
        action_type AS "Type",
        date AS "Date"
    FROM InventoryLog
)";

    Backend::loadTableData(tableInventoryLog, inventoryLogQuery);
    btnAdd->setVisible(false);
    btnUpdate->setVisible(false);
    btnDelete->setVisible(false);
    btnFilter->setVisible(true);
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
}
