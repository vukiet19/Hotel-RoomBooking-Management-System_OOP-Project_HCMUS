#include "backend.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QSqlQuery>
#include "cores/Room/RoomFactory.h"
#include "cores/Room/DerivedRooms.h"
#include "cores/Room/TypeRoom.h"
#include "cores/Room/Room.h"
#include <QSqlRecord>
#include "cores/Room/DepositStatus.h"
#include "backend/Manager/DatabaseManager.h"
#include "backend/Repository/CustomerRepository.h"
#include "cores/Customer/Customer.h"
#include "backend/Repository/RoomRepository.h"

bool Backend::checkValidUsername(const std::string &g)
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

bool Backend::checkValidPassword(const std::string &g, const std::string &p)
{
    return g == p;
}

void Backend::loadTableData(QTableWidget *table, const QString &queryStr)
{
    QSqlQuery query(queryStr);

    if (!query.exec())
    {
        qDebug() << "[ERROR] Load table failed:" << query.lastError().text();
        return;
    }

    table->clear();
    QSqlRecord rec = query.record();
    int columnCount = rec.count();

    table->setColumnCount(columnCount);
    table->setRowCount(0);

    QStringList headers;
    for (int i = 0; i < columnCount; ++i)
    {
        headers << rec.fieldName(i);
    }
    table->setHorizontalHeaderLabels(headers);

    int row = 0;
    while (query.next())
    {
        table->insertRow(row);
        for (int col = 0; col < columnCount; ++col)
        {
            table->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        }
        row++;
    }
}
