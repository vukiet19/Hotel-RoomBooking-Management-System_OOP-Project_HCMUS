cmake_minimum_required(VERSION 3.16)

project(QtTest VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH "/Users/son/Qt/6.11.1/macos")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(QtTest main.cpp)
target_link_libraries(QtTest PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

add_executable(Booking
 Booking/Booking.h
 Booking/Booking.cpp)
target_link_libraries(Booking PRIVATE Qt6::Core)

find_package(Qt6 COMPONENTS Sql REQUIRED)
target_link_libraries(QtTest PRIVATE Qt6::Sql)