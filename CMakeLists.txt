cmake_minimum_required(VERSION 3.16)

project(Hotel_Management)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/msvc2022_64")

find_package(Qt6 COMPONENTS Core Sql REQUIRED)

file(GLOB_RECURSE SOURCES 
    "Customer/*.cpp"
    "Booking/*.cpp"
    "Room/*.cpp"
    "Manager/*.cpp"
    "Service/*.cpp"
    "Repository/*.cpp"
    "Observer.cpp"
    "main.cpp"
)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Database 
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

list(FILTER SOURCES EXCLUDE REGEX "^build/")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Sql)