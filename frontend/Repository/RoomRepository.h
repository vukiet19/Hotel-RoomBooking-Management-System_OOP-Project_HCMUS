// ???: trước mắt là lấy baseRoomPrice từ database
// Hàm này sẽ liên kết với DatabaseManager dể đọc/ghi thông tin lên database

#pragma once
#include "Room/Room.h"
#include <QString>
#include <vector>

class RoomRepository
{
public:
	double getRoomPriceData(const QString &typeRoom);
	bool add(const Room &r);
	std::vector<Room *> getRoomsByCapacity(int numPeople);
};
