#[[**************************************************************************
* libragephoto RAGE Photo Parser
* Copyright (C) 2021-2023 Syping
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

cmake_minimum_required(VERSION 3.8)
project(ragephoto VERSION 0.5.0 LANGUAGES C CXX)
include(GNUInstallDirs)

# RagePhoto CMake includes
include(cmake/cxxstd.cmake)
include(cmake/unicode.cmake)

# RagePhoto Top Level ON
if (${CMAKE_PROJECT_NAME} STREQUAL "ragephoto")
    set(RPTL_ON ON)
else()
    set(RPTL_ON OFF)
endif()

# RagePhoto Source files
option(RAGEPHOTO_C_LIBRARY "Build libragephoto as C library" OFF)
if (RAGEPHOTO_C_LIBRARY)
    set(RAGEPHOTO_HEADERS
        src/RagePhotoC.h
        src/RagePhotoLibrary.h
        src/RagePhotoTypedefs.h
    )
    set(RAGEPHOTO_SOURCES
        src/RagePhoto.c
    )
else()
    set(RAGEPHOTO_HEADERS
        src/RagePhoto.h
        src/RagePhotoLibrary.h
        src/RagePhotoTypedefs.h
    )
    set(RAGEPHOTO_SOURCES
        src/RagePhoto.cpp
    )
endif()

# RagePhoto Library Type
option(RAGEPHOTO_STATIC "Build libragephoto as static library" OFF)
if (RAGEPHOTO_STATIC)
    option(RAGEPHOTO_C_API "Build libragephoto with C API support" OFF)
    set(LIBRAGEPHOTO_LIBTYPE LIBRAGEPHOTO_STATIC)
else()
    option(RAGEPHOTO_C_API "Build libragephoto with C API support" ON)
    set(LIBRAGEPHOTO_LIBTYPE LIBRAGEPHOTO_SHARED)
endif()

# RagePhoto Benchmark
option(RAGEPHOTO_BENCHMARK "Build with libragephoto benchmark" OFF)
if (RAGEPHOTO_BENCHMARK)
    list(APPEND LIBRAGEPHOTO_DEFINES
        RAGEPHOTO_BENCHMARK
    )
endif()

# RagePhoto C API
if (RAGEPHOTO_C_LIBRARY)
    set(LIBRAGEPHOTO_API LIBRAGEPHOTO_C_API)
    list(APPEND RAGEPHOTO_HEADERS
        src/RagePhotoA.h
    )
else()
    if (RAGEPHOTO_C_API)
        set(LIBRAGEPHOTO_API LIBRAGEPHOTO_C_API)
        list(APPEND RAGEPHOTO_HEADERS
            src/RagePhotoA.h
            src/RagePhotoC.h
        )
    else()
        set(LIBRAGEPHOTO_API LIBRAGEPHOTO_C_NOAPI)
    endif()
endif()

# RagePhoto Win32 Shared Resources
if (WIN32)
    string(TIMESTAMP ragephoto_BUILD_YEAR "%Y" UTC)
    configure_file(src/ragephoto.rc.in "${ragephoto_BINARY_DIR}/resources/ragephoto.rc" @ONLY)
    list(APPEND RAGEPHOTO_SHARED_RESOURCES
        "${ragephoto_BINARY_DIR}/resources/ragephoto.rc"
    )
endif()

# RagePhoto Configures + Target + Installs
configure_file(src/ragephoto.pc.in "${ragephoto_BINARY_DIR}/pkgconfig/ragephoto.pc" @ONLY)
configure_file(src/RagePhotoConfig.h.in "${ragephoto_BINARY_DIR}/include/RagePhotoConfig.h" @ONLY)
list(APPEND RAGEPHOTO_HEADERS
    "${ragephoto_BINARY_DIR}/include/RagePhotoConfig.h"
)
if (RAGEPHOTO_STATIC)
    add_library(ragephoto STATIC ${RAGEPHOTO_HEADERS} ${RAGEPHOTO_SOURCES} ${RAGEPHOTO_STATIC_RESOURCES})
