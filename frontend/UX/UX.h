#pragma once

#include "../UI/UI.h"
#include <QString>

class MainWindowController : public MainWindowUi
{
    Q_OBJECT

public:
    MainWindowController(QWidget *parent = nullptr);
    void handleDashboardTab();

public slots:
    void onRoomStatusObserved(const QString &roomId, int newStatusInt, const QString &timestamp);
    void onBookingStatusObserved(int bookingId, const QString &customerName, const QString &roomNumber, int newStatusInt, double totalPrice, const QString &timestamp);

private:
    void initConnections();
    void showToastNotification(const QString &title, const QString &message);
    void setActiveButton(QPushButton *clickedButton);
    void setActionBarVisible(bool visible);

    void showBookingTab();
    void showBookingServicesTab();
    void showCustomerTab();
    void showFoodTab();
    void showInventoryTab();
    void showInventoryLogTab();
    void showRoomTab();
    void showRoomTypeTab();
    void showServiceTab();
    void showBillTab();
    void handleCheckout();

    void showFilterDashboardDialog();

    void showAddBookingDialog();
    void showFilterBookingDialog();

    void showAddRoomDialog();

    void showFilterCustomerDialog();
    void showDeleteCustomerDialog();
    void showAddCustomerDialog();
    void showUpdateCustomerDialog();

    void showUpdateRoomDialog();
    void showDeleteRoomDialog();

    void showAddFoodDialog();
    void showUpdateFoodDialog();
    void showDeleteFoodDialog();
    void showFilterFoodDialog();
};

