#pragma once
#include <QString>
#include <string>

using namespace std;

class Room
{
private:
	int id;
	string typeRoom;
	double basePrice;
	
public:
	void setTypeRoom(string typeRoom);
	void setBasePrice();
	double getBasePrice();
};

