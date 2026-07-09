#pragma once
#include <QDateTime>
#include <QString>

class Invoice {
private:
    int id;
    QDateTime createdAt;
    double totalAmount;
    QString paymentStatus;
    int bookingId;

public:
    Invoice(int id = 0, const QDateTime& createdAt = QDateTime(), double totalAmount = 0.0, const QString& paymentStatus = "", int bookingId = 0) : id(id), createdAt(createdAt), totalAmount(totalAmount), paymentStatus(paymentStatus), bookingId(bookingId) {};

    int getId() const { 
        return id; 
    }
    
    QDateTime getCreatedAt() const { 
        return createdAt; 
    }
    
    double getTotalAmount() const { 
        return totalAmount; 
    }
    
    QString getPaymentStatus() const { 
        return paymentStatus; 
    }
    
    int getBookingId() const { 
        return bookingId; 
    }
    
    void setTotalAmount(double amount) { 
        totalAmount = amount; 
    }
    
    void setPaymentStatus(const QString& status) { 
        paymentStatus = status; 
    }
};

