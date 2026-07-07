// Hàm này dùng để lưu các hàm chạy front end
// Hàm này liên kết với frontend.cpp
#pragma once

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <string>

class MainWindow : public QWidget
{
public:
    MainWindow(QWidget *parent = nullptr);

private:
    // Nút 10 lựa chọn
    QPushButton *button1;
    QPushButton *button2;
    QPushButton *button3;
    QPushButton *button4;
    QPushButton *button5;
    QPushButton *button6;
    QPushButton *button7;
    QPushButton *button8;
    QPushButton *button9;
    QPushButton *button10;

    // Đây là hàm khi ấn nút tương ứng thì sẽ thực thi theo hàm đó( trong đây là gọi hàm loadTableData để hiện table và setActiveButton để ra hiệu ứng nút)
    void handleLogin_1();
    void handleLogin_2();
    void handleLogin_3();
    void handleLogin_4();
    void handleLogin_5();
    void handleLogin_6();
    void handleLogin_7();
    void handleLogin_8();
    void handleLogin_9();
    void handleLogin_10();

    // load data từ table tương ứng
    void loadTableData(QTableWidget *table, QString queryStr);
    // Nếu ấn nút đó thì sẽ ra biểu tượng để làm nổi bật
    void setActiveButton(QPushButton *clickedButton);
    // Hàm này dùng để update khi cập nhật database
    void update();

    // Đây là một cái stack lưu widget, khi lưu thì phần tử đầu sẽ là phần tử đầu tiên đc đi vào( giống queue)
    QStackedWidget *stackedWidget;

    // Định dạng table qt
    QTableWidget *tableBooking;
    QTableWidget *tableBookingItems;
    QTableWidget *tableCustomer;
    QTableWidget *tableFood;
    QTableWidget *tableInventory;
    QTableWidget *tableInventoryLog;
    QTableWidget *tableRoom;
    QTableWidget *tableRoomType;
    QTableWidget *tableService;
    QTableWidget *tableBill;
};

class LoginWindow : public QWidget
{
public:
    // Hàm khởi tạo
    LoginWindow(QWidget *parent = nullptr);
    // Hàm destructor
    ~LoginWindow();
    void handleLogin();

private:
    // Hàm này để kiểm tra valid hay ko
    bool check_valid_username(const std::string &g);
    bool check_valid_password(const std::string &g, const std::string &p);

    // window để đổi sang trang mainwindow
    MainWindow *window2;

    // QLabel sẽ là kiểu dạng text , sẽ fhi lên UI
    QLabel *label1;
    QLabel *label2;
    QLabel *label3;
    // Giống như cái line mà edit được nghĩa là cái này được dùng để input (Trong class log thì sẽ lưu những gì đc lưu từ ngừoi dùng)
    QLineEdit *inputBox_user;
    QLineEdit *inputBox_pass;

    // Đây cũng là button như trên
    QPushButton *button;

    QLabel *imageLabel;
    // Hàm này để ném error nếu nhập sai
    QLabel *l1;
};
