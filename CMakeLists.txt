cmake_minimum_required(VERSION 3.7)
enable_language(CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    list(APPEND LIBRAGEPHOTO_DEFINES
        USE_WINAPI
    )
endif()

message("-- Testing iconv")
try_run(ICONV_RUN ICONV_COMPILE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/tests/IconvTest.cpp)
if (ICONV_COMPILE)
    list(APPEND LIBRAGEPHOTO_DEFINES
        USE_ICONV
    )
    message("-- Testing iconv - yes")
else()
    message("-- Testing iconv - no")
endif()

option(WITH_BENCHMARK "Benchmark RagePhoto Parsing Engine" OFF)
if (WITH_BENCHMARK)
    list(APPEND LIBRAGEPHOTO_DEFINES
        RAGEPHOTO_BENCHMARK
    )
endif()

project(ragephoto LANGUAGES CXX)
add_library(ragephoto SHARED
    src/libragephoto_global.h
    src/RagePhoto.cpp
    src/RagePhoto.h
)
target_compile_definitions(ragephoto PRIVATE
    LIBRAGEPHOTO_LIBRARY
    ${LIBRAGEPHOTO_DEFINES}
)
install(TARGETS ragephoto DESTINATION lib)
install(FILES
    src/RagePhoto.h
    src/libragephoto_global.h
    DESTINATION include
)
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

project(ragephoto-extract LANGUAGES CXX)
add_executable(ragephoto-extract
    src/libragephoto_global.h
    src/RagePhoto-Extract.cpp
    src/RagePhoto.h
)
target_link_libraries(ragephoto-extract ragephoto)
install(TARGETS ragephoto-extract DESTINATION bin)
