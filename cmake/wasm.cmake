#[[**************************************************************************
* libragephoto RAGE Photo Parser
* Copyright (C) 2023-2025 Syping
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

add_executable(ragephoto-wasm
    ${RAGEPHOTO_HEADERS}
    ${RAGEPHOTO_SOURCES}
)
set_target_properties(ragephoto-wasm PROPERTIES
    C_STANDARD ${RAGEPHOTO_C_STANDARD}
    C_STANDARD_REQUIRED ON
    CXX_STANDARD ${RAGEPHOTO_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ON
    PREFIX "lib"
    OUTPUT_NAME "ragephoto"
)
target_compile_definitions(ragephoto-wasm PRIVATE
    LIBRAGEPHOTO_LIBRARY
    LIBRAGEPHOTO_WASM
    ${LIBRAGEPHOTO_DEFINES}
)
target_link_options(ragephoto-wasm PRIVATE
    "SHELL:-O3"
    "SHELL:--no-entry"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    "SHELL:-s MODULARIZE=1"
    "SHELL:-s WASM=1"
    "SHELL:-s WASM_BIGINT=1"
    "SHELL:-s EXPORT_NAME=libragephotoModule"
    "SHELL:-s EXPORTED_FUNCTIONS=_free,_malloc"
    "SHELL:-s EXPORTED_RUNTIME_METHODS=ccall,cwrap"
)
target_include_directories(ragephoto-wasm PUBLIC
    "${ragephoto_BINARY_DIR}/include"
    "${ragephoto_SOURCE_DIR}/src/core"
)
