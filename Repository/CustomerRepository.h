//GEN AI dựa vào booking duyệt lại th nên logic gần như i chang

//SỰ KHÁC BIỆT CHÍNH VỚI BOOKINGREPOSITORY LÀ:
//Không có convert enum vì database dùng int, còn Customer dùng enum trong code C++
//Có thêm hàm xử lý điểm và tier 

#pragma once
#include "../Customer/Customer.h"
#include <vector>

using namespace std;

class CustomerRepository
{
public:
    CustomerRepository();

    // Hàm add cũ (giữ lại để ko conflict code cũ)
    bool add(Customer customer);

    // Thêm một Customer mới vào database, trả về ID tự sinh (hoặc -1 nếu lỗi)
    int add(Customer *customer);

    // Cập nhật thông tin của Customer trong database
    bool update(Customer *customer);

    // Xóa một Customer ra khỏi database dựa theo ID
    bool remove(int customerId);

    // Lấy thông tin một Customer cụ thể từ database dựa theo ID
    // Note: Đối tượng trả về được cấp phát động, caller tự chịu trách nhiệm giải phóng (delete)
    Customer *getById(int customerId);

    // Trả về danh sách tất cả các Customer hiện có trong database
    vector<Customer *> getAll();

private:
    // Kiểm tra schema của bảng Customer (tự động thêm các cột points và membership_tier nếu thiếu)
    void verifySchema();
};