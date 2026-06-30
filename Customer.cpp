// Hàm này chứa các phương thức của customer. Gồm các constructor,getter,setter, check logic của phần setter và phần display in ra thông tin
// Các file liên quan: MemberhshipPolicy.cpp( trong đó class customer  đc kế thừa từ lớp MembershipPolicy)

#include "Customer.h"

using namespace std;
// Biến static dùng để lưu id của customer
int Customer::nextId = 0;

// Constructor, trong đó nhận kế thừa 'tier' từ class cha MembershipPolicy
Customer::Customer(string fullname, string phone, string idcard, MembershipTier tier) : MembershipPolicy(tier)
{
    this->id = nextId++;
    this->fullname = fullname;

    // check dieu kien cua cac chi so nay
    setPhone(phone);
    setIdcard(idcard);
}

// getter,setter

void Customer::setId(int id) { this->id = id; }

void Customer::setPhone(string phone)
{
    // Kiểm tra xem phone có độ dài 10 không
    if (phone.length() != 10)
    {
        throw invalid_argument("Error: Phone number must be 10 digits long.");
    }

    // Kiểm tra có tồn tại ký tự không phải số không
    for (char g : phone)
    {
        if (!isdigit(g))
        {
            throw invalid_argument("Error: Phone number must contain only numbers.");
        }
    }
    this->phone = phone;
}

// logic hàm setter của idCard giống như hàm phone trên
void Customer::setIdcard(string idcard)
{
    if (idcard.length() != 10)
    {
        throw invalid_argument("Error: ID Card must be 10 digits long.");
    }

    for (char g : idcard)
    {
        if (!isdigit(g))
        {
            throw invalid_argument("Error: ID Card must contain only numbers.");
        }
    }
    this->idcard = idcard;
}

// getter

int Customer::getId() { return this->id; }

string Customer::getPhone() { return this->phone; }

void Customer::setIdroom(string idRoom) { this->idRoom = idRoom; }

string Customer::getIdcard() { return this->idcard; }

string Customer::getIdRoom() { return idRoom; }

/// hàm để display(lúc sau có thể đổi để cho phù hợp)
void Customer::display()
{
    cout << "Customer id: " << this->id << '\n';
    cout << "Customer phone: " << this->phone << '\n';
    cout << "Customer idcard: " << this->idcard << '\n';
}
