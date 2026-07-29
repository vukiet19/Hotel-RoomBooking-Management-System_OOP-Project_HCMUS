#pragma once

#include <QWidget>

class SplitTablePage;
class QTableWidget;
class QPushButton;

class ServicePage : public QWidget
{
public:
    explicit ServicePage(QWidget *parent = nullptr);

    QTableWidget *foodTable() const;
    QTableWidget *serviceTable() const;
    QPushButton *foodTabButton() const;
    QPushButton *serviceTabButton() const;
    void setSection(int section);

private:
    SplitTablePage *sections;
};
