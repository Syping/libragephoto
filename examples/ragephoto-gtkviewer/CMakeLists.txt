cmake_minimum_required(VERSION 3.7)
enable_language(CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(gtkmm REQUIRED gtkmm-3.0)

project(ragephoto-gtkviewer LANGUAGES CXX)

set(GTKVIEWER_HEADERS
    src/PhotoViewer.h
)

set(GTKVIEWER_SOURCES
    src/main.cpp
    src/PhotoViewer.cpp
)

if(TARGET ragephoto)
    set(RAGEPHOTO_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src)
    add_executable(ragephoto-gtkviewer ${GTKVIEWER_HEADERS} ${GTKVIEWER_SOURCES})
    target_link_libraries(ragephoto-gtkviewer ${gtkmm_LIBRARIES} ragephoto)
    target_link_directories(ragephoto-gtkviewer PRIVATE ${gtkmm_LIBRARY_DIRS})
    target_include_directories(ragephoto-gtkviewer PRIVATE ${gtkmm_INCLUDE_DIRS} ${RAGEPHOTO_INCLUDE_DIRS})
else()
    message("ragephoto-gtkviewer need to be build with libragephoto together")
endif()
