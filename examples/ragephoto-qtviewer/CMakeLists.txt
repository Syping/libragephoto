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

if (TARGET ragephoto)
    set(RAGEPHOTO_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src)
    set(RAGEPHOTO_LIBRARIES ragephoto)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(RAGEPHOTO REQUIRED ragephoto)
endif()

add_executable(ragephoto-qtviewer ${QTVIEWER_SOURCES})
target_compile_options(ragephoto-qtviewer PRIVATE ${RAGEPHOTO_CFLAGS})
target_link_libraries(ragephoto-qtviewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${RAGEPHOTO_LIBRARIES})
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.13.0")
    target_link_directories(ragephoto-qtviewer PRIVATE ${RAGEPHOTO_LIBRARY_DIRS})
endif()
target_include_directories(ragephoto-qtviewer PRIVATE ${RAGEPHOTO_INCLUDE_DIRS})
