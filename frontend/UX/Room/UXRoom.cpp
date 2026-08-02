#include "backend/Repository/RoomRepository.h"
#include "cores/Room/DerivedRooms.h"
#include "cores/Room/RoomFactory.h"
#include "frontend/UI/UI.h"
#include "frontend/UX/UX.h"
#include "frontend/usercheck/backend.h"
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

// Helper khởi tạo đúng con trỏ polymorphic Room theo loại phòng chọn trên UI
static Room* createRoomInstance(const QString& typeStr) {
    if (typeStr == "VIP") {
        return new VipRoom();
    }
    else if (typeStr == "President" || typeStr == "PresidentialSuite") {
        return new PresiRoom();
    }
    return new StandardRoom();
}

// Section 7: List Rooms Handler
void MainWindowController::showRoomTab() {
    stackedWidget->setCurrentIndex(RoomIndex);
    roomPage->setSection(0);
    setActionBarVisible(true);
    setActiveButton(buttonRoom);
    QString queryStr = R"(
        SELECT 
            room_id AS "Room ID",
            room_number AS "Room Number",
            room_type AS "Type",
            status AS "Status",
            base_price AS "Price",
            number_people AS "Number People"
        FROM ListRooms
    )";

    Backend::loadTableData(tableRoom, queryStr);

    btnAdd->setVisible(true);
    btnUpdate->setVisible(true);
    btnDelete->setVisible(true);
    btnFilter->setVisible(true);

    btnAdd->disconnect();
    btnUpdate->disconnect();
    btnDelete->disconnect();
    btnFilter->disconnect();

    connect(btnAdd, &QPushButton::clicked, this, &MainWindowController::showAddRoomDialog);
    connect(btnUpdate, &QPushButton::clicked, this, &MainWindowController::showUpdateRoomDialog);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindowController::showDeleteRoomDialog);

    connect(btnFilter, &QPushButton::clicked, this, [=]() {
        QString roomQuery = R"(
            SELECT 
                room_id AS "Room ID",
                room_number AS "Room Number",
                room_type AS "Type",
                status AS "Status",
                base_price AS "Price",
                number_people AS "Number People"
            FROM ListRooms
            ORDER BY room_type, room_number
        )";
        Backend::loadTableData(tableRoom, roomQuery);
    });
}

