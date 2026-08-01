#include "frontend/UI/UI.h"
#include "frontend/UX/UX.h"
#include "frontend/usercheck/backend.h"
#include <QPushButton>
#include <QStackedWidget>

// Section 10: Bills Handler
void MainWindowController::showBillTab() {
  stackedWidget->setCurrentIndex(BillIndex);
  setActionBarVisible(true);
  setActiveButton(buttonBill);
  QString billQuery = R"(
    SELECT 
        bill_id AS "Bill ID",
        booking_id AS "Booking ID",
        total_amount AS "Total Amount"
    FROM Bills
)";

  Backend::loadTableData(tableBill, billQuery);
  btnAdd->setVisible(false);
  btnUpdate->setVisible(false);
  btnDelete->setVisible(false);
  btnFilter->setVisible(true);
  btnAdd->disconnect();
  btnUpdate->disconnect();
  btnDelete->disconnect();
  btnFilter->disconnect();
}
