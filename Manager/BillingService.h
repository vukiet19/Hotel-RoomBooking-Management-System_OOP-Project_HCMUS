#pragma once
#include "Invoice.h"
#include "InvoiceRepository.h"
#include "../Service/ServiceItem.h"
#include "../Service/ServiceItemRepository.h"
#include "../Booking/Booking.h"

class BillingService
{
private:
	InvoiceRepository invoiceRepo;
public: 
	double calculateServiceCharge(const Booking& booking);
	BillingService() = default;
	Invoice generateInvoice(const Booking& booking, double baseRoomCharge);
};

