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

    Customer(string fullname, string phone, string idcard, MembershipTier tier = Unknown);

    // getter,setter

    void setId(int id);

    void setPhone(string phone);

    void setIdcard(string idcard);

    void setIdroom(string idRoom);

    string getIdRoom();

    int getId();
    string getPhone();
    string getIdcard();

    void display();
};