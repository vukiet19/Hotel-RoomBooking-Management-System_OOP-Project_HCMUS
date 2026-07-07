cmake_minimum_required(VERSION 3.16)

project(Hotel_Management)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 1. Updated to your macOS Homebrew Qt path
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt")

# 2. Added 'Widgets' to fix the QApplication error
find_package(Qt6 COMPONENTS Core Sql Widgets REQUIRED)

file(GLOB_RECURSE SOURCES 
    main.cpp
    frontend.cpp
    frontend.h
    DatabaseManager.cpp   
    DatabaseManager.h
)

list(FILTER SOURCES EXCLUDE REGEX "^build/")

add_executable(${PROJECT_NAME} ${SOURCES})

# 3. Linked Qt6::Widgets to your executable
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Sql Qt6::Widgets)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Database 
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})