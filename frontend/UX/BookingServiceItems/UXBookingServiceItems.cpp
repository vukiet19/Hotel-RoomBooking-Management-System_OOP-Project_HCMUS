#include "frontend/UX/UX.h"
#include "frontend/UI/UI.h"
#include "frontend/usercheck/backend.h"
#include <QPushButton>
#include <QStackedWidget>

// Section 2: Booking Service Items Handler
void MainWindowController::handleLogin_2()
{
    stackedWidget->setCurrentIndex(BookingIndex);
    bookingPage->setSection(1);
    setActionBarVisible(true);
    setActiveButton(buttonBooking);
    btnAdd->setVisible(false);
    btnUpdate->setVisible(false);
    btnDelete->setVisible(false);
    btnFilter->setVisible(true);
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
    QString bookingItemsQuery = R"(
    SELECT 
        id AS "ID",
        booking_id AS "Booking ID",
        item_id AS "Item ID",
        quantity AS "Qty",
        customer_note AS "Note",
        final_price AS "Price"
    FROM BookingServiceItems
)";

    Backend::loadTableData(tableBookingItems, bookingItemsQuery);
}
