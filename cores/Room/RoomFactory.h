#pragma once
#include <QVector>
#include "Room.h"
#include <vector>
using namespace std;

struct RoomFilter {
	QString typeRoom;
	int numberPeople;
	double maxPriceRoom;
	bool sortByPriceAscending;
};

class RoomFactory
{
public:
	//sửa thành unique pointer phòng khi quên delete
	vector<unique_ptr<Room>> checkAvailableRooms(const RoomFilter& filter);
};

