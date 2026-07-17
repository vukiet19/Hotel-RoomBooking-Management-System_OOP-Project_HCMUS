//GEN AI dựa vào booking duyệt lại th nên logic gần như i chang

//SỰ KHÁC BIỆT CHÍNH VỚI BOOKINGREPOSITORY LÀ:
//1. CONVERT ROOM TYPE INTEGER -> STRING (database)
//2. CÓ THÊM HÀM TÌM THÔNG TIN GIÁ PHÒNG DỰA VÀO LOẠI PHÒNG

#pragma once
#include "../Room/Room.h"
#include <QString>
#include <vector>
#include <string>

using namespace std;

class RoomRepository
{
public:
    RoomRepository();

    // Hàm lấy giá cũ (giữ lại để tương thích ngược với code cũ)
    double getRoomPriceData(const QString& typeRoom);

    // Thêm một Room mới vào database (ListRooms), trả về ID tự sinh dạng integer
    int add(Room *room);

    // Cập nhật thông tin của Room trong database (ListRooms)
    bool update(Room *room);

    // Xóa một Room ra khỏi database dựa theo chuỗi ID (ví dụ: "R0001")
    bool remove(const string &roomId);

    // Lấy thông tin một Room cụ thể từ database dựa theo chuỗi ID (ví dụ: "R0001")
    // Tự động phân loại và khởi tạo đúng lớp con kế thừa (StandardRoom, VipRoom, PresiRoom)
    // Note: Đối tượng trả về được cấp phát động, caller tự chịu trách nhiệm giải phóng (delete)
    Room *getById(const string &roomId);

    // Trả về danh sách tất cả các Room hiện có trong database
    vector<Room *> getAll();

private:
    // Kiểm tra schema của bảng Room
    void verifySchema();
};