void MainWindowController::showAddRoomDialog() {
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Add room");
    dialog->setFixedSize(420, 500);

    dialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
        "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }"
        "QMessageBox { background-color: white; }"
        "QMessageBox QLabel { color: #333333; font-size: 13px; font-weight: normal; }"
        "QMessageBox QPushButton { background-color: #cbd5e1; color: #1e293b; border: none; border-radius: 4px; padding: 6px 20px; font-weight: bold; }");

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(30, 30, 30, 30);

    QLabel* titleLabel = new QLabel("Room Details", dialog);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #3730a3; margin-bottom: 15px;");
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout* form = new QFormLayout();
    form->setSpacing(15);

    QString inputStyle =
        "QLineEdit, QComboBox {"
        "   background-color: #ffffff; "
        "   border: 2px solid #38bdf8; "
        "   border-radius: 8px; "
        "   padding: 10px; "
        "   font-size: 14px; "
        "   color: #0f172a; "
        "}"
        "QLineEdit:hover, QComboBox:hover { border: 2px solid #0284c7; }"
        "QLineEdit:focus, QComboBox:focus { border: 2px solid #0369a1; background-color: #f0f9ff; }";

    QLineEdit* txtId = new QLineEdit(dialog);
    txtId->setPlaceholderText("EX: R101, R102...");
    txtId->setStyleSheet(inputStyle);

    QLineEdit* txtNumber = new QLineEdit(dialog);
    txtNumber->setPlaceholderText("EX: 101, 102...");
    txtNumber->setStyleSheet(inputStyle);

    QComboBox* cbType = new QComboBox(dialog);
    cbType->addItems({ "Standard", "VIP", "President" });
    cbType->setStyleSheet(inputStyle);

    QComboBox* cbStatus = new QComboBox(dialog);
    cbStatus->addItems({ "Available", "Reserved", "Occupied", "Maintenance" });
    cbStatus->setStyleSheet(inputStyle);

    QLineEdit *txtPrice = new QLineEdit(dialog);
    txtPrice->setPlaceholderText("EX: 500000 VND, 1000000 VND...");
    txtPrice->setStyleSheet(inputStyle);

    QLineEdit* txtPeople = new QLineEdit(dialog);
    txtPeople->setPlaceholderText("EX: 2, 4...");
    txtPeople->setStyleSheet(inputStyle);

    form->addRow("Room ID:", txtId);
    form->addRow("Room number:", txtNumber);
    form->addRow("Type Room:", cbType);
    form->addRow("Status:", cbStatus);
    form->addRow("Base price:", txtPrice);
    form->addRow("Capacity:", txtPeople);

    layout->addLayout(form);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton* btnSave = new QPushButton("Save", dialog);
    QPushButton* btnCancel = new QPushButton("Cancel", dialog);

    btnSave->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366f1, stop:1 #8b5cf6); color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4f46e5, stop:1 #7c3aed); }");
    btnCancel->setStyleSheet("background-color: #cbd5e1; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

    btnSave->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);
    layout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);

    connect(btnSave, &QPushButton::clicked, [=]() {
        if (txtNumber->text().isEmpty() || txtPrice->text().isEmpty() || txtPeople->text().isEmpty()) {
            QMessageBox::warning(dialog, "Error", "Please fill up information");
            return;
        }

        Room* newRoom = createRoomInstance(cbType->currentText());

        if (!txtId->text().trimmed().isEmpty()) {
            newRoom->setId(txtId->text().trimmed().toStdString());
        } else {
            newRoom->setId("R" + txtNumber->text().trimmed().toStdString());
        }

        newRoom->setRoomNumber(txtNumber->text().toStdString());

        QString statusText = cbStatus->currentText();
        RoomStatus statusEnum = RoomStatus::Available;
        if (statusText == "Reserved") statusEnum = RoomStatus::Reserved;
        else if (statusText == "Occupied") statusEnum = RoomStatus::Occupied;
        else if (statusText == "Maintenance") statusEnum = RoomStatus::Maintenance;

        newRoom->setStatus(statusEnum);
        newRoom->setBasePrice(txtPrice->text().toDouble());
        newRoom->setNumberPeople(txtPeople->text().toInt());

        RoomRepository repo;
        bool success = repo.add(newRoom);

        delete newRoom;

        if (success) {
            QMessageBox::information(dialog, "Success", "Add room successfully!");
            dialog->accept();
            showRoomTab();
        }
        else {
            QMessageBox::critical(dialog, "Error", "Can not save in database");
        }
        });

    dialog->exec();
    dialog->deleteLater();
}

