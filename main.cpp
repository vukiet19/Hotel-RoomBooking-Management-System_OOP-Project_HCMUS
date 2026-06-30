#include "CUSTOMER/Customer.h"
#include "ROOM/Room.h"
#include "ROOM/RoomStatus.h"
#include "ROOM/Typeroom.h"
#include "ROOM/DerivedRooms.h"
#include "ROOM/observer.h"

int main()
{
    // Khởi tạo customer
    Customer a("Megumin", "1234567890", "1234567890");

    // Khởi tạo phòng
    StandardRoom r1;

    a.setIdroom(r1.getId());
    cout << a.getIdRoom() << '\n';

    // Setbaseprice
    r1.setBasePrice(10000000);

    // Kiểm tra tier hiện tại của khách hàng
    cout << a.getTier() << '\n';

    // Lúc sau hàm này sẽ là checkout, dùng để tính điểm.
    r1.getBill(a);

    cout << a.getPoint() << '\n';

    cout << a.getTier() << '\n';

    for (int i = 0; i < 10; i++)
    {
        r1.getBill(a);
    }

    cout << a.getPoint() << '\n';

    cout << a.getTier() << '\n';
}