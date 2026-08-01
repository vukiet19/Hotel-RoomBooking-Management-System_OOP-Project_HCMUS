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

// Section 6: Inventory Log Handler
void MainWindowController::showInventoryLogTab()
{
    stackedWidget->setCurrentIndex(InventoryIndex);
    inventoryPage->setSection(1);
    setActionBarVisible(true);
    setActiveButton(buttonInventory);
    
    Backend::loadTableData(tableInventoryLog, "SELECT * FROM InventoryLog ORDER BY date DESC");
    
    btnAdd->setVisible(false);
    btnUpdate->setVisible(false);
    btnDelete->setVisible(false);
    btnFilter->setVisible(true);
    
    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();

    // KẾT NỐI SỰ KIỆN NÚT FILTER
    connect(btnFilter, &QPushButton::clicked, this, &MainWindowController::FilterInventoryLogClick);
}

void MainWindowController::FilterInventoryLogClick()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Filter Inventory Logs");
    dialog->setFixedSize(380, 200);

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    QFormLayout *form = new QFormLayout();

    QLineEdit *txtItemId = new QLineEdit(dialog);
    txtItemId->setPlaceholderText("Leave blank for all");

    QComboBox *cbAction = new QComboBox(dialog);
    cbAction->addItems({"All", "RESERVE", "RELEASE"}); // Dựa theo logic InventoryService của bạn

    form->addRow("Item ID:", txtItemId);
    form->addRow("Action Type:", cbAction);
    layout->addLayout(form);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnApply = new QPushButton("Apply", dialog);
    QPushButton *btnReset = new QPushButton("Reset", dialog);
    
    btnLayout->addWidget(btnReset);
    btnLayout->addWidget(btnApply);
    layout->addLayout(btnLayout);

    connect(btnReset, &QPushButton::clicked, [=]() {
        Backend::loadTableData(tableInventoryLog, "SELECT * FROM InventoryLog ORDER BY date DESC");
        dialog->accept();
    });

    connect(btnApply, &QPushButton::clicked, [=]() {
        QString queryStr = "SELECT * FROM InventoryLog WHERE 1=1";
        
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