#include "Room.h"
#include "RoomRepository.h"

void Room::setTypeRoom(string typeRoom){
	this->typeRoom = typeRoom;
}

void Room::setBasePrice() {
	RoomRepository rr;
	QString typeRoom = QString::fromStdString(this->typeRoom);
	double price = rr.getRoomPriceData(typeRoom);
	if (price) {
		this->basePrice = price;
		return;
	}
	throw "Price loi (0.0)";
}

double Room::getBasePrice() {
	return basePrice;
}