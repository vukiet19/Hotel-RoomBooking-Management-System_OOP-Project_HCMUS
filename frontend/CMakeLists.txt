cmake_minimum_required(VERSION 3.16)
project(HotelManagement VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tell CMake to automatically handle Qt's special macros
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 1. Find the Qt6 libraries (including Sql)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

# 2. CREATE THE APP FIRST
add_executable(HotelManagement
    main.cpp
)

# 3. LINK THE LIBRARIES SECOND (This must always come after add_executable)
target_link_libraries(HotelManagement PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)