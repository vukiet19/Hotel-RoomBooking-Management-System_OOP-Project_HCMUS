#include "frontend/UX/UX.h"
#include "frontend/UI/UI.h"
#include "frontend/usercheck/backend.h"
#include <QPushButton>
#include <QStackedWidget>
#include <QDialog>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QLabel>

// Section 6: Inventory Log Handler
void MainWindowController::showInventoryLogTab()
{
    stackedWidget->setCurrentIndex(InventoryIndex);
    inventoryPage->setSection(1);
    setActionBarVisible(true);
    setActiveButton(buttonInventory);
    
    QString logQuery = "SELECT log_id AS 'Log ID', item_id AS 'Item ID', "
                       "CASE WHEN quantity > 0 THEN '+' || quantity ELSE quantity END AS 'Quantity', "
                       "action_type AS 'Action Type', date AS 'Date' "
                       "FROM InventoryLog ORDER BY date DESC";
    Backend::loadTableData(tableInventoryLog, logQuery);
    
    btnAdd->setVisible(false);
    btnUpdate->setVisible(false);
    btnDelete->setVisible(false);
    if (btnAddToBooking) {
        btnAddToBooking->setVisible(false); 
    }
    btnFilter->setVisible(true);
    
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();
    if (btnAddToBooking) btnAddToBooking->disconnect();

    connect(btnFilter, &QPushButton::clicked, this, &MainWindowController::FilterInventoryLogClick);
}

void MainWindowController::FilterInventoryLogClick()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Filter Inventory Logs");
    dialog->setFixedSize(380, 260);
    dialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
        "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; background: transparent; }");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(30, 20, 30, 20);

    QLabel *titleLabel = new QLabel("Filter Inventory Logs", dialog);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #3730a3; margin-bottom: 10px;");
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout *form = new QFormLayout();
    form->setSpacing(12);

    QString inputStyle = "QLineEdit, QComboBox {"
                         "   background-color: #ffffff; border: 2px solid #38bdf8; "
                         "   border-radius: 8px; padding: 6px; font-size: 14px; color: #0f172a; min-height: 25px; }";

    QLineEdit *txtItemId = new QLineEdit(dialog);
    txtItemId->setPlaceholderText("Leave blank for all");
    txtItemId->setStyleSheet(inputStyle);

    QComboBox *cbAction = new QComboBox(dialog);
    cbAction->addItems({"All", "RESERVE", "RELEASE"});
    cbAction->setStyleSheet(inputStyle);

    form->addRow("Item ID:", txtItemId);
    form->addRow("Action Type:", cbAction);
    layout->addLayout(form);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnApply = new QPushButton("Apply", dialog);
    QPushButton *btnReset = new QPushButton("Reset", dialog);
    
    btnApply->setStyleSheet("QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #10b981, stop:1 #059669); color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold; }");
    btnReset->setStyleSheet("background-color: #cbd5e1; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

    btnApply->setCursor(Qt::PointingHandCursor);
    btnReset->setCursor(Qt::PointingHandCursor);

    btnLayout->addWidget(btnReset);
    btnLayout->addWidget(btnApply);
    layout->addLayout(btnLayout);

    QString baseSelect = "SELECT log_id AS 'Log ID', item_id AS 'Item ID', "
                         "quantity AS 'Quantity', action_type AS 'Action Type', date AS 'Date' "
                         "FROM InventoryLog";

    connect(btnReset, &QPushButton::clicked, [=]() {
        Backend::loadTableData(tableInventoryLog, baseSelect + " ORDER BY date DESC");
        dialog->accept();
    });

    connect(btnApply, &QPushButton::clicked, [=]() {
        QString queryStr = baseSelect + " WHERE 1=1";
        
        if (!txtItemId->text().isEmpty()) {
            queryStr += " AND item_id = " + txtItemId->text().trimmed();
        }
        
        if (cbAction->currentText() != "All") {
            queryStr += " AND action_type = '" + cbAction->currentText() + "'";
        }
        
        queryStr += " ORDER BY date DESC";
        
        Backend::loadTableData(tableInventoryLog, queryStr);
        dialog->accept();
    });

    dialog->exec();
    dialog->deleteLater();
}