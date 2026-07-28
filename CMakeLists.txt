cmake_minimum_required(VERSION 3.16)

project(Hotel_Management LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql Widgets)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "backend/*.cpp"
    "cores/*.cpp"
    "frontend/*.cpp"
    "main.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Sql Qt6::Widgets)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/backend/Database"
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
