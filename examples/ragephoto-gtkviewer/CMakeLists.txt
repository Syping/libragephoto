cmake_minimum_required(VERSION 3.7)
project(ragephoto-gtkviewer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

set(GTKVIEWER_HEADERS
    src/PhotoViewer.h
)

set(GTKVIEWER_SOURCES
    src/main.cpp
    src/PhotoViewer.cpp
)

if (TARGET ragephoto)
    set(RAGEPHOTO_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src)
    set(RAGEPHOTO_LIBRARIES ragephoto)
else()
    pkg_check_modules(RAGEPHOTO REQUIRED ragephoto)
endif()

add_executable(ragephoto-gtkviewer ${GTKVIEWER_HEADERS} ${GTKVIEWER_SOURCES})
target_compile_options(ragephoto-gtkviewer PRIVATE ${GTKMM_CFLAGS} ${RAGEPHOTO_CFLAGS})
target_link_libraries(ragephoto-gtkviewer PRIVATE ${GTKMM_LIBRARIES} ${RAGEPHOTO_LIBRARIES})
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.13.0")
    target_link_directories(ragephoto-gtkviewer PRIVATE ${GTKMM_LIBRARY_DIRS} ${RAGEPHOTO_LIBRARY_DIRS})
endif()
target_include_directories(ragephoto-gtkviewer PRIVATE ${GTKMM_INCLUDE_DIRS} ${RAGEPHOTO_INCLUDE_DIRS})
install(TARGETS ragephoto-gtkviewer DESTINATION bin)
