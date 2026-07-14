// File BookingRepos ~ trạm trung chuyển: kết nối giữa database và back-end: đọc dữ liệu Customer từ file Customer lên database

#pragma once
#include "../Customer/Customer.h"
#include <QString>

class CustomerRepository
{
public:
    bool add(Customer customer);
    std::vector<Customer> filter(QString columnName, QString searchValue);
    bool update(Customer customer);
    bool remove(std::string id_customer);
};