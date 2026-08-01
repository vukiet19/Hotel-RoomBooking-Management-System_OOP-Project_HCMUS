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

  void handleLogin_1();
  void handleLogin_2();
  void handleLogin_3();
  void handleLogin_4();
  void handleLogin_5();
  void handleLogin_6();
  void handleLogin_7();
  void handleLogin_8();
  void handleLogin_9();
  void handleLogin_10();
  void handleCheckout();

  void showFilterDashboardDialog();

  void showAddBookingDialog();
  void showFilterBookingDialog();

  void showAddRoomDialog();

  void showFilterCustomerDialog();
  void showDeleteCustomerDialog();
  void AddNewCustomerClicked();
  void showUpdateCustomerDialog();

  void showUpdateRoomDialog();
  void showDeleteRoomDialog();

  void AddNewFoodClick();
  void UpdateFoodClick();
  void DeleteFoodClick();
  void FilterFoodClick();

  // Inventory
  void AddInventoryClick();
  void UpdateInventoryClick();
  void DeleteInventoryClick();
  void FilterInventoryClick();

  void FilterInventoryLogClick();
};
