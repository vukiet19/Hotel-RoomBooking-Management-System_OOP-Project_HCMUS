#include "frontend/UX/UX.h"
#include "frontend/UI/UI.h"
#include "frontend/usercheck/backend.h"
#include "backend/Repository/InventoryRepository.h"
#include "backend/Manager/DatabaseManager.h"
#include <QPushButton>
#include <QStackedWidget>
#include <QDialog>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

// Section 5: Inventory Handler
void MainWindowController::handleLogin_5()
{
    stackedWidget->setCurrentIndex(InventoryIndex);
    inventoryPage->setSection(0);
    setActionBarVisible(true);
    setActiveButton(buttonInventory);
    
    Backend::loadTableData(tableInventory, "SELECT * FROM Inventory");

    btnAdd->setVisible(true);
    btnUpdate->setVisible(true);
    btnDelete->setVisible(true);
    btnFilter->setVisible(true);

    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();

    connect(btnAdd, &QPushButton::clicked, this, &MainWindowController::AddInventoryClick);
    connect(btnUpdate, &QPushButton::clicked, this, &MainWindowController::UpdateInventoryClick);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindowController::DeleteInventoryClick);
    connect(btnFilter, &QPushButton::clicked, this, &MainWindowController::FilterInventoryClick);
}

void MainWindowController::AddInventoryClick()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Add New Item to Inventory");
    dialog->setFixedSize(400, 300);
    dialog->setStyleSheet("QDialog { background-color: white; } QLabel { font-weight: bold; }");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    QFormLayout *form = new QFormLayout();

    QString inputStyle = "QLineEdit, QSpinBox { border: 1px solid #cbd5e1; border-radius: 4px; padding: 6px; }";

    QLineEdit *txtName = new QLineEdit(dialog);
    txtName->setStyleSheet(inputStyle);
    
    QSpinBox *spinQty = new QSpinBox(dialog);
    spinQty->setMaximum(10000);
    spinQty->setStyleSheet(inputStyle);
    
    QLineEdit *txtPrice = new QLineEdit(dialog);
    txtPrice->setPlaceholderText("Price (e.g. 15000)");
    txtPrice->setStyleSheet(inputStyle);

    form->addRow("Item Name:", txtName);
    form->addRow("Quantity:", spinQty);
    form->addRow("Price (VND):", txtPrice);
    layout->addLayout(form);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnSave = new QPushButton("Save", dialog);
    QPushButton *btnCancel = new QPushButton("Cancel", dialog);
    
    btnSave->setStyleSheet("background-color: #3b82f6; color: white; border-radius: 4px; padding: 8px; font-weight: bold;");
    btnCancel->setStyleSheet("background-color: #cbd5e1; color: #1e293b; border-radius: 4px; padding: 8px; font-weight: bold;");

    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnSave);
    layout->addLayout(btnLayout);

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);
    connect(btnSave, &QPushButton::clicked, [=]() {
        QString name = txtName->text().trimmed();
        int qty = spinQty->value();
        double price = txtPrice->text().toDouble();

        if (name.isEmpty() || price <= 0) {
            QMessageBox::warning(dialog, "Input Error", "Please input a valid Name and Price!");
            return;
        }

        InventoryRepository repo;
        if (repo.insertItem(name, qty, price)) { // Sử dụng hàm insertItem bạn đã viết sẵn
            QMessageBox::information(dialog, "Success", "Added new item successfully!");
            dialog->accept();
            handleLogin_5(); // Reload table
        } else {
            QMessageBox::critical(dialog, "Error", "Failed to add item to database!");
        }
    });

    dialog->exec();
    dialog->deleteLater();
}

void MainWindowController::UpdateInventoryClick()
{
    int currentRow = tableInventory->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Select Item", "Please select an item to update!");
        return;
    }

    QString id = tableInventory->item(currentRow, 0)->text();
    QString currentName = tableInventory->item(currentRow, 1)->text();
    int currentQty = tableInventory->item(currentRow, 3)->text().toInt();

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Update Inventory Quantity");
    dialog->setFixedSize(350, 200);

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    QFormLayout *form = new QFormLayout();

    QLabel *lblName = new QLabel(currentName, dialog);
    QSpinBox *spinQty = new QSpinBox(dialog);
    spinQty->setMaximum(10000);
    spinQty->setValue(currentQty);

    form->addRow("Item Name:", lblName);
    form->addRow("New Quantity:", spinQty);
    layout->addLayout(form);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnSave = new QPushButton("Update", dialog);
    QPushButton *btnCancel = new QPushButton("Cancel", dialog);
    
    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnSave);
    layout->addLayout(btnLayout);

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);
    connect(btnSave, &QPushButton::clicked, [=]() {
        InventoryRepository repo;
        if (repo.updateQuantity(id.toInt(), spinQty->value())) { // Gọi hàm updateQuantity có sẵn
            QMessageBox::information(dialog, "Success", "Item quantity updated successfully!");
            dialog->accept();
            handleLogin_5(); // Reload table
        } else {
            QMessageBox::critical(dialog, "Error", "Failed to update quantity!");
        }
    });

    dialog->exec();
    dialog->deleteLater();
}

void MainWindowController::DeleteInventoryClick()
{
    int currentRow = tableInventory->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Select Item", "Please select an item to delete!");
        return;
    }

    QString id = tableInventory->item(currentRow, 0)->text();
    QString name = tableInventory->item(currentRow, 1)->text();

    if (QMessageBox::question(this, "Confirm Deletion", 
        QString("Are you sure you want to delete '%1' from Inventory?").arg(name)) == QMessageBox::Yes) 
    {
        QSqlQuery query(DatabaseManager::instance().database());
        query.prepare("DELETE FROM Inventory WHERE item_id = :id");
        query.bindValue(":id", id);

        if (query.exec()) {
            QMessageBox::information(this, "Success", "Item deleted successfully!");
            handleLogin_5(); // Reload table
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete item: " + query.lastError().text());
        }
    }
}

void MainWindowController::FilterInventoryClick()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Filter Inventory");
    dialog->setFixedSize(350, 200);

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    QFormLayout *form = new QFormLayout();

    QLineEdit *txtName = new QLineEdit(dialog);
    txtName->setPlaceholderText("Item name contains...");
    
    QSpinBox *spinMinQty = new QSpinBox(dialog);
    spinMinQty->setMaximum(10000);
    
    form->addRow("Name:", txtName);
    form->addRow("Min Quantity:", spinMinQty);
    layout->addLayout(form);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnApply = new QPushButton("Apply", dialog);
    QPushButton *btnReset = new QPushButton("Reset", dialog);
    
    btnLayout->addWidget(btnReset);
    btnLayout->addWidget(btnApply);
    layout->addLayout(btnLayout);

    connect(btnReset, &QPushButton::clicked, [=]() {
        Backend::loadTableData(tableInventory, "SELECT * FROM Inventory");
        dialog->accept();
    });

    connect(btnApply, &QPushButton::clicked, [=]() {
        QString queryStr = "SELECT * FROM Inventory WHERE quantity >= " + QString::number(spinMinQty->value());
        if (!txtName->text().isEmpty()) {
            queryStr += " AND item_name LIKE '%" + txtName->text().trimmed() + "%'";
        }
        Backend::loadTableData(tableInventory, queryStr);
        dialog->accept();
    });

    dialog->exec();
    dialog->deleteLater();
}
