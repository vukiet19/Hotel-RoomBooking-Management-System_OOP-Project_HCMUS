// File .h chứa  định nghĩa class của MembershipPolicy và định nghĩa hàm
//  Các file liên quan: Customer.cpp(do customer là class con của MembershipPolicy nên trong phần constructor có tính chất kế thừa),Membership.cpp

#pragma once
#include <map>
#include "MembershipTier.h"

using namespace std;

class MembershipPolicy

{
private:
    // thuộc tính
    int point;
    MembershipTier tier;
    map<MembershipTier, double> discountRates;

public:
    // constructor
    MembershipPolicy();

    MembershipPolicy(MembershipTier tier);

    double getDiscountRate(MembershipTier tier);

    MembershipTier getTier();

    int getPoint();

    void setDiscount(double rate, MembershipTier tier);

    void setTier(MembershipTier tier);

    void setPoint(int point);
};