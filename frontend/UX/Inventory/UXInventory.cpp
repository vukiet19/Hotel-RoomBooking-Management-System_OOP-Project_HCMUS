#include "frontend/UX/UX.h"
#include "frontend/UI/UI.h"
#include "frontend/usercheck/backend.h"
#include <QPushButton>
#include <QStackedWidget>

// Section 5: Inventory Handler
void MainWindowController::handleLogin_5()
{
    stackedWidget->setCurrentIndex(InventoryIndex);
    inventoryPage->setSection(0);
    setActionBarVisible(true);
    setActiveButton(buttonInventory);
    Backend::loadTableData(tableInventory, "SELECT * FROM Inventory");
    btnAdd->setVisible(true);
    btnUpdate->setVisible(true);
    btnDelete->setVisible(true);
    btnFilter->setVisible(true);
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
}
