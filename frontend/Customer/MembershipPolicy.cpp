// Hàm này dùng để chứa các phương thức của class MembershipPolicy,gồm các getter,setter,constrcutor
// Các hàm liên quan:MembershipPolicy.h(chứa các định nghĩa của class MembershipPolicy)  và file customer( Vì customer là kế thừa từ MembershipPolicy)

#include "MembershipPolicy.h"
#include <iostream>
// Constructor default, khi khởi tạo
MembershipPolicy::MembershipPolicy()
{
    // Khi khởi tạo mặc định thì tier sẽ mặc định là Unknown( nghĩa là newbie)
    tier = Unknown;
    // set discount rate cho từng hạn mục( ko có Unknown do trong map ko định nghĩa thfi mặc định là 0)
    discountRates[Silver] = 0.05;
    discountRates[Gold] = 0.1;
    discountRates[Platinum] = 0.2;
}
// constructor (Với tier được truyền vào )
MembershipPolicy::MembershipPolicy(MembershipTier tier)
{
    discountRates[Silver] = 0.05;
    discountRates[Gold] = 0.1;
    discountRates[Platinum] = 0.2;

    // Đối với mỗi tier thì sẽ có điểm khởi đầu khác nhau, tương ứng với điểm thấp nhất của tier đó
    if (tier == 0)
        point = 0;
    if (tier == 1)
        point = 10;
    if (tier == 2)
        point = 100;
    if (tier == 3)
        point = 100;

    this->tier = tier;
}

// setter cho point, code này sẽ setter cho point và cập nhật tier nếu đủ yêu cầu
void MembershipPolicy::setPoint(int point)
{
    this->point = point;
    if (point >= 10)
        tier = Silver;
    if (point >= 100)
        tier = Gold;
    if (point >= 1000)
        tier = Platinum;
}
// getter
MembershipTier MembershipPolicy::getTier() { return tier; }

double MembershipPolicy::getDiscountRate(MembershipTier tier)
{
    return discountRates[tier];
}

// settter discountRate(sự giảm giá của từng tier)
void MembershipPolicy::setDiscount(double rate, MembershipTier tier)
{
    discountRates[tier] = rate;
}

// set tier ( Gỉa sử rằng nếu mới vô có tier đó thfi cũng sẽ set point tương ứng với điểm thấp nhất tier đó)
void MembershipPolicy::setTier(MembershipTier tier)
{
    this->tier = tier;

    if (this->tier == 0)
        point = 0;
    if (this->tier == 1)
        point = 10;
    if (this->tier == 2)
        point = 100;
    if (this->tier == 3)
        point = 1000;
}

// getter
int MembershipPolicy::getPoint()
{
    return point;
}