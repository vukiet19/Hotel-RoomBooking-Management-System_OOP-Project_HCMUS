#pragma once

#include <QWidget>

class TablePage;
class QTableWidget;

class BillPage : public QWidget
{
public:
    explicit BillPage(QWidget *parent = nullptr);
    QTableWidget *table() const;

private:
    TablePage *content;
};
