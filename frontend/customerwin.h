#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QDateEdit>
#include <QSpinBox>
#include <QString>

class CustomerInputWindow : public QWidget
{
    Q_OBJECT

public:
    CustomerInputWindow(QWidget *parent = nullptr);

private:
    QLineEdit *txtName;
    QLineEdit *txtPhone;
    QDateEdit *dateCheckIn;
    QSpinBox *spinPeople;
    QPushButton *btnNext;
    QLineEdit *ID;
    void onNextClicked();
};

class CustomerWindow : public QWidget
{

public:
    CustomerWindow(QString name, QString phone, QString id, QString date, int people, QWidget *parent = nullptr);

private:
    QTableWidget *tableRoom;
    QPushButton *btnBook;

    QString customerName;
    QString customerPhone;
    QString ID;
    QString checkInDate;
    int numPeople;

    void loadFilteredRooms();
    void onBookRoomClicked();
};