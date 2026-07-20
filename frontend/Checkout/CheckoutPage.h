#pragma once

#include <QVector>
#include <QWidget>

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;

struct CheckoutServicePreview
{
    QString name;
    int quantity = 0;
    double unitPrice = 0.0;
};

struct CheckoutBookingPreview
{
    QString bookingId;
    QString customerName;
    QString phone;
    QString roomNumber;
    QString roomType;
    QString checkInDate;
    QString expectedCheckOutDate;
    int nights = 0;
    double roomCharge = 0.0;
    double discount = 0.0;
    double deposit = 0.0;
    QVector<CheckoutServicePreview> services;
};

class CheckoutPage : public QWidget
{
    Q_OBJECT

public:
    explicit CheckoutPage(QWidget *parent = nullptr);

private:
    void setupUi();
    void loadMockBookings();
    void populateBookingTable(const QString &filter = QString());
    void showBookingDetails(int row);
    void clearBookingDetails();
    void showConfirmDialog();

    double serviceCharge(const CheckoutBookingPreview &booking) const;
    double totalCharge(const CheckoutBookingPreview &booking) const;

    QVector<CheckoutBookingPreview> bookings;

    QLineEdit *searchEdit = nullptr;
    QTableWidget *bookingTable = nullptr;
    QTableWidget *serviceTable = nullptr;
    QWidget *detailsContainer = nullptr;

    QLabel *bookingIdLabel = nullptr;
    QLabel *customerNameLabel = nullptr;
    QLabel *phoneLabel = nullptr;
    QLabel *roomLabel = nullptr;
    QLabel *roomTypeLabel = nullptr;
    QLabel *checkInLabel = nullptr;
    QLabel *expectedCheckOutLabel = nullptr;
    QLabel *nightsLabel = nullptr;

    QLabel *roomChargeLabel = nullptr;
    QLabel *serviceChargeLabel = nullptr;
    QLabel *discountLabel = nullptr;
    QLabel *depositLabel = nullptr;
    QLabel *totalLabel = nullptr;

    QComboBox *paymentMethodComboBox = nullptr;
    QPushButton *confirmButton = nullptr;
};