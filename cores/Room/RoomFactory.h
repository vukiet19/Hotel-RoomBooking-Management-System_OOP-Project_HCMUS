#pragma once
#include <QVector>
#include "Room.h"

struct RoomFilter {
	QString typeRoom;
	int numberPeople;
	double maxPriceRoom;
	bool sortByPriceAscending;
};

class RoomFactory
{
public:
	QVector<Room*> checkAvailableRooms(const RoomFilter& filter);
};

