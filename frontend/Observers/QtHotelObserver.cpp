#include "QtHotelObserver.h"
#include <QDebug>

QtHotelObserver::QtHotelObserver(QObject *parent) : QObject(parent) {
  HotelEventManager::instance().addRoomObserver(this);
  HotelEventManager::instance().addBookingObserver(this);
}

QtHotelObserver &QtHotelObserver::instance() {
  static QtHotelObserver inst;
  return inst;
}

void QtHotelObserver::onRoomStatusChanged(const RoomEvent &event) {
  qDebug() << "[OBSERVER EVENT] Room:" << QString::fromStdString(event.roomId)
           << "Status:" << static_cast<int>(event.newStatus);
  emit roomStatusChanged(QString::fromStdString(event.roomId),
                         static_cast<int>(event.newStatus),
                         QString::fromStdString(event.timestamp));
}

void QtHotelObserver::onBookingStatusChanged(const BookingEvent &event) {
  qDebug() << "[OBSERVER EVENT] Booking ID:" << event.bookingId
           << "Status:" << static_cast<int>(event.newStatus);
  emit bookingStatusChanged(event.bookingId,
                            QString::fromStdString(event.customerName),
                            QString::fromStdString(event.roomNumber),
                            static_cast<int>(event.newStatus), event.totalPrice,
                            QString::fromStdString(event.timestamp));
}