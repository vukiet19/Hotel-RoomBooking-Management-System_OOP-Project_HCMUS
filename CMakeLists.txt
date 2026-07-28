cmake_minimum_required(VERSION 3.16)

project(Hotel_Management)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 1. TỰ ĐỘNG TÌM QT TRÊN CẢ MAC VÀ WINDOWS
if(APPLE AND NOT CMAKE_PREFIX_PATH)
    if(EXISTS "/opt/homebrew/opt/qt")
        set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt") # Mac Apple Silicon
    elseif(EXISTS "/usr/local/opt/qt")
        set(CMAKE_PREFIX_PATH "/usr/local/opt/qt")   # Mac Intel
    endif()
endif()

# 2. TÌM CÁC THƯ VIỆN QT6
find_package(Qt6 COMPONENTS Core Sql Widgets REQUIRED)

# 3. GOM TẤT CẢ FILE SOURCES (Dùng GLOB kết hợp ép nhận diện các file Repository cốt lõi)
file(GLOB_RECURSE SOURCES 
    "Customer/*.cpp"
    "Room/*.cpp"
    "Manager/*.cpp"
    "Mainwindow/Control/*.cpp"
    "Repository/*.cpp"
    "Repository/*.h"
    "Observer.cpp"
    "Mainwindow/UI/*.cpp"
    "Login/*.cpp"
    "CustomerWindow/*.cpp"
    "Checkout/*.cpp"
    "backend/*.cpp"
    "main.cpp"
)

# Loại bỏ thư mục build nếu nằm trong cây thư mục
list(FILTER SOURCES EXCLUDE REGEX "build/")

# 4. KHAI BÁO EXECUTABLE
if(APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES})
else()
    add_executable(${PROJECT_NAME} ${SOURCES})
endif()

# 5. LINK THƯ VIỆN QT
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Sql Qt6::Widgets)

# 6. COPY DATABASE VÀO THƯ MỤC BUILD
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Database" 
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

# 7. INCLUDE CÁC DIRECTORIES
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})