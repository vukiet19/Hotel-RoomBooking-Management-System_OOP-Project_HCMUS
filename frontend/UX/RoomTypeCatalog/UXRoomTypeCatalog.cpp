#include "frontend/UX/UX.h"
#include "frontend/UI/UI.h"
#include "frontend/usercheck/backend.h"
#include <QPushButton>
#include <QStackedWidget>

// Section 8: Room Type Catalog Handler
void MainWindowController::handleLogin_8()
{
    stackedWidget->setCurrentIndex(RoomIndex);
    roomPage->setSection(1);
    setActionBarVisible(true);
    setActiveButton(buttonRoom);
    Backend::loadTableData(tableRoomType, "SELECT * FROM RoomTypeCatalog");
    btnAdd->setVisible(false);
    btnUpdate->setVisible(false);
    btnDelete->setVisible(false);
    btnFilter->setVisible(true);
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
}
