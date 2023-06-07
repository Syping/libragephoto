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

set(RAGEPHOTO_UNICODE "" CACHE STRING "libragephoto Unicode implementation")
if (RAGEPHOTO_UNICODE)
    string(TOUPPER "UNICODE_${RAGEPHOTO_UNICODE}" UNICODE_DEF)
    list(APPEND LIBRAGEPHOTO_DEFINES
        "${UNICODE_DEF}"
    )
    message("-- UnicodeCvt - ${RAGEPHOTO_UNICODE}")
else()
    # RagePhoto Unicode functionality tests
    message("-- Testing codecvt")
    try_run(CODECVT_RUN CODECVT_COMPILE "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/tests/CodecvtTest.cpp")
    if (CODECVT_COMPILE AND CODECVT_RUN EQUAL 0)
        set(CODECVT_COMPAT TRUE)
        message("-- Testing codecvt - yes")
    else()
        message("-- Testing codecvt - no")
    endif()
    
    message("-- Testing iconv")
    try_run(ICONV_RUN ICONV_COMPILE "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/tests/IconvTest.cpp")
    if (ICONV_COMPILE AND ICONV_RUN EQUAL 0)
        set(ICONV_COMPAT TRUE)
        message("-- Testing iconv - yes")
    else()
        message("-- Testing iconv - no")
    endif()
    
    if (WIN32)
        message("-- Testing wincvt")
        try_run(WINCVT_RUN WINCVT_COMPILE "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/tests/WincvtTest.cpp")
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
            "UNICODE_WINCVT"
        )
        message("-- UnicodeCvt - wincvt")
    elseif (CODECVT_COMPAT AND NOT RAGEPHOTO_C_LIBRARY)
        list(APPEND LIBRAGEPHOTO_DEFINES
            "UNICODE_CODECVT"
        )
        message("-- UnicodeCvt - codecvt")
    elseif (ICONV_COMPAT)
        list(APPEND LIBRAGEPHOTO_DEFINES
            "UNICODE_ICONV"
        )
        message("-- UnicodeCvt - iconv")
    else()
        message("-- UnicodeCvt - none")
    endif()
endif()
