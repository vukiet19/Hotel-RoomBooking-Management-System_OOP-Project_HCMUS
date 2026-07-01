#include "Manager/DatabaseManager.h"
#include "Manager/DashboardService.h"
#include "Booking/BookingRepository.h"
#include "Customer/CustomerRepository.h"
#include "Room/Room.h"
#include "Room/DerivedRooms.h"
#include "Booking/Booking.h"
#include "Booking/BookingFactory.h"
#include "Booking/BookingType.h"

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
		cout << ds.getTodayBookings() << ' ' << fixed << setprecision(0) << ds.getRevenue("year") << endl;

		DatabaseManager::instance().close();*/

		cout << "test\n";
		
		//auto cho tiện minh hoạ th chứ lúc làm cứ viết thẳng nó ra ví du unique_ptr<Customer> guest 1;
		auto guest1 = make_unique<Customer>("Alice", "0000000000", "0000000000", MembershipTier::Gold);
		auto guest2 = make_unique<Customer>("Bob", "0000000001", "0000000001", MembershipTier::Silver); 
			
		auto room101 = make_unique<StandardRoom>("S-101");
		auto room102 = make_unique<VipRoom>("V-201");


		room101->setBasePrice(500000);
		room102->setBasePrice(500000);

		QDateTime checkInDate = QDateTime::currentDateTime();
		QDateTime checkOutDate = checkInDate.addDays(3);

		//status room 101 đầu
		cout<<"Status room 101 pre-book"<<'\n';

		//GET LÀ HÀM CHO MƯỢN(ĐƯỢC NHÌN NHƯNG KO ĐƯỢC XOÁ) (MOVE MỚI LÀ CHUYỂN NHƯỢNG QUYỀN SỞ HỮU)
		printRoomStatus("Room 101 (Pre-book)", room101.get());

		unique_ptr<StandardRoomBooking> booking1(static_cast<StandardRoomBooking*>(
			BookingFactory::createBooking(BookingType::STANDARD, guest1.get(), checkInDate, checkOutDate, room101.get(), 50.00)
		));
		
		cout<<"Status room 101 sau book"<<'\n';
		//nên hiện status reserved
		printRoomStatus("Room 101 (Post-book)", room101.get());

		cout<<"hoàn tất reservation (giai quyết deposi)"<<'\n';
		//nên hiện occupied 
		booking1->resolveDeposit();
		printRoomStatus("Room 101 (Checked In)", room101.get());

		unique_ptr<Booking> booking2(
			BookingFactory::createBooking(BookingType::STANDARD, guest2.get(), checkInDate, checkOutDate, room102.get(), 0.00)
		);
		printRoomStatus("Room 102", room102.get());

		unique_ptr<Booking> doubleBookingAttempt(
			BookingFactory::createBooking(BookingType::STANDARD, guest1.get(), checkInDate, checkOutDate, room102.get(), 0.00)
		);
			
		if (doubleBookingAttempt == nullptr) {
			cout << "không cho tiếp cận room status occupied\n";
		}


	}
	else {
		qDebug() << "ERROR: Khong mo duoc Database!";
	}

	return 0;
}