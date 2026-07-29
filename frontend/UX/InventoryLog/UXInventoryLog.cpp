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
    Backend::loadTableData(tableInventoryLog, "SELECT * FROM InventoryLog");
    btnAdd->setVisible(false);
    btnUpdate->setVisible(false);
    btnDelete->setVisible(false);
    btnFilter->setVisible(true);
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
}
