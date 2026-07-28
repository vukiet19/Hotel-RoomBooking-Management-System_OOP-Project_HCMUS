#pragma once

#include <QObject>
#include <QString>
#include "frontend/Observers/Observer.h"

// Qt-compatible Observer bridge that translates C++ HotelEventManager callbacks into Qt signals.
class QtHotelObserver : public QObject, public RoomObserver, public BookingObserver
{
    Q_OBJECT

private:
    QtHotelObserver(QObject *parent = nullptr);
    ~QtHotelObserver() override = default;

public:
    static QtHotelObserver &instance();

    // Overridden from RoomObserver
    void onRoomStatusChanged(const RoomEvent &event) override;

    // Overridden from BookingObserver
    void onBookingStatusChanged(const BookingEvent &event) override;

signals:
    void roomStatusChanged(QString roomId, int newStatusInt, QString timestamp);
    void bookingStatusChanged(int bookingId, QString customerName, QString roomNumber, int newStatusInt, double totalPrice, QString timestamp);
};
