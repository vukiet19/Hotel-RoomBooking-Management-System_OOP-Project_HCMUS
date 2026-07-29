#include "frontend/UX/UX.h"
#include "frontend/UI/UI.h"
#include "frontend/usercheck/backend.h"
#include <QPushButton>
#include <QStackedWidget>

// Section 9: List Service Items Handler
void MainWindowController::handleLogin_9()
{
    stackedWidget->setCurrentIndex(ServiceIndex);
    servicePage->setSection(1);
    setActionBarVisible(true);
    setActiveButton(buttonService);
    Backend::loadTableData(tableService, "SELECT * FROM ListServiceItems");
    btnAdd->setVisible(true);
    btnUpdate->setVisible(true);
    btnDelete->setVisible(true);
    btnFilter->setVisible(true);
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
}
