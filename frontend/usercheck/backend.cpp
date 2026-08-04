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

            QString fieldName = rec.fieldName(col);
            if (fieldName.contains("status", Qt::CaseInsensitive))
            {
                QFont font = item->font();
                font.setBold(true);
                item->setFont(font);

                if (valStr.compare("Occupied", Qt::CaseInsensitive) == 0)
                {
                    item->setText("OCCUPIED");
                    item->setForeground(QColor("#ffffff")); // Vivid white text
                    item->setBackground(QColor("#dc2626")); // Bold crimson red badge highlight
                }
                else if (valStr.compare("Available", Qt::CaseInsensitive) == 0)
                {
                    item->setText("available");
                    item->setForeground(QColor("#15803d")); // Green text
                    item->setBackground(QColor("#dcfce7")); // Soft green background
                }
                else if (valStr.compare("CONFIRMED", Qt::CaseInsensitive) == 0 ||
                         valStr.compare("CHECKED_IN", Qt::CaseInsensitive) == 0)
                {
                    item->setForeground(QColor("#15803d")); // Green text
                    item->setBackground(QColor("#dcfce7")); // Soft green background
                }
                else if (valStr.compare("Maintenance", Qt::CaseInsensitive) == 0)
                {
                    item->setForeground(QColor("#dc2626")); // Red text
                    item->setBackground(QColor("#fee2e2")); // Soft red background
                }
                else if (valStr.compare("Reserved", Qt::CaseInsensitive) == 0 ||
                         valStr.compare("UNCONFIRMED", Qt::CaseInsensitive) == 0 ||
                         valStr.compare("HELD", Qt::CaseInsensitive) == 0)
                {
                    item->setForeground(QColor("#d97706")); // Amber text
                    item->setBackground(QColor("#fef3c7")); // Soft amber background
                }
                else if (valStr.compare("CHECKED_OUT", Qt::CaseInsensitive) == 0 ||
                         valStr.compare("RETURNED", Qt::CaseInsensitive) == 0 ||
                         valStr.compare("NONE", Qt::CaseInsensitive) == 0)
                {
                    item->setForeground(QColor("#64748b")); // Slate text
                    item->setBackground(QColor("#f1f5f9")); // Soft slate background
                }
            }

            table->setItem(row, col, item);
        }
        row++;
    }
}
