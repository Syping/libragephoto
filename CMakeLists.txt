#[[**************************************************************************
* libragephoto RAGE Photo Parser
* Copyright (C) 2021-2025 Syping
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

cmake_minimum_required(VERSION 3.16)
cmake_policy(VERSION 3.16...3.28)
project(ragephoto VERSION 0.8.0 LANGUAGES C CXX)
include(GNUInstallDirs)

# RagePhoto CMake includes
include(cmake/unicode.cmake)

# RagePhoto Top Level
if (${CMAKE_PROJECT_NAME} STREQUAL "ragephoto")
    set(RPTL_ON ON)
else()
    set(RPTL_ON OFF)
endif()

# RagePhoto Source files
option(RAGEPHOTO_C_LIBRARY "Build libragephoto as C library" OFF)
if (RAGEPHOTO_C_LIBRARY)
    set(RAGEPHOTO_HEADERS
        src/core/RagePhoto.h
        src/core/ragephoto_c
        src/core/ragephoto_c.hpp
        src/core/RagePhoto
        src/core/RagePhoto.hpp
        src/core/RagePhotoLibrary.h
        src/core/RagePhotoTypedefs.h
    )
    set(RAGEPHOTO_SOURCES
        src/core/RagePhoto.c
    )
else()
    set(RAGEPHOTO_HEADERS
        src/core/ragephoto_cxx
        src/core/ragephoto_cxx.hpp
        src/core/RagePhoto
        src/core/RagePhoto.hpp
        src/core/RagePhotoLibrary.h
        src/core/RagePhotoTypedefs.h
    )
    set(RAGEPHOTO_SOURCES
        src/core/RagePhoto.cpp
    )
endif()

# RagePhoto Library Type
option(RAGEPHOTO_STATIC "Build libragephoto as static library" OFF)
if (RAGEPHOTO_STATIC)
    set(LIBRAGEPHOTO_LIBTYPE LIBRAGEPHOTO_STATIC)
else()
    set(LIBRAGEPHOTO_LIBTYPE LIBRAGEPHOTO_SHARED)
endif()

# RagePhoto Benchmark
option(RAGEPHOTO_BENCHMARK "Build with libragephoto benchmark" OFF)

# RagePhoto Debug
option(RAGEPHOTO_DEBUG "Build with libragephoto debug output" OFF)

# RagePhoto API
option(RAGEPHOTO_C_API "Build libragephoto with C API support" ON)
if (RAGEPHOTO_C_LIBRARY)
    set(LIBRAGEPHOTO_API LIBRAGEPHOTO_C_ONLY)
else()
    if (RAGEPHOTO_C_API)
        set(LIBRAGEPHOTO_API LIBRAGEPHOTO_CXX_C)
        list(APPEND RAGEPHOTO_HEADERS
            src/core/RagePhoto.h
            src/core/ragephoto_c
            src/core/ragephoto_c.hpp
        )
    else()
        set(LIBRAGEPHOTO_API LIBRAGEPHOTO_CXX_ONLY)
    endif()
endif()

# RagePhoto C/C++ Standard
set(RAGEPHOTO_C_STANDARD "99" CACHE STRING "libragephoto C standard")
set(RAGEPHOTO_CXX_STANDARD "11" CACHE STRING "libragephoto C++ standard")

# RagePhoto Win32 Shared Resources
if (WIN32)
    string(TIMESTAMP ragephoto_BUILD_YEAR "%Y" UTC)
    configure_file(src/core/ragephoto.rc.in "${ragephoto_BINARY_DIR}/resources/ragephoto.rc" @ONLY)
    list(APPEND RAGEPHOTO_SHARED_RESOURCES
        "${ragephoto_BINARY_DIR}/resources/ragephoto.rc"
    )
endif()

