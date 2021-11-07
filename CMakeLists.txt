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
project(ragephoto VERSION 0.1.0 LANGUAGES CXX)
include(GNUInstallDirs)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# RagePhoto functionality tests
message("-- Testing codecvt")
try_run(CODECVT_RUN CODECVT_COMPILE ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/tests/CodecvtTest.cpp)
if (CODECVT_COMPILE AND CODECVT_RUN EQUAL 0)
    set(CODECVT_COMPAT TRUE)
    message("-- Testing codecvt - yes")
else()
    message("-- Testing codecvt - no")
endif()

message("-- Testing iconv")
try_run(ICONV_RUN ICONV_COMPILE ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/tests/IconvTest.cpp)
if (ICONV_COMPILE AND ICONV_RUN EQUAL 0)
    set(ICONV_COMPAT TRUE)
    message("-- Testing iconv - yes")
else()
    message("-- Testing iconv - no")
endif()

if (WIN32)
    message("-- Testing wincvt")
    try_run(WINCVT_RUN WINCVT_COMPILE ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/tests/WincvtTest.cpp)
    if (WINCVT_COMPILE AND WINCVT_RUN EQUAL 0)
        set(WINCVT_COMPAT TRUE)
        message("-- Testing wincvt - yes")
    else()
        message("-- Testing wincvt - no")
    endif()
endif()

# Unicode implementation for RagePhoto
if (WINCVT_COMPAT)
    list(APPEND LIBRAGEPHOTO_DEFINES
        UNICODE_WINCVT
    )
elseif (CODECVT_COMPAT)
    list(APPEND LIBRAGEPHOTO_DEFINES
        UNICODE_CODECVT
    )
elseif (ICONV_COMPAT)
    list(APPEND LIBRAGEPHOTO_DEFINES
        UNICODE_ICONV
    )
endif()

# RagePhoto Source files
set(RAGEPHOTO_HEADERS
    src/RagePhoto.h
    src/RagePhotoData.h
)
set(RAGEPHOTO_SOURCES
    src/RagePhoto.cpp
)

# RagePhoto Library Type
option(BUILD_SHARED "Build libragephoto as shared library" ON)
if (BUILD_SHARED)
    option(WITH_C_API "Build libragephoto with C API support" ON)
    set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
    set(LIBRAGEPHOTO_LIBTYPE LIBRAGEPHOTO_SHARED)
else()
    option(WITH_C_API "Build libragephoto with C API support" OFF)
    set(LIBRAGEPHOTO_LIBTYPE LIBRAGEPHOTO_STATIC)
endif()

# RagePhoto Benchmark
option(WITH_BENCHMARK "Build with libragephoto benchmark" OFF)
if (WITH_BENCHMARK)
    list(APPEND LIBRAGEPHOTO_DEFINES
        RAGEPHOTO_BENCHMARK
    )
endif()

# RagePhoto C API
if (WITH_C_API)
    set(LIBRAGEPHOTO_API LIBRAGEPHOTO_C_API)
    list(APPEND RAGEPHOTO_HEADERS
        src/RagePhotoA.h
    )
else()
    set(LIBRAGEPHOTO_API LIBRAGEPHOTO_C_NOAPI)
endif()

# RagePhoto Configures + Target + Installs
configure_file(src/ragephoto.pc.in pkgconfig/ragephoto.pc @ONLY)
configure_file(src/libragephoto_global.h.in include/libragephoto_global.h @ONLY)
list(APPEND RAGEPHOTO_HEADERS
    ${PROJECT_BINARY_DIR}/include/libragephoto_global.h
)
if (BUILD_SHARED)
    add_library(ragephoto SHARED ${RAGEPHOTO_HEADERS} ${RAGEPHOTO_SOURCES})
else()
    add_library(ragephoto STATIC ${RAGEPHOTO_HEADERS} ${RAGEPHOTO_SOURCES})
endif()
target_compile_definitions(ragephoto PRIVATE
    LIBRAGEPHOTO_LIBRARY
    ${LIBRAGEPHOTO_DEFINES}
)
target_include_directories(ragephoto PUBLIC
    ${PROJECT_BINARY_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)
install(TARGETS ragephoto DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${RAGEPHOTO_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RagePhoto)
install(FILES ${PROJECT_BINARY_DIR}/pkgconfig/ragephoto.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# RagePhoto Documentation
option(WITH_DOCUMENTATION "Build libragephoto with documentation" OFF)
if (WITH_DOCUMENTATION)
    add_subdirectory(doc)
endif()

# RagePhoto Examples
option(WITH_GTK_EXAMPLE "Build libragephoto with GTK Photo Viewer" OFF)
if (WITH_GTK_EXAMPLE)
    add_subdirectory(examples/ragephoto-gtkviewer)
endif()

option(WITH_QT_EXAMPLE "Build libragephoto with Qt Photo Viewer" OFF)
if (WITH_QT_EXAMPLE)
    add_subdirectory(examples/ragephoto-qtviewer)
endif()

# RagePhoto Extract Tool
if (${CMAKE_PROJECT_NAME} STREQUAL "ragephoto")
    option(WITH_EXTRACT "Build libragephoto with ragephoto-extract" ON)
else()
    option(WITH_EXTRACT "Build libragephoto with ragephoto-extract" OFF)
endif()
if (WITH_EXTRACT)
    if (WITH_C_API)
        enable_language(C)
        set(EXTRACT_SOURCES src/RagePhoto-Extract.c)
    else()
        set(EXTRACT_SOURCES src/RagePhoto-Extract.cpp)
    endif()
    add_executable(ragephoto-extract ${RAGEPHOTO_HEADERS} ${EXTRACT_SOURCES})
    target_link_libraries(ragephoto-extract PRIVATE ragephoto)
    install(TARGETS ragephoto-extract DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
