// File BookingRepos ~ trạm trung chuyển: kết nối giữa database và back-end: đọc dữ liệu Customer từ file Customer lên database

#pragma once
#include "Customer.h"

class CustomerRepository
{
public:
    bool add(Customer customer);
};