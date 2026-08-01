cmake_minimum_required(VERSION 3.16)

project(build LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Use an explicitly supplied Qt location first. Otherwise, look in common
# developer locations so a normal Windows CMake configure also works.
if(NOT CMAKE_PREFIX_PATH AND NOT Qt6_DIR)
    if(WIN32)
        set(_qt_candidates
            "C:/Qt/6.11.1/mingw_64"
            "C:/Qt/6.11.1/msvc2022_64"
            "C:/Qt/6.10.0/mingw_64"
            "C:/Qt/6.10.0/msvc2022_64"
        )
    elseif(APPLE)
        set(_qt_candidates
            "/opt/homebrew/opt/qt"
            "/usr/local/opt/qt"
        )
    endif()

    foreach(_qt_candidate IN LISTS _qt_candidates)
        if(EXISTS "${_qt_candidate}/lib/cmake/Qt6/Qt6Config.cmake")
            set(CMAKE_PREFIX_PATH "${_qt_candidate}")
            break()
        endif()
    endforeach()
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Sql Widgets)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/backend/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cores/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/frontend/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

# Do not compile generated files if a build directory was accidentally placed
# below one of the source folders.
list(FILTER PROJECT_SOURCES EXCLUDE REGEX "/(build|cmake-build[^/]*)/")
list(FILTER PROJECT_SOURCES EXCLUDE REGEX "/[^/]*_autogen/")

if(APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${PROJECT_SOURCES})
else()
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/backend"
    "${CMAKE_CURRENT_SOURCE_DIR}/cores"
    "${CMAKE_CURRENT_SOURCE_DIR}/frontend"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Sql
    Qt6::Widgets
)

# The database and image assets are required beside the executable at runtime.
foreach(_runtime_directory IN ITEMS backend/Database assets)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_runtime_directory}")
        file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/${_runtime_directory}"
             DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    endif()
endforeach()

# Make the Windows build self-contained: Qt DLLs, plugins, and MinGW runtime
# are deployed beside the generated executable.
if(WIN32)
    get_target_property(_qt_core_location Qt6::Core IMPORTED_LOCATION_RELEASE)
    if(NOT _qt_core_location)
        get_target_property(_qt_core_location Qt6::Core IMPORTED_LOCATION)
    endif()

    if(_qt_core_location)
        get_filename_component(_qt_bin_directory "${_qt_core_location}" DIRECTORY)
        find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt HINTS "${_qt_bin_directory}")
    endif()

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-translations "$<TARGET_FILE:${PROJECT_NAME}>"
            COMMENT "Deploying Qt runtime beside ${PROJECT_NAME}"
        )
    else()
        message(WARNING "windeployqt was not found; add the Qt bin directory to PATH before running the executable.")
    endif()
endif()
