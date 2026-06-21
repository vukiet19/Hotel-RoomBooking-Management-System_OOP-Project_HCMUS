#include <iostream>
#include "Customer.h"
#include "MembershipTier.h"

using namespace std;

int Customer::nextId = 0;

int main() {
    Customer a("Megumin", "1234567890", "1234567890", Sliver);
    a.display();
    
    return 0;
}