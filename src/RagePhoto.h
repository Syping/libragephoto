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

#ifndef RAGEPHOTO_H
#define RAGEPHOTO_H

#include "libragephoto_global.h"
#include <iostream>
#include <cstdint>
#include <cstdio>

class LIBRAGEPHOTO_EXPORT RagePhoto
{
public:
    enum class PhotoFormat : uint32_t {
        GTA5 = 0x01000000U,
        RDR2 = 0x04000000U,
        Undefined = 0,
    };
    RagePhoto();
    ~RagePhoto();
    bool load(const char *data, size_t length);
    const char *photoData();
    const uint32_t photoSize();
    const std::string description();
    const std::string json();
    const std::string header();
    const std::string title();

private:
    inline size_t bRead(const char *input, char *output, size_t *pos, size_t len);
    inline size_t bRead(const char *input, char *output, size_t *pos, size_t len, size_t inputLen);
    inline uint32_t charToUInt32BE(char *x);
    inline uint32_t charToUInt32LE(char *x);
    inline void uInt32ToCharBE(uint32_t x, char *y);
    inline void uInt32ToCharLE(uint32_t x, char *y);
    char* p_photoData;
    std::string p_descriptionString;
    std::string p_jsonString;
    std::string p_photoString;
    std::string p_titleString;
    uint32_t p_descBuffer;
    uint32_t p_descOffset;
    uint32_t p_endOfFile;
    uint32_t p_headerSum;
    uint32_t p_jsonBuffer;
    uint32_t p_jsonOffset;
    uint32_t p_photoBuffer;
    uint32_t p_photoSize;
    uint32_t p_titlBuffer;
    uint32_t p_titlOffset;
};

#endif // RAGEPHOTO_H
