#pragma once

#include <QWidget>

class SplitTablePage;
class QTableWidget;
class QPushButton;

class ServicePage : public QWidget
{
public:
    explicit ServicePage(QWidget *parent = nullptr);

    QTableWidget *serviceCatalogTable() const;
    QTableWidget *foodOptionsTable() const;
    QPushButton *serviceCatalogTabButton() const;
    QPushButton *foodOptionsTabButton() const;
    void setSection(int section);

private:
    SplitTablePage *sections;
};
