#pragma once
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QString>
#include <QVector>
#include "Invoice.h"

class InvoiceRepository
{
public:
	InvoiceRepository() = default;
	bool add(const Invoice& invoice); //Them hoa don, True khi them thanh cong, False khi them that bai
	bool update(const Invoice& invoice); //Cap nhat thong tin hoa don, True khi cap nhat thanh cong, False khi cap nhat that bai
	Invoice findByBookingId(int bookingId); //Tim kiem hoa don thong qua BookingId
};

