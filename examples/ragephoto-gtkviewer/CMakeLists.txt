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
project(ragephoto-gtkviewer LANGUAGES CXX)
include(GNUInstallDirs)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

set(GTKVIEWER_SOURCES
    src/main.cpp
)

if (TARGET ragephoto)
    set(RAGEPHOTO_LIBRARIES ragephoto)
    set(RAGEPHOTO_LIBRARY_DIRS ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
else()
    pkg_check_modules(RAGEPHOTO REQUIRED ragephoto)
endif()

add_executable(ragephoto-gtkviewer WIN32 ${GTKVIEWER_SOURCES})
set_target_properties(ragephoto-gtkviewer PROPERTIES
    INSTALL_RPATH "${GTKMM_LIBRARY_DIRS};${RAGEPHOTO_LIBRARY_DIRS}"
)
target_compile_options(ragephoto-gtkviewer PRIVATE ${GTKMM_CFLAGS} ${RAGEPHOTO_CFLAGS})
target_link_libraries(ragephoto-gtkviewer PRIVATE ${GTKMM_LIBRARIES} ${RAGEPHOTO_LIBRARIES})
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.13.0")
    target_link_directories(ragephoto-gtkviewer PRIVATE ${GTKMM_LIBRARY_DIRS} ${RAGEPHOTO_LIBRARY_DIRS})
endif()
target_include_directories(ragephoto-gtkviewer PRIVATE ${GTKMM_INCLUDE_DIRS} ${RAGEPHOTO_INCLUDE_DIRS})
install(TARGETS ragephoto-gtkviewer DESTINATION ${CMAKE_INSTALL_BINDIR})
