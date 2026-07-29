#pragma once

#include <QWidget>

class SplitTablePage;
class QTableWidget;
class QPushButton;

class InventoryPage : public QWidget
{
public:
    explicit InventoryPage(QWidget *parent = nullptr);

    QTableWidget *inventoryTable() const;
    QTableWidget *inventoryLogTable() const;
    QPushButton *inventoryTabButton() const;
    QPushButton *inventoryLogTabButton() const;
    void setSection(int section);

private:
    SplitTablePage *sections;
};
