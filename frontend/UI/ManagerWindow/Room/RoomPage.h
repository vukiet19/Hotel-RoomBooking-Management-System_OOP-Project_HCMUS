#pragma once

#include <QWidget>

class SplitTablePage;
class QTableWidget;
class QPushButton;

class RoomPage : public QWidget
{
public:
    explicit RoomPage(QWidget *parent = nullptr);

    QTableWidget *roomTable() const;
    QTableWidget *roomTypeTable() const;
    QPushButton *roomTabButton() const;
    QPushButton *roomTypeTabButton() const;
    void setSection(int section);

private:
    SplitTablePage *sections;
};
