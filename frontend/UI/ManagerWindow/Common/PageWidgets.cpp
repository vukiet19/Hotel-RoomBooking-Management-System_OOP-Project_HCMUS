#include "PageWidgets.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QVBoxLayout>

TablePage::TablePage(const QStringList &headers, QWidget *parent)
    : QWidget(parent), tableWidget(new QTableWidget(0, headers.size(), this)) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  tableWidget->setHorizontalHeaderLabels(headers);
  tableWidget->horizontalHeader()->setSectionResizeMode(
      headers.size() <= 7 ? QHeaderView::Stretch : QHeaderView::Interactive);
  tableWidget->horizontalHeader()->setMinimumSectionSize(120);
  tableWidget->horizontalHeader()->setDefaultSectionSize(140);
  tableWidget->horizontalHeader()->setStretchLastSection(headers.size() > 7);
  tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
  tableWidget->verticalHeader()->setDefaultSectionSize(45);
  tableWidget->verticalHeader()->setMinimumWidth(40);
  tableWidget->setAlternatingRowColors(true);
  tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  tableWidget->setTextElideMode(Qt::ElideRight);
  tableWidget->setWordWrap(false);
  tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  layout->addWidget(tableWidget);
}

QTableWidget *TablePage::table() const { return tableWidget; }

SplitTablePage::SplitTablePage(const QString &firstTitle,
                               const QStringList &firstHeaders,
                               const QString &secondTitle,
                               const QStringList &secondHeaders,
                               QWidget *parent)
    : QWidget(parent), sectionStack(new QStackedWidget(this)),
      firstPage(new TablePage(firstHeaders, this)),
      secondPage(new TablePage(secondHeaders, this)),
      firstTabButton(new QPushButton(firstTitle, this)),
      secondTabButton(new QPushButton(secondTitle, this)) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(16);

  auto *tabLayout = new QHBoxLayout();
  tabLayout->setContentsMargins(0, 0, 0, 0);
  tabLayout->setSpacing(10);

  for (auto *button : {firstTabButton, secondTabButton}) {
    button->setCheckable(true);
    button->setCursor(Qt::PointingHandCursor);
    button->setStyleSheet("QPushButton { background: #ffffff; color: #3730a3; "
                          "border: 2px solid #a5b4fc; "
                          "border-radius: 16px; padding: 8px 18px; font-size: "
                          "14px; font-weight: 600; }"
                          "QPushButton:hover { background: #eef2ff; }"
                          "QPushButton:checked { background: #4f46e5; color: "
                          "#ffffff; border-color: #4f46e5; }");
    tabLayout->addWidget(button);
  }
  tabLayout->addStretch();

  sectionStack->addWidget(firstPage);
  sectionStack->addWidget(secondPage);
  layout->addLayout(tabLayout);
  layout->addWidget(sectionStack, 1);

  connect(firstTabButton, &QPushButton::clicked, this,
          [this] { setCurrentSection(0); });
  connect(secondTabButton, &QPushButton::clicked, this,
          [this] { setCurrentSection(1); });
  setCurrentSection(0);
}

QTableWidget *SplitTablePage::firstTable() const { return firstPage->table(); }
QTableWidget *SplitTablePage::secondTable() const {
  return secondPage->table();
}
QPushButton *SplitTablePage::firstButton() const { return firstTabButton; }
QPushButton *SplitTablePage::secondButton() const { return secondTabButton; }

void SplitTablePage::setCurrentSection(int section) {
  sectionStack->setCurrentIndex(section);
  updateTabState(section);
}

void SplitTablePage::updateTabState(int section) {
  firstTabButton->setChecked(section == 0);
  secondTabButton->setChecked(section == 1);
}
