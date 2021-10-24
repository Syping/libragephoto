#[[**************************************************************************
* libragephoto RAGE Photo Parser
* Copyright (C) 2021 Syping
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* This software is provided as-is, no warranties are given to you, we are not
* responsible for anything with use of the software, you are self responsible.
****************************************************************************]]

cmake_minimum_required(VERSION 3.7)
project(ragephoto-qtviewer LANGUAGES CXX)
include(GNUInstallDirs)

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

add_executable(ragephoto-qtviewer WIN32 ${QTVIEWER_SOURCES})
target_compile_options(ragephoto-qtviewer PRIVATE ${RAGEPHOTO_CFLAGS})
target_link_libraries(ragephoto-qtviewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${RAGEPHOTO_LIBRARIES})
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.13.0")
    target_link_directories(ragephoto-qtviewer PRIVATE ${RAGEPHOTO_LIBRARY_DIRS})
endif()
target_include_directories(ragephoto-qtviewer PRIVATE ${RAGEPHOTO_INCLUDE_DIRS})
install(TARGETS ragephoto-qtviewer DESTINATION ${CMAKE_INSTALL_BINDIR})
