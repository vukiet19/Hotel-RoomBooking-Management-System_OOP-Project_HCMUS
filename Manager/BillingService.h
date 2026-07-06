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
	double calculateServiceCharge(const Booking& booking);
public: 
	BillingService() = default;
	Invoice generateInvoice(const Booking& booking, double baseRoomCharge);
};

