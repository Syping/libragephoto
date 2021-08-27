cmake_minimum_required(VERSION 3.7)
project(ragephoto-qtviewer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FORCE_QT_VERSION "" CACHE STRING "Force Qt Version")
if(FORCE_QT_VERSION)
    set(QT_VERSION_MAJOR ${FORCE_QT_VERSION})
else()
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
endif()
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(QTVIEWER_SOURCES
    src/main.cpp
)

if(TARGET ragephoto)
    add_executable(ragephoto-qtviewer ${QTVIEWER_SOURCES})
    target_link_libraries(ragephoto-qtviewer Qt${QT_VERSION_MAJOR}::Widgets ragephoto)
    target_include_directories(ragephoto-qtviewer PRIVATE ${RAGEPHOTO_INCLUDE_DIRS})
else()
    message("ragephoto-qtviewer need to be build with libragephoto together")
endif()
