#pragma once

#include <QWidget>
#include <QStringList>

class QTableWidget;
class QPushButton;
class QStackedWidget;

class TablePage : public QWidget
{
public:
    TablePage(const QStringList &headers, QWidget *parent = nullptr);

    QTableWidget *table() const;

private:
    QTableWidget *tableWidget;
};

class SplitTablePage : public QWidget
{
public:
    SplitTablePage(const QString &firstTitle, const QStringList &firstHeaders,
                   const QString &secondTitle, const QStringList &secondHeaders,
                   QWidget *parent = nullptr);

    QTableWidget *firstTable() const;
    QTableWidget *secondTable() const;
    QPushButton *firstButton() const;
    QPushButton *secondButton() const;
    void setCurrentSection(int section);

private:
    void updateTabState(int section);

    QStackedWidget *sectionStack;
    TablePage *firstPage;
    TablePage *secondPage;
    QPushButton *firstTabButton;
    QPushButton *secondTabButton;
};
