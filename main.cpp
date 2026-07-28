#include "backend/Manager/DatabaseManager.h"
#include "frontend/UI/Login/Login.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (!DatabaseManager::instance().open())
    {
        qDebug() << "ERROR: Khong mo duoc Database!";
    }

    LoginWindow loginWindow;
    loginWindow.show();
    return app.exec();
}
