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
find_package(Doxygen REQUIRED)
include(GNUInstallDirs)

if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation.")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(
    doc ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generate API documentation with Doxygen"
    VERBATIM
)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION ${CMAKE_INSTALL_DOCDIR})
