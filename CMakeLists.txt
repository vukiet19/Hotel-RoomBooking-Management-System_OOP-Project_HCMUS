cmake_minimum_required(VERSION 3.16)

project(Hotel_Management LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 1. Auto-detect Qt installation on macOS & Windows
if(APPLE AND NOT CMAKE_PREFIX_PATH)
    if(EXISTS "/opt/homebrew/opt/qt")
        set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt") # Mac Apple Silicon
    elseif(EXISTS "/usr/local/opt/qt")
        set(CMAKE_PREFIX_PATH "/usr/local/opt/qt")   # Mac Intel
    endif()
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Sql Widgets)

# 2. Gather source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "backend/*.cpp"
    "cores/*.cpp"
    "frontend/*.cpp"
    "main.cpp"
)

# Exclude build and autogen directories if nested inside source tree
list(FILTER SOURCES EXCLUDE REGEX "build/")
list(FILTER SOURCES EXCLUDE REGEX "_autogen/")

# 3. Define Executable
add_executable(${PROJECT_NAME} ${SOURCES})

# 4. Include directories & Link Qt libraries
target_include_directories(${PROJECT_NAME} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/backend"
    "${CMAKE_CURRENT_SOURCE_DIR}/cores"
    "${CMAKE_CURRENT_SOURCE_DIR}/frontend"
)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Sql Qt6::Widgets)

# 5. Copy Database and Assets to build output
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/backend/Database"
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets"
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

if(APPLE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/backend/Database"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Database"
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    )
endif()

