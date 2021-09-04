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
project(ragephoto VERSION 0.1 LANGUAGES CXX)
include(GNUInstallDirs)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("-- Testing codecvt")
try_run(CODECVT_RUN CODECVT_COMPILE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/tests/CodecvtTest.cpp)
if (CODECVT_COMPILE AND CODECVT_RUN EQUAL 0)
    list(APPEND LIBRAGEPHOTO_DEFINES
        CODECVT_COMPATIBLE
    )
    message("-- Testing codecvt - yes")
else()
    message("-- Testing codecvt - no")
endif()

message("-- Testing iconv")
try_run(ICONV_RUN ICONV_COMPILE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/tests/IconvTest.cpp)
if (ICONV_COMPILE AND ICONV_RUN EQUAL 0)
    list(APPEND LIBRAGEPHOTO_DEFINES
        ICONV_COMPATIBLE
    )
    message("-- Testing iconv - yes")
else()
    message("-- Testing iconv - no")
endif()

set(RAGEPHOTO_HEADERS
    src/libragephoto_global.h
    src/RagePhoto.h
)
set(RAGEPHOTO_SOURCES
    src/RagePhoto.cpp
)

option(BUILD_SHARED "Build libragephoto as shared library" ON)
if (BUILD_SHARED)
    add_library(ragephoto SHARED ${RAGEPHOTO_HEADERS} ${RAGEPHOTO_SOURCES})
    set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
    set(LIBRAGEPHOTO_LIBTYPE -DLIBRAGEPHOTO_SHARED)
else()
    add_library(ragephoto STATIC ${RAGEPHOTO_HEADERS} ${RAGEPHOTO_SOURCES})
    set(LIBRAGEPHOTO_LIBTYPE -DLIBRAGEPHOTO_STATIC)
endif()
configure_file(src/ragephoto.pc.in pkgconfig/ragephoto.pc @ONLY)

option(WITH_BENCHMARK "Build with libragephoto benchmark" OFF)
if (WITH_BENCHMARK)
    list(APPEND LIBRAGEPHOTO_DEFINES
        RAGEPHOTO_BENCHMARK
    )
endif()

target_compile_definitions(ragephoto PRIVATE
    LIBRAGEPHOTO_LIBRARY
    ${LIBRAGEPHOTO_DEFINES}
)
target_compile_definitions(ragephoto PUBLIC
    ${LIBRAGEPHOTO_LIBTYPE}
)
install(TARGETS ragephoto DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${RAGEPHOTO_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pkgconfig/ragephoto.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

option(WITH_GTK_EXAMPLE "Build libragephoto with GTK Photo Viewer" OFF)
if (WITH_GTK_EXAMPLE)
    add_subdirectory(examples/ragephoto-gtkviewer)
endif()

option(WITH_QT_EXAMPLE "Build libragephoto with Qt Photo Viewer" OFF)
if (WITH_QT_EXAMPLE)
    add_subdirectory(examples/ragephoto-qtviewer)
endif()

if (CMAKE_PROJECT_NAME EQUAL "ragephoto")
    option(WITH_EXTRACT "Build libragephoto with ragephoto-extract" ON)
else()
    option(WITH_EXTRACT "Build libragephoto with ragephoto-extract" OFF)
endif()
if (WITH_EXTRACT)
    set(EXTRACT_SOURCES
        src/RagePhoto-Extract.cpp
    )
    add_executable(ragephoto-extract ${RAGEPHOTO_HEADERS} ${EXTRACT_SOURCES})
    target_link_libraries(ragephoto-extract PRIVATE ragephoto)
    install(TARGETS ragephoto-extract DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
