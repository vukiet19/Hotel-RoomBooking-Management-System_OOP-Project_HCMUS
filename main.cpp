#include "Manager/DatabaseManager.h"
#include "Manager/DashboardService.h"
#include "Repository/BookingRepository.h"
#include "Repository/CustomerRepository.h"
#include "Room/Room.h"
#include "Room/DerivedRooms.h"
#include "Booking/Booking.h"
#include "Booking/BookingFactory.h"
#include "Booking/BookingType.h"
#include "Service/InventoryService.h"
#include "Repository/InventoryRepository.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <iostream>
#include <iomanip>

using namespace std;

void printRoomStatus(const string& roomName, Room* r) {
    cout << roomName << " Status: ";
    switch (r->getStatus()) {
        case RoomStatus::Available:   cout << "Available\n"; break;
        case RoomStatus::Reserved:    cout << "Reserved\n"; break;
        case RoomStatus::Occupied:    cout << "Occupied\n"; break;
        case RoomStatus::Maintenance: cout << "Maintenance\n"; break; 
    }
}

int main(int argc, char* argv[]) {
	QCoreApplication a(argc, argv);

	std::cout << "--- CHUONG TRINH BAT DAU CHAY ---" << std::endl;

	if (DatabaseManager::instance().open()) {		
		/*Customer cus("Duy", "0363667670", "0363667670");
		cus.setId(1001);
		cus.setIdroom("S1002");

		CustomerRepository repoCus;
		repoCus.add(cus);

		DashboardService ds;
		cout << ds.getTodayBookings() << ' ' << fixed << setprecision(0) << ds.getRevenue("year") << endl;*/



		/*InventoryRepository invRepo;

		// Danh sách các món đồ cần demo nạp vào kho
		struct ItemDemo {
			QString name;
			int qty;
			double price;
		};

		QList<ItemDemo> itemsToSeed = {
			{"Nước suối", 100, 10000},
			{"Khăn tắm", 50, 50000},
			{"Bàn chải", 200, 5000},
			{"Dầu gội", 80, 15000}
		};

		cout << "\n=== DANG KHOI TAO DU LIEU KHO ===" << endl;
		for (const auto& item : itemsToSeed) {
			if (invRepo.insertItem(item.name, item.qty, item.price)) {
				cout << "Da them: " << item.name.toStdString() << " thanh cong!" << endl;
			} else {
				cout << "Da them: " << item.name.toStdString() << " that bai (co the do da ton tai)." << endl;
			}
		}
		cout << "=================================\n" << endl;*/

		


		cout << "\n=== TEST INVENTORY SERVICE ===" << endl;
		InventoryService invService;
		
		// LƯU Ý: Thay "Nước suối" bằng một item_name đang có thật trong bảng Inventory trên SQLite của bạn
		QString testItem = "Khăn tắm"; 
		int testQty = 2;

		// 1. Test hàm checkStock
		cout << "[1] Kiem tra kho cho " << testItem.toStdString() << "..." << endl;
		bool hasStock = invService.checkStock(testItem, testQty);
		cout << "-> Ket qua checkStock: " << (hasStock ? "Kho du hang!" : "Kho KHONG du hang hoac khong ton tai!") << endl;

		// 2. Test hàm reserveItem (chỉ xuất kho nếu bước 1 true)
		if (hasStock) {
			cout << "\n[2] Tien hanh xuat kho " << testQty << " " << testItem.toStdString() << "..." << endl;
			bool reserved = invService.reserveItem(testItem, testQty);
			cout << "-> Ket qua reserveItem: " << (reserved ? "Thanh cong" : "That bai") << endl;
		}

		// 3. Test hàm releaseItem (Hoàn kho lại)
		/*cout << "\n[3] Tien hanh hoan tra kho " << testQty << " " << testItem.toStdString() << "..." << endl;
		bool released = invService.releaseItem(testItem, testQty);
		cout << "-> Ket qua releaseItem: " << (released ? "Thanh cong" : "That bai") << endl;

		cout << "==============================\n" << endl;*/
		
		
		DatabaseManager::instance().close();

	}
	else {
		qDebug() << "ERROR: Khong mo duoc Database!";
	}

	return 0;
}