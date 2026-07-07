#pragma once
#include "Invoice.h"
#include "../Repository/InvoiceRepository.h"
#include "../Service/ServiceItem.h"
#include "../Repository/ServiceItemRepository.h"

class BillingService
{
private:
	InvoiceRepository invoiceRepo;
	double calculateServiceCharge(int bookingId);
public: 
	BillingService() = default;
	Invoice generateInvoice(int bookingId, double baseRoomCharge);
};

