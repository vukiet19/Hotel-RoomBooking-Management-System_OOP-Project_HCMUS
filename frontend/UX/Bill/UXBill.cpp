#include "frontend/UI/UI.h"
#include "frontend/UX/UX.h"
#include "frontend/usercheck/backend.h"
#include <QPushButton>
#include <QStackedWidget>

// Section 10: Bills Handler
void MainWindowController::handleLogin_10() {
  stackedWidget->setCurrentIndex(BillIndex);
  setActionBarVisible(true);
  setActiveButton(buttonBill);
  Backend::loadTableData(tableBill, "SELECT * FROM Bills");
  btnAdd->setVisible(false);
  btnUpdate->setVisible(false);
  btnDelete->setVisible(false);
  btnFilter->setVisible(true);
  btnAdd->disconnect();
  btnUpdate->disconnect();
  btnDelete->disconnect();
  btnFilter->disconnect();
}
