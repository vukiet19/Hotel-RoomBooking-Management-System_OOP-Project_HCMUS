#include "frontend/UI/UI.h"
#include "frontend/UX/UX.h"
#include "frontend/usercheck/usercheck.h"
#include <QPushButton>
#include <QStackedWidget>

// Section 8: Room Type Catalog Handler
void MainWindowController::showRoomTypeTab() {
  stackedWidget->setCurrentIndex(RoomIndex);
  roomPage->setSection(1);
  setActionBarVisible(true);
  setActiveButton(buttonRoom);
  QString queryStr = R"(
        SELECT 
            room_type AS "Type",
            base_price AS "Base Price"
        FROM RoomTypeCatalog
        ORDER BY base_price ASC
    )";

  Backend::loadTableData(tableRoomType, queryStr);
  btnAdd->setVisible(false);
  btnUpdate->setVisible(false);
  btnDelete->setVisible(false);
  btnFilter->setVisible(true);
  btnAdd->disconnect();
  btnUpdate->disconnect();
  btnDelete->disconnect();
  btnFilter->disconnect();
}
