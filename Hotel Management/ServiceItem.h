#pragma once
#include <QString>
class ServiceItem
{
private:
	QString itemId;
	QString itemName;
	QString category;
	double basePrice;
	bool vipFreeStatus;
public:
	ServiceItem(const QString& id = "", const QString& name = "", const QString& category = "", double price = 0.0, bool vipFree = false) : itemId(id), itemName(name), category(category), basePrice(price), vipFreeStatus(vipFreeStatus) {};

	//Getters:
	QString getId() const {
		return itemId;
	}
	QString getName() const {
		return itemName;
	}
	QString getCategory() const {
		return category;
	}
	double getBasePrice() const {
		return basePrice;
	}
	bool isVipFree() const {
		return vipFreeStatus;
	}

	//Setters:
	void setId(const QString& id) {
		itemId = id;
	}
	void setName(const QString& name) {
		itemName = name;
	}
	void setCategory(const QString& ctg) {
		category = ctg;
	}
	void setBasePrice(double price) {
		basePrice = price;
	}
	void setVipFreeStatus(bool status) {
		vipFreeStatus = status;
	}
};

