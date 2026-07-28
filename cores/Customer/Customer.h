// File .h này chứa thông tin của class Customer
// File này có liên quan đến MembershipTier.h(chứa emu ) và MembershipPolicy.h ( dùng cho kế thừa)

#pragma once
#include <iostream>
#include <string>
#include <stdexcept> // dung cho throw
#include <cctype>    // dungf cho isdigit
#include "MembershipTier.h"
#include "MembershipPolicy.h"

using namespace std;

class Customer : public MembershipPolicy
{
private:
    int id;
    string fullname;
    string phone;
    string idcard;
    string idRoom;

public:
    static int nextId;

    // Constructor
    Customer();
    Customer(string fullname, string phone, string idcard, MembershipTier tier = Unknown);

    // setter
    void setId(int id);
    void setFullname(string fullname);
    void setPhone(string phone);
    void setIdcard(string idcard);
    void setIdroom(string idRoom);

    // getter
    int getId();
    string getFullname();
    string getPhone();
    string getIdcard();
    string getIdRoom();

    void display();
};