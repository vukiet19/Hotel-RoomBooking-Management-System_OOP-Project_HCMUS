#pragma once
#include "Room.h"
using namespace std;
// Note_can_chu_y , Tham khao https://viblo.asia/p/virtual-function-ham-ao-override-va-final-naQZRWRPlvx

// Tóm tắt lại override: hàm này có chức năng chính là sẽ ghi đè lên hàm cha , ví dụ nếu hàm cha và con nếu cùng có hàm như getBasePrice thì hàm con này sẽ ghi đè lên hàm cha , kết quả sẽ in theo kết quả hàm con

// Stadard room
class StandardRoom : public Room
{
public:
    StandardRoom();

    StandardRoom(string roomNum);

    RoomType getType() const override;

    int getBasePrice() const override;
};

class VipRoom : public Room
{
public:
    VipRoom();
    VipRoom(string roomNum);

    RoomType getType() const override;

    int getBasePrice() const override;
};

class PresiRoom : public Room
{
public:
    PresiRoom();

    PresiRoom(string roomNum);

    RoomType getType() const override;

    int getBasePrice() const override;
};