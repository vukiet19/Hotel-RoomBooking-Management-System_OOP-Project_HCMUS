#include "backend.h"
#include "frontend.h"
#include <QSqlQuery>

Backend::Backend(QObject *parent) : QObject(parent) {}

void Backend::setMainWindowUI(QStackedWidget *stacked, QTableWidget *table1, QTableWidget *table2,
                              QTableWidget *table3, QTableWidget *table4, QTableWidget *table5,
                              QTableWidget *table6, QTableWidget *table7, QTableWidget *table8,
                              QTableWidget *table9, QTableWidget *table10)
{
    m_stackedWidget = stacked;
    m_tableBooking = table1;
    m_tableBookingItems = table2;
    m_tableCustomer = table3;
    m_tableFood = table4;
    m_tableInventory = table5;
    m_tableInventoryLog = table6;
    m_tableRoom = table7;
    m_tableRoomType = table8;
    m_tableService = table9;
    m_tableBill = table10;
}

void Backend::setLoginWindowUI(QLineEdit *user, QLineEdit *pass, QLabel *label)
{
    m_inputBox_user = user;
    m_inputBox_pass = pass;
    m_l1 = label;
}

void Backend::handleLogin_1()
{
    m_stackedWidget->setCurrentIndex(0);
    loadTableData(m_tableBooking, "SELECT * FROM Bookings");
}

void Backend::handleLogin_2()
{
    m_stackedWidget->setCurrentIndex(1);
    loadTableData(m_tableBookingItems, "SELECT * FROM BookingServiceItems");
}

void Backend::handleLogin_3()
{
    m_stackedWidget->setCurrentIndex(2);
    loadTableData(m_tableCustomer, "SELECT * FROM Customer");
}

void Backend::handleLogin_4()
{
    m_stackedWidget->setCurrentIndex(3);
    loadTableData(m_tableFood, "SELECT * FROM FoodOptions");
}

void Backend::handleLogin_5()
{
    m_stackedWidget->setCurrentIndex(4);
    loadTableData(m_tableInventory, "SELECT * FROM Inventory");
}

void Backend::handleLogin_6()
{
    m_stackedWidget->setCurrentIndex(5);
    loadTableData(m_tableInventoryLog, "SELECT * FROM InventoryLog");
}

void Backend::handleLogin_7()
{
    m_stackedWidget->setCurrentIndex(6);
    loadTableData(m_tableRoom, "SELECT * FROM ListRooms");
}

void Backend::handleLogin_8()
{
    m_stackedWidget->setCurrentIndex(7);
    loadTableData(m_tableRoomType, "SELECT * FROM RoomTypeCatalog");
}

void Backend::handleLogin_9()
{
    m_stackedWidget->setCurrentIndex(8);
    loadTableData(m_tableService, "SELECT * FROM ServicesCatalog");
}

void Backend::handleLogin_10()
{
    m_stackedWidget->setCurrentIndex(9);
    loadTableData(m_tableBill, "SELECT * FROM Bills");
}

bool Backend::check_valid_username(const std::string &g)
{
    if (g.size() == 0)
        return false;
    for (size_t i = 0; i < g.size(); i++)
    {
        if (g[i] == ' ')
            return false;
    }
    return true;
}

bool Backend::check_valid_password(const std::string &g, const std::string &p)
{
    return g == p;
}

void Backend::handleLogin()
{
    if (!m_inputBox_user || !m_inputBox_pass || !m_l1)
        return;

    QString user = m_inputBox_user->text();
    QString pass = m_inputBox_pass->text();

    if (check_valid_username(user.toStdString()) && check_valid_password(pass.toStdString(), "suuu"))
    {
        m_l1->setText("Correct");
        m_l1->setGeometry(325, 560, 400, 50);
        m_l1->setStyleSheet("font-size: 18px; font-weight: bold; color: green;");
        m_l1->show();

        if (m_mainWindow)
        {
            m_mainWindow->show();
            // Close the login window
            if (parent())
            {
                QWidget *loginWidget = qobject_cast<QWidget *>(parent());
                if (loginWidget)
                {
                    loginWidget->close();
                }
            }
        }
    }
    else
    {
        m_l1->setText("Incorrect username or password");
        m_l1->setGeometry(325, 560, 400, 50);
        m_l1->setStyleSheet("font-size: 18px; font-weight: bold; color: RED;");
        m_l1->show();
    }
}

void Backend::loadTableData(QTableWidget *table, QString queryStr)
{
    if (!table)
        return;

    table->setRowCount(0);

    if (!DatabaseManager::instance().open())
        return;

    QSqlQuery query(DatabaseManager::instance().database());
    if (query.exec(queryStr))
    {
        int row = 0;
        while (query.next())
        {
            table->insertRow(row);
            for (int col = 0; col < table->columnCount(); ++col)
            {
                table->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
            }
            row++;
        }
    }
}