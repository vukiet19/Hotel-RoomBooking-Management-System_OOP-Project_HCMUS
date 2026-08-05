#pragma once

#include "../UI/UI.h"
#include <QString>

class MainWindowController : public MainWindowUi {
  Q_OBJECT

public:
  MainWindowController(QWidget *parent = nullptr);
  void handleDashboardTab();

public slots:
  void onRoomStatusObserved(const QString &roomId, int newStatusInt,
                            const QString &timestamp);
  void onBookingStatusObserved(int bookingId, const QString &customerName,
                               const QString &roomNumber, int newStatusInt,
                               double totalPrice, const QString &timestamp);

private:
  void initConnections();
  void showToastNotification(const QString &title, const QString &message);
  void setActiveButton(QPushButton *clickedButton);
  void setActionBarVisible(bool visible);

  void showBookingTab();
  void showBookingServicesTab();
  void showFilterBookingServicesDialog();
  void showCustomerTab();
  void showRoomTab();
  void showRoomTypeTab();
  void showBillTab();
  void showFilterBillDialog();
  void handleCheckout();
  void handleLogout();

  void showFilterDashboardDialog();

  void showAddBookingDialog();
  void showFilterBookingDialog();
  void showUpdateBookingDialog();
  void showDeleteBookingDialog();

  void showAddRoomDialog();

  void showFilterCustomerDialog();
  void showDeleteCustomerDialog();
  void showAddCustomerDialog();
  void showUpdateCustomerDialog();

  void showUpdateRoomDialog();
  void showDeleteRoomDialog();

  // Service
  void showServiceTab();
  void showAddServiceDialog();
  void showUpdateServiceDialog();
  void showDeleteServiceDialog();
  void showFilterServiceDialog();
  void showAddServiceToBookingDialog();

  // Inventory
  void showInventoryTab();
  void AddInventoryClick();
  void UpdateInventoryClick();
  void DeleteInventoryClick();
  void FilterInventoryClick();
  void AddToBookingInventoryClick();

  void showInventoryLogTab();
  void FilterInventoryLogClick();
};
