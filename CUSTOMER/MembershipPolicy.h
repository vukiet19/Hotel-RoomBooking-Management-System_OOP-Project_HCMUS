#pragma once
#include <map>
#include"MembershipTier.h"

using namespace std;

class MembershipPolicy {
private:
    map<MembershipTier, double> discountRates;

public:
    MembershipPolicy() {
        discountRates[Sliver] = 0.05;
        discountRates[Gold] = 0.1;
        discountRates[Platinum] = 0.2;
    }

    double getDiscountRate(MembershipTier tier) {
        return discountRates[tier];
    }

    void setDiscount(double rate, MembershipTier tier) {
        discountRates[tier] = rate;
    }
};