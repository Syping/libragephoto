cmake_minimum_required(VERSION 3.7)
enable_language(CXX)

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

project(ragephoto LANGUAGES CXX)

set(RAGEPHOTO_SOURCES
    src/RagePhoto.cpp
    src/RagePhoto.h
)
set(RAGEPHOTO_HEADERS
    src/libragephoto_global.h
    src/RagePhoto.h
)

option(BUILD_SHARED "Build libragephoto as shared library" ON)
if (BUILD_SHARED)
    add_library(ragephoto SHARED ${RAGEPHOTO_HEADERS} ${RAGEPHOTO_SOURCES})
    set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
else()
    add_library(ragephoto STATIC ${RAGEPHOTO_HEADERS} ${RAGEPHOTO_SOURCES})
endif()

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
install(TARGETS ragephoto DESTINATION lib)
install(FILES ${RAGEPHOTO_HEADERS} DESTINATION include)

option(WITH_EXAMPLES "Build ragephoto examples" OFF)
if (WITH_EXAMPLES)
    add_subdirectory(examples/ragephoto-gtkviewer)
endif()

option(WITH_EXTRACT "Build ragephoto-extract" ON)
if (WITH_EXTRACT)
    project(ragephoto-extract LANGUAGES CXX)
    set(EXTRACT_SOURCES
        src/RagePhoto-Extract.cpp
    )
    add_executable(ragephoto-extract ${RAGEPHOTO_HEADERS} ${EXTRACT_SOURCES})
    target_link_libraries(ragephoto-extract ragephoto)
    install(TARGETS ragephoto-extract DESTINATION bin)
endif()
