#include "RoomPage.h"

#include "frontend/UI/ManagerWindow/Common/PageWidgets.h"
#include <QVBoxLayout>

RoomPage::RoomPage(QWidget *parent)
    : QWidget(parent),
      sections(new SplitTablePage(
          "Room", {"Room ID", "Room Number", "Type", "Status", "Price", "Number of People"},
          "Room Type", {"Type", "Base Price"}, this))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(sections);
}

QTableWidget *RoomPage::roomTable() const { return sections->firstTable(); }
QTableWidget *RoomPage::roomTypeTable() const { return sections->secondTable(); }
QPushButton *RoomPage::roomTabButton() const { return sections->firstButton(); }
QPushButton *RoomPage::roomTypeTabButton() const { return sections->secondButton(); }
void RoomPage::setSection(int section) { sections->setCurrentSection(section); }
