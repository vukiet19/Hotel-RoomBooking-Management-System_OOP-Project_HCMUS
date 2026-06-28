#pragma once
#include "ServiceItem.h"
#include <QString>
#include <qVector>

class ServiceItemRepository
{
public:
	ServiceItemRepository() = default;

	//Lấy toàn bộ menu dịch vụ (dùng để load lên UI)
	QVector<ServiceItem> getAll();

	//Tìm kiếm 1 dịch vụ thông qua itemId (VD: M_PEPSI)
	ServiceItem findById(const QString& itemId);

	//Lấy danh sách dịch vụ thông qua category
	QVector<ServiceItem> getByCategory(const QString& category);
};

