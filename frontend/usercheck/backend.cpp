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
#include <QColor>
#include <QFont>

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

    QSqlRecord rec = query.record();
    int columnCount = rec.count();

    // Preserve existing horizontal header labels if column count matches and headers are non-empty
    QStringList existingHeaders;
    if (table->columnCount() == columnCount)
    {
        for (int i = 0; i < columnCount; ++i)
        {
            QTableWidgetItem *hItem = table->horizontalHeaderItem(i);
            if (hItem && !hItem->text().isEmpty())
            {
                existingHeaders << hItem->text();
            }
        }
    }

    table->clearContents();
    table->setColumnCount(columnCount);
    table->setRowCount(0);

    // Check if query record provides explicit custom header aliases
    bool hasQueryAliases = false;
    for (int i = 0; i < columnCount; ++i)
    {
        QString fname = rec.fieldName(i);
        if (fname.contains(' ') || fname != fname.toLower())
        {
            hasQueryAliases = true;
            break;
        }
    }

    if (hasQueryAliases || existingHeaders.size() != columnCount)
    {
        QStringList headers;
        for (int i = 0; i < columnCount; ++i)
        {
            headers << rec.fieldName(i);
        }
        table->setHorizontalHeaderLabels(headers);
    }
    else
    {
        table->setHorizontalHeaderLabels(existingHeaders);
    }

    int row = 0;
    while (query.next())
    {
        table->insertRow(row);
        for (int col = 0; col < columnCount; ++col)
        {
            QString valStr = query.value(col).toString();
            QTableWidgetItem *item = new QTableWidgetItem(valStr);

            if (valStr.contains("Available", Qt::CaseInsensitive) || valStr == "0")
            {
                item->setForeground(QColor("#16a34a")); // Emerald green
                item->setBackground(QColor("#f0fdf4")); // Soft green highlight background
                QFont font = item->font();
                font.setBold(true);
                item->setFont(font);
            }

            table->setItem(row, col, item);
        }
        row++;
    }
}
