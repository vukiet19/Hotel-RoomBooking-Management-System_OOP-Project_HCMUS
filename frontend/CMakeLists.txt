cmake_minimum_required(VERSION 3.16)

project(Hotel_Management LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql Widgets)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "Checkout/*.cpp"
    "Customer/*.cpp"
    "Room/*.cpp"
    "Manager/DatabaseManager.cpp"
    "Repository/*.cpp"
    "Observer.cpp"
    "main.cpp"
    "frontend.cpp"
    "backend.cpp"
    "Customerwin.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Core Qt6::Sql Qt6::Widgets
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Database"

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/Database/hotel.db"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Database/hotel.db"

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/NTH.png"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/NTH.png"
)

if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt windeployqt.exe
        HINTS "${Qt6_DIR}/../../../bin"
    )

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --release
                --compiler-runtime
                --no-translations
                "$<TARGET_FILE:${PROJECT_NAME}>"
        )
    endif()
endif()

add_custom_target(run
    DEPENDS ${PROJECT_NAME}
    COMMAND "$<TARGET_FILE:${PROJECT_NAME}>"
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
