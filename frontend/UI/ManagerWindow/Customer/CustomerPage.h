#pragma once

#include <QWidget>

class TablePage;
class QTableWidget;

class CustomerPage : public QWidget
{
public:
    explicit CustomerPage(QWidget *parent = nullptr);
    QTableWidget *table() const;

private:
    TablePage *content;
};
