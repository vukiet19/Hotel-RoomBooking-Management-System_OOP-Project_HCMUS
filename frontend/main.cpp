#include <QApplication>
#include "frontend.h"
#include "Manager/DashboardService.h"
#include "Manager/DatabaseManager.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    LoginWindow window;
    window.show();

    return app.exec();
}