else()
    add_library(ragephoto SHARED ${RAGEPHOTO_HEADERS} ${RAGEPHOTO_SOURCES} ${RAGEPHOTO_SHARED_RESOURCES})
    set_target_properties(ragephoto PROPERTIES
        PREFIX "lib"
        VERSION "${ragephoto_VERSION}"
        SOVERSION "${ragephoto_VERSION}"
    )
endif()
target_compile_definitions(ragephoto PRIVATE
    LIBRAGEPHOTO_LIBRARY
    ${LIBRAGEPHOTO_DEFINES}
)
if (MSVC AND MSVC_VERSION GREATER_EQUAL 1914 AND NOT RAGEPHOTO_C_LIBRARY)
    target_compile_options(ragephoto PRIVATE "/Zc:__cplusplus")
endif()
target_include_directories(ragephoto PUBLIC
    "${ragephoto_BINARY_DIR}/include"
    "${ragephoto_SOURCE_DIR}/src"
)
install(TARGETS ragephoto
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
install(FILES ${RAGEPHOTO_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/RagePhoto")
if (UNIX)
    install(FILES "${ragephoto_BINARY_DIR}/pkgconfig/ragephoto.pc" DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endif()

# RagePhoto WebAssembly Target
if (EMSCRIPTEN)
    include(cmake/wasm.cmake)
endif()

# RagePhoto Documentation
option(RAGEPHOTO_DOC "Build libragephoto with documentation" OFF)
if (RAGEPHOTO_DOC)
    add_subdirectory(doc)
endif()

# RagePhoto Examples
option(RAGEPHOTO_EXAMPLE_GTKVIEWER "Build libragephoto with GTK Photo Viewer" OFF)
if (RAGEPHOTO_EXAMPLE_GTKVIEWER)
    add_subdirectory(examples/ragephoto-gtkviewer)
endif()

option(RAGEPHOTO_EXAMPLE_QTVIEWER "Build libragephoto with Qt Photo Viewer" OFF)
if (RAGEPHOTO_EXAMPLE_QTVIEWER)
    add_subdirectory(examples/ragephoto-qtviewer)
endif()

# RagePhoto Extract Tool
option(RAGEPHOTO_EXTRACT "Build libragephoto with ragephoto-extract" ${RPTL_ON})
if (RAGEPHOTO_EXTRACT)
    # RagePhoto-Extract Source files
    if (RAGEPHOTO_C_API)
        set(EXTRACT_SOURCES src/RagePhoto-Extract.c)
    else()
        set(EXTRACT_SOURCES src/RagePhoto-Extract.cpp)
    endif()
    # RagePhoto-Extract Win32 Shared Resources
    if (WIN32)
        string(TIMESTAMP ragephoto_BUILD_YEAR "%Y" UTC)
        configure_file(src/ragephoto-extract.rc.in "${ragephoto_BINARY_DIR}/resources/ragephoto-extract.rc" @ONLY)
        list(APPEND EXTRACT_RESOURCES
            "${ragephoto_BINARY_DIR}/resources/ragephoto-extract.rc"
        )
    endif()
    # RagePhoto-Extract Target + Installs
    add_executable(ragephoto-extract ${RAGEPHOTO_HEADERS} ${EXTRACT_SOURCES} ${EXTRACT_RESOURCES})
    set_target_properties(ragephoto-extract PROPERTIES
        INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
    )
    if (MSVC AND MSVC_VERSION GREATER_EQUAL 1914 AND NOT RAGEPHOTO_C_API)
        target_compile_options(ragephoto-extract PRIVATE "/Zc:__cplusplus")
    endif()
    target_link_libraries(ragephoto-extract PRIVATE ragephoto)
    install(TARGETS ragephoto-extract DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()

# CPack Package Generation
if (RPTL_ON)
    include(InstallRequiredSystemLibraries)
    set(CPACK_PACKAGE_DESCRIPTION "Open Source RAGE Photo Parser for GTA V and RDR 2")
    set(CPACK_PACKAGE_NAME "libragephoto")
    set(CPACK_PACKAGE_VENDOR "Syping")
    set(CPACK_PACKAGE_VERSION_MAJOR "${ragephoto_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${ragephoto_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${ragephoto_VERSION_PATCH}")
    set(CPACK_STRIP_FILES ON)
    include(CPack)
endif()
