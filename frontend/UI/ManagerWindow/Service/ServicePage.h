#pragma once
#include <QWidget>

class TablePage;
class QTableWidget;

class ServicePage : public QWidget
{
public:
    explicit ServicePage(QWidget *parent = nullptr);
    QTableWidget *serviceCatalogTable() const;

private:
    TablePage *content;
};