# RagePhoto Configures + Target + Installs
configure_file(src/core/ragephoto.pc.in "${ragephoto_BINARY_DIR}/pkgconfig/ragephoto.pc" @ONLY)
configure_file(src/core/RagePhotoConfig.h.in "${ragephoto_BINARY_DIR}/include/RagePhotoConfig.h" @ONLY)
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
set_target_properties(ragephoto PROPERTIES
    C_STANDARD ${RAGEPHOTO_C_STANDARD}
    C_STANDARD_REQUIRED ON
    CXX_STANDARD ${RAGEPHOTO_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ON
)
target_compile_definitions(ragephoto PRIVATE
    LIBRAGEPHOTO_LIBRARY
    ${LIBRAGEPHOTO_DEFINES}
    $<$<BOOL:${RAGEPHOTO_BENCHMARK}>:RAGEPHOTO_BENCHMARK>
    $<$<BOOL:${RAGEPHOTO_DEBUG}>:RAGEPHOTO_DEBUG>
)
if (MSVC AND MSVC_VERSION GREATER_EQUAL 1914)
    target_compile_options(ragephoto PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>)
endif()
target_include_directories(ragephoto PUBLIC
    "${ragephoto_BINARY_DIR}/include"
    "${ragephoto_SOURCE_DIR}/src/core"
)
target_link_libraries(ragephoto PRIVATE
    $<$<BOOL:${ICONV_LINKED}>:iconv>
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
        set(EXTRACT_SOURCES src/extract/RagePhoto-Extract.c)
    else()
        set(EXTRACT_SOURCES src/extract/RagePhoto-Extract.cpp)
    endif()
    # RagePhoto-Extract Win32 Shared Resources
    if (WIN32)
        string(TIMESTAMP ragephoto_BUILD_YEAR "%Y" UTC)
        configure_file(src/extract/ragephoto-extract.rc.in "${ragephoto_BINARY_DIR}/resources/ragephoto-extract.rc" @ONLY)
        list(APPEND EXTRACT_RESOURCES
            "${ragephoto_BINARY_DIR}/resources/ragephoto-extract.rc"
        )
    endif()
    # RagePhoto-Extract Target + Installs
    add_executable(ragephoto-extract ${RAGEPHOTO_HEADERS} ${EXTRACT_SOURCES} ${EXTRACT_RESOURCES})
    set_target_properties(ragephoto-extract PROPERTIES
        INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
        CXX_STANDARD ${RAGEPHOTO_CXX_STANDARD}
        CXX_STANDARD_REQUIRED ON
    )
    if (MSVC AND MSVC_VERSION GREATER_EQUAL 1914)
        target_compile_options(ragephoto-extract PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>)
    endif()
    target_link_libraries(ragephoto-extract PRIVATE ragephoto)
    install(TARGETS ragephoto-extract DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()

# RagePhoto Python Package
option(RAGEPHOTO_PYTHON "Create ragephoto Python Package" OFF)
if (RAGEPHOTO_PYTHON)
    # Python Package Library file
    if (WIN32)
        set(PYRAGEPHOTO_LIBRARY "libragephoto.dll")
    else()
        set(PYRAGEPHOTO_LIBRARY "libragephoto.so")
    endif()
    # Generate Python Package Project files
    configure_file(src/python/setup.py.in "${ragephoto_BINARY_DIR}/pyragephoto/setup.py" @ONLY)
    configure_file(src/python/pyproject.toml.in "${ragephoto_BINARY_DIR}/pyragephoto/pyproject.toml" @ONLY)
    configure_file(src/python/__version__.py.in "${ragephoto_BINARY_DIR}/pyragephoto/ragephoto/__version__.py" @ONLY)
    # Python Package Source files + Target
    set(PYRAGEPHOTO_SOURCES
        "src/python/__init__.py"
        "src/python/libragephoto_loader.py"
        "src/python/ragephoto.py"
    )
    add_custom_target(pyragephoto SOURCES ${PYRAGEPHOTO_SOURCES})
    # Copy Python Package to build directory
    file(COPY ${PYRAGEPHOTO_SOURCES} DESTINATION "${ragephoto_BINARY_DIR}/pyragephoto/ragephoto")
    # Python Package Bundle Settings
    option(RAGEPHOTO_PYTHON_BUNDLE_LIBRARY "Bundle libragephoto with ragephoto Python Package" OFF)
    if (RAGEPHOTO_PYTHON_BUNDLE_LIBRARY)
        add_custom_command(
            TARGET ragephoto
            POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:ragephoto>" "${ragephoto_BINARY_DIR}/pyragephoto/ragephoto/${PYRAGEPHOTO_LIBRARY}"
            BYPRODUCTS "${ragephoto_BINARY_DIR}/pyragephoto/ragephoto/${PYRAGEPHOTO_LIBRARY}"
            VERBATIM
        )
    endif()
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
