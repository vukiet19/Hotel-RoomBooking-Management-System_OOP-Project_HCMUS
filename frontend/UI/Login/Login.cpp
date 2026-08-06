#include "Login.h"
#include "../CustomerWindow/customerwin.h"
#include "frontend/UX/UX.h"
#include <QMessageBox>
#include <QPixmap>
#include <QVBoxLayout>
using namespace std;

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent) {
  setAttribute(Qt::WA_DeleteOnClose, true);
  setAttribute(Qt::WA_StyledBackground, true);
  setFixedSize(1200, 800);
  setWindowTitle("Hotel_Management");
  setObjectName("MyMainWindow");

  // --- LIGHT, FRESH, & AIRY GRADIENT BACKGROUND ---
  setStyleSheet("#MyMainWindow {"
                "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 "
                "#f0f9ff, stop:0.5 #e0f2fe, stop:1 #ffffff);"
                "}");

  label1 = new QLabel("LOGIN", this);
  QLabel *subLabel = new QLabel("Please log in to manage your hotel", this);

  inputBox_user = new QLineEdit(this);
  inputBox_pass = new QLineEdit(this);
  inputBox_pass->setEchoMode(QLineEdit::Password);

  button = new QPushButton("Log In", this);

  imageLabel = new QLabel(this);
  l1 = new QLabel("© 2026 Premium Hotel Management System", this);

  QPixmap myImage("assets/US.png");
  imageLabel->setPixmap(myImage);
  imageLabel->setScaledContents(true);
  imageLabel->setFixedSize(120, 120);

  label1->setFixedHeight(60);
  label1->setAlignment(Qt::AlignCenter);
  subLabel->setFixedHeight(30);
  subLabel->setAlignment(Qt::AlignCenter);
  inputBox_user->setFixedSize(500, 55);
  inputBox_pass->setFixedSize(500, 55);
  button->setFixedSize(500, 55);
  l1->setFixedHeight(40);
  l1->setAlignment(Qt::AlignCenter);

  auto *rootLayout = new QVBoxLayout(this);
  rootLayout->setContentsMargins(80, 60, 80, 30);
  rootLayout->setSpacing(0);
  rootLayout->addStretch(2);
  rootLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);
  rootLayout->addSpacing(24);
  rootLayout->addWidget(label1);
  rootLayout->addWidget(subLabel);
  rootLayout->addSpacing(42);
  rootLayout->addWidget(inputBox_user, 0, Qt::AlignHCenter);
  rootLayout->addSpacing(18);
  rootLayout->addWidget(inputBox_pass, 0, Qt::AlignHCenter);
  rootLayout->addSpacing(30);
  rootLayout->addWidget(button, 0, Qt::AlignHCenter);
  rootLayout->addStretch(3);
  rootLayout->addWidget(l1);

  // --- TYPOGRAPHY (Dark slate colors for the light background) ---
  label1->setStyleSheet("font-size: 42px; font-weight: bold; color: #0f172a; "
                        "font-family: 'Segoe UI', Arial;");
  subLabel->setStyleSheet(
      "font-size: 18px; color: #475569; font-family: 'Segoe UI', Arial;");
  l1->setStyleSheet("font-size: 13px; color: #94a3b8;");

  // --- COLORFUL & VIBRANT INPUT STYLES ---
  // --- STYLE CHO Ô NHẬP LIỆU (Hợp với nền xanh sáng) ---
  QString inputStyle =
      "QLineEdit {"
      "   background-color: #ffffff; " // Nền trắng tinh
      "   border: 3px solid #38bdf8; " // KHUNG VIỀN 3px màu xanh dương sáng
                                       // (rất nổi bật)
      "   border-radius: 22px; "       // Bo góc khung viền
      "   padding: 10px 20px; "
      "   font-size: 16px; "
      "   color: #0f172a; " // Chữ màu đậm
      "}"
      "QLineEdit:hover {"
      "   border: 3px solid #0284c7; " // Khung viền đậm hơn khi rà chuột vào
      "}"
      "QLineEdit:focus {"
      "   border: 3px solid #0369a1; " // Khung viền xanh ngọc biển sẫm khi
                                       // click vào gõ
      "   background-color: #f0f9ff; " // Nền đổi màu xanh lơ nhạt xíu khi đang
                                       // gõ
      "}";

  // Áp dụng style cho 2 ô nhập liệu
  inputBox_user->setStyleSheet(inputStyle);
  inputBox_pass->setStyleSheet(inputStyle);

  // Inputs get a stronger, brighter border and background when clicked
  inputBox_user->setStyleSheet(inputStyle +
                               "QLineEdit:focus { border: 2px solid #6366f1; "
                               "background-color: #e0e7ff; }");
  inputBox_pass->setStyleSheet(inputStyle +
                               "QLineEdit:focus { border: 2px solid #6366f1; "
                               "background-color: #e0e7ff; }");

  inputBox_user->setPlaceholderText("Username");
  inputBox_pass->setPlaceholderText("Password");

  // --- COLORFUL VIBRANT BUTTON (Indigo to Purple Gradient) ---
  button->setStyleSheet("QPushButton {"
                        "   background: qlineargradient(x1:0, y1:0, x2:1, "
                        "y2:0, stop:0 #6366f1, stop:1 #8b5cf6); "
                        "   color: white; "
                        "   border: none; "
                        "   border-radius: 27px; "
                        "   font-size: 18px; "
                        "   font-weight: bold;"
                        "}"
                        "QPushButton:hover {"
                        "   background: qlineargradient(x1:0, y1:0, x2:1, "
                        "y2:0, stop:0 #4f46e5, stop:1 #7c3aed); "
                        "}"
                        "QPushButton:pressed {"
                        "   background: #4338ca; "
                        "}");

  button->setCursor(Qt::PointingHandCursor);

  connect(button, &QPushButton::clicked, this, &LoginWindow::handleLogin);
}

LoginWindow::~LoginWindow() {}

void LoginWindow::handleLogin() {
  QString user = inputBox_user->text();
  QString pass = inputBox_pass->text();

  if (user == "admin" && pass == "admin123") {
    MainWindowController *mainWindow = new MainWindowController();
    mainWindow->show();
    mainWindow->handleDashboardTab();
    this->close();
  } else if (user == "customer" && pass == "123456") {
    CustomerInputWindow *customerWindow = new CustomerInputWindow();
    customerWindow->show();

    this->close();
  } else {
    QMessageBox::warning(this, "Error", "Incorrect usename or password!");
  }
}
