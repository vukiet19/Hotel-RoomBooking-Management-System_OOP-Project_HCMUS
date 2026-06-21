#pragma once
#include <iostream>
#include <string>
#include <stdexcept> // dung cho throw
#include <cctype> // dungf cho isdigit
#include "MembershipTier.h"
#include "MembershipPolicy.h"

using namespace std;

class Customer : public MembershipPolicy {
private:
    int id;
    string fullname;
    string phone;
    string idcard;
    MembershipTier tier;

public:
    static int nextId;

    // Constructor
    Customer(string fullname, string phone, string idcard, MembershipTier tier) : MembershipPolicy() {
        this->id = nextId++;
        this->fullname = fullname;

        //check dieu kien cua cac chi so nay
        setPhone(phone);
        setIdcard(idcard); 

        this->tier = tier;
    }

    //getter,setter

    void setId(int id) { this->id = id; }
    
    void setPhone(string phone) { 
        if (phone.length() < 10) {
            throw invalid_argument("Error: Phone number must be at least 10 digits long.");
        }
        
        for (char g : phone) {
            if (!isdigit(g)) {
                throw invalid_argument("Error: Phone number must contain only numbers.");
            }
        }
        this->phone = phone;
    }
    
    void setIdcard(string idcard) { 
        if (idcard.length() < 10) { 
            throw invalid_argument("Error: ID Card must be at least 10 digits long.");
        }
        
        for (char g : idcard) {
            if (!isdigit(g)) { 
                throw invalid_argument("Error: ID Card must contain only numbers.");
            }
        }
        this->idcard = idcard; 
    }
    
    void setMembershipTier(MembershipTier tier) { this->tier = tier; }

    int getId() { return this->id; }
    string getPhone() { return this->phone; }
    string getIdcard() { return this->idcard; }
    MembershipTier getTier() { return this->tier; }

    void display() {
        cout << "Customer id: " << this->id << '\n';
        cout << "Customer phone: " << this->phone << '\n';
        cout << "Customer idcard: " << this->idcard << '\n';
        cout << "Current MembershipTier: " << this->tier << '\n';
        cout << "Discount: " << getDiscountRate(this->tier) * 100 << "%" << '\n';
    }
};