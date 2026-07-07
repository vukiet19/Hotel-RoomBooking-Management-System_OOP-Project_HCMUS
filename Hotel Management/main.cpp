#include "DatabaseManager.h"
#include "DashboardService.h"
#include "BookingRepository.h"
#include "DerivedRooms.h"
#include "Room.h"
#include "BillingService.h"
#include "Invoice.h"
#include "InvoiceRepository.h"
#include "ServiceItem.h"
#include "ServiceItemRepository.h"
#include "RoomFactory.h"
#include <QDebug>
#include <iostream>
#include <iomanip>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
using namespace std;

int main(int argc, char* argv[])
{
    // Ensure a Q(Core)Application exists so Qt internals and plugins remain
    // valid while we open/close database connections.
    QApplication app(argc, argv);

    // Create the DatabaseManager instance after the Q(Core)Application
    // so its shutdown() can safely call Qt internals.
    DatabaseManager::instance();
    RoomFactory factory;
    RoomFilter myFilter;
    myFilter.numberPeople = 2;          
    myFilter.typeRoom = "StandardRoom"; 
    myFilter.maxPriceRoom = 7000000;   
    myFilter.sortByPriceAscending = true;
    QVector<Room*> results = factory.checkAvailableRooms(myFilter);
    qDebug() << "KET QUA TIM KIEM: ";
    if (results.isEmpty()) {
        qDebug() << "Khong tim thay phong nao phu hop!";
    }
    else {
        for (const auto& room : results) {
            qDebug() << "Phong:" << room->getRoomNumber().c_str()
                << "| Loai:" << roomTypeToString(room->getType()).c_str();
        }
    }

    for (auto room : results) {
        delete room;
    }
    results.clear();

    DatabaseManager::instance().close();

    return 0;

}