#pragma once

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
class MainWindowController;

class LoginWindow : public QWidget
{

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private:
    MainWindowController *window2;

    QLabel *label1;
    QLabel *l1;
    QLabel *imageLabel;
    QLineEdit *inputBox_user;
    QLineEdit *inputBox_pass;
    QPushButton *button;
    void handleLogin();
};