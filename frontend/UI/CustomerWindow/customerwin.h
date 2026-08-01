#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QDateEdit>
#include <QSpinBox>
#include <QString>
#include <QCheckBox>

class CustomerInputWindow : public QWidget
{
    Q_OBJECT

public:
    CustomerInputWindow(QWidget *parent = nullptr);

private:
    QLineEdit *txtName;
    QLineEdit *txtPhone;
    QDateEdit *dateCheckIn;
    QDateEdit *datecheckout;
    QSpinBox *spinPeople;
    QPushButton *btnNext;
    QLineEdit *ID;
    QCheckBox *chkMembership;
    void onNextClicked();
};

class CustomerWindow : public QWidget
{

public:
    CustomerWindow(QString name, QString phone, QString id, QString date, QString datecheckout, int people, bool isMembership, QWidget *parent = nullptr);

private:
    QTableWidget *tableRoom;
    QPushButton *btnBook;

    QString customerName;
    QString customerPhone;
    QString ID;
    QString checkInDate;

    QString datecheckout;

    int numPeople;
    bool isMembership;

    void loadFilteredRooms();
    void onBookRoomClicked();
};