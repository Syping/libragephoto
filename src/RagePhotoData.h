/*****************************************************************************
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
*****************************************************************************/

#ifndef RAGEPHOTODATA_H
#define RAGEPHOTODATA_H

#ifdef __cplusplus
#include "libragephoto_global.h"
#include <iostream>
#include <cstdint>

struct LIBRAGEPHOTO_CXX_EXPORT RagePhotoData
{
    bool photoLoaded;
    char* photoData;
    std::string description;
    std::string json;
    std::string header;
    std::string title;
    uint8_t error;
    uint32_t descBuffer;
    uint32_t descOffset;
    uint32_t endOfFile;
    uint32_t headerSum;
    uint32_t jsonBuffer;
    uint32_t jsonOffset;
    uint32_t photoBuffer;
    uint32_t photoFormat;
    uint32_t photoSize;
    uint32_t titlBuffer;
    uint32_t titlOffset;
    uint32_t unnamedSum1;
    uint32_t unnamedSum2;
};
#endif // __cplusplus

#endif // RAGEPHOTODATA_H