void MainWindowController::showUpdateRoomDialog() {
    int currentRow = tableRoom->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Select Room", "Please select a room from the table to update!");
        return;
    }

    QString currentId = tableRoom->item(currentRow, 0)->text();
    QString currentNumber = tableRoom->item(currentRow, 1)->text();
    QString currentType = tableRoom->item(currentRow, 2)->text();
    QString currentStatus = tableRoom->item(currentRow, 3)->text();
    QString currentPrice = tableRoom->item(currentRow, 4)->text();
    QString currentPeople = tableRoom->item(currentRow, 5)->text();

    QDialog* updateDialog = new QDialog(this);
    updateDialog->setWindowTitle("Update Room Details");
    updateDialog->setFixedSize(420, 550);

    updateDialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f0f9ff, stop:1 #ffffff); }"
        "QLabel { color: #1e293b; font-weight: bold; font-size: 14px; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(updateDialog);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel* titleLabel = new QLabel("Edit Room Attributes", updateDialog);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #3730a3; margin-bottom: 15px;");
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QFormLayout* form = new QFormLayout();
    form->setSpacing(15);

    QString inputStyle =
        "QLineEdit, QComboBox {"
        "   background-color: #ffffff; "
        "   border: 2px solid #38bdf8; "
        "   border-radius: 8px; "
        "   padding: 10px; "
        "   font-size: 14px; "
        "   color: #0f172a; "
        "}"
        "QLineEdit:hover, QComboBox:hover { border: 2px solid #0284c7; }"
        "QLineEdit:focus, QComboBox:focus { border: 2px solid #0369a1; background-color: #f0f9ff; }";

    QLineEdit* txtId = new QLineEdit(currentId, updateDialog);
    txtId->setReadOnly(true);
    txtId->setStyleSheet(inputStyle + "QLineEdit { background-color: #e2e8f0; }");

    QLineEdit* txtNumber = new QLineEdit(currentNumber, updateDialog);
    txtNumber->setStyleSheet(inputStyle);

    QComboBox* cbType = new QComboBox(updateDialog);
    cbType->addItems({ "Standard", "VIP", "President" });
    cbType->setCurrentText(currentType);
    cbType->setStyleSheet(inputStyle);

    QComboBox* cbStatus = new QComboBox(updateDialog);
    cbStatus->addItems({ "Available", "Reserved", "Occupied", "Maintenance" });
    cbStatus->setCurrentText(currentStatus);
    cbStatus->setStyleSheet(inputStyle);

    QLineEdit* txtPrice = new QLineEdit(currentPrice, updateDialog);
    txtPrice->setStyleSheet(inputStyle);

    QLineEdit* txtPeople = new QLineEdit(currentPeople, updateDialog);
    txtPeople->setStyleSheet(inputStyle);

    form->addRow("Room ID:", txtId);
    form->addRow("Room Number:", txtNumber);
    form->addRow("Type Room:", cbType);
    form->addRow("Status:", cbStatus);
    form->addRow("Base Price (VND):", txtPrice);
    form->addRow("Capacity:", txtPeople);

    mainLayout->addLayout(form);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton* btnSave = new QPushButton("Update", updateDialog);
    QPushButton* btnCancel = new QPushButton("Cancel", updateDialog);

    btnSave->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6366f1, stop:1 #8b5cf6); color: white; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");
    btnCancel->setStyleSheet(
        "background-color: #cbd5e1; color: #475569; border: none; border-radius: 8px; padding: 10px 0; font-size: 15px; font-weight: bold;");

    buttonLayout->addWidget(btnCancel);
    buttonLayout->addWidget(btnSave);
    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, updateDialog, &QDialog::reject);

    connect(btnSave, &QPushButton::clicked, [=]() {
        Room* roomObj = createRoomInstance(cbType->currentText());
        roomObj->setId(currentId.toStdString());
        roomObj->setRoomNumber(txtNumber->text().toStdString());

        QString statusText = cbStatus->currentText();
        RoomStatus statusEnum = RoomStatus::Available;
        if (statusText == "Reserved") statusEnum = RoomStatus::Reserved;
        else if (statusText == "Occupied") statusEnum = RoomStatus::Occupied;
        else if (statusText == "Maintenance") statusEnum = RoomStatus::Maintenance;

        roomObj->setStatus(statusEnum);
        roomObj->setBasePrice(txtPrice->text().toDouble());
        roomObj->setNumberPeople(txtPeople->text().toInt());

        RoomRepository repo;
        bool success = repo.update(roomObj);

        delete roomObj; // Thu hồi bộ nhớ dynamic

        if (success) {
            QMessageBox::information(updateDialog, "Success", "Room updated successfully!");
            updateDialog->accept();
            showRoomTab();
        }
        else {
            QMessageBox::critical(updateDialog, "Error", "Failed to update room in database.");
        }
        });

    updateDialog->exec();
    updateDialog->deleteLater();
}

void MainWindowController::showDeleteRoomDialog() {
    int currentRow = tableRoom->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Select Room", "Please select a room row to delete!");
        return;
    }

    QString roomId = tableRoom->item(currentRow, 0)->text();
    QString roomNum = tableRoom->item(currentRow, 1)->text();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Room Deletion",
        QString("Are you sure you want to delete Room %1 (ID: %2)?").arg(roomNum, roomId),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        RoomRepository repo;
        // Thực thi xóa theo room_id hoặc room_number tùy theo thiết kế của Repo
        if (repo.remove(roomId.toStdString()) || repo.remove(roomNum.toStdString())) {
            QMessageBox::information(this, "Deleted", "Room deleted successfully!");
            showRoomTab();
        }
        else {
            QMessageBox::critical(this, "Error", "Failed to delete room from database.");
        }
    }
}