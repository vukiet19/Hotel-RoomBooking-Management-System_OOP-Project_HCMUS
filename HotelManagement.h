#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_HotelManagement.h"

class HotelManagement : public QMainWindow
{
    Q_OBJECT

public:
    HotelManagement(QWidget *parent = nullptr);
    ~HotelManagement();

private:
    Ui::HotelManagementClass ui;
};

