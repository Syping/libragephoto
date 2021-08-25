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

#include "RagePhoto.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

#ifdef RAGEPHOTO_BENCHMARK
#include <chrono>
#endif

#ifdef USE_ICONV
#include "iconv.h"
#endif

RagePhoto::RagePhoto()
{
    p_photoLoaded = false;
    p_photoData = nullptr;
    p_error = Error::Uninitialised;
}

RagePhoto::~RagePhoto()
{
    free(p_photoData);
}

void RagePhoto::clear()
{
    if (p_photoLoaded) {
        free(p_photoData);
        p_photoData = nullptr;
        p_photoLoaded = false;
    }
    p_descriptionString.clear();
    p_jsonString.clear();
    p_photoString.clear();
    p_titleString.clear();
    p_error = Error::Uninitialised;
}

bool RagePhoto::load(const char *data, size_t length)
{
#ifdef RAGEPHOTO_BENCHMARK
    auto benchmark_parse_start = std::chrono::high_resolution_clock::now();
#endif

    // Avoid data conflicts
    clear();

    size_t pos = 0;
    char uInt32Buffer[4];
    size_t size = readBuffer(data, uInt32Buffer, &pos, 4, length);
    if (size != 4) {
        p_error = Error::NoFormatIdentifier; // 1
        return false;
    }

    uint32_t format = charToUInt32LE(uInt32Buffer);
    if (format == static_cast<uint32_t>(PhotoFormat::GTA5)) {
        char photoHeader[256];
        size = readBuffer(data, photoHeader, &pos, 256, length);
        if (size != 256) {
            p_error = Error::IncompleteHeader; // 3
            return false;
        }

#ifdef USE_ICONV
        iconv_t iconv_in = iconv_open("UTF-8", "UTF-16LE");
        if (iconv_in == (iconv_t)-1) {
            p_error = Error::UnicodeInitError; // 4
            return false;
        }
        char photoString[256];
        size_t src_s = sizeof(photoHeader);
        size_t dst_s = sizeof(photoString);
        char *src = photoHeader;
        char *dst = photoString;
        size_t ret = iconv(iconv_in, &src, &src_s, &dst, &dst_s);
        iconv_close(iconv_in);
        if (ret == static_cast<size_t>(-1)) {
            p_error = Error::UnicodeHeaderError; // 5
            return false;
        }
        p_photoString = std::string(photoString);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteChecksum; // 6
            return false;
        }
        p_headerSum = charToUInt32LE(uInt32Buffer);

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteEOF; // 7
            return false;
        }
        p_endOfFile = charToUInt32LE(uInt32Buffer);

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteJsonOffset; // 8
            return false;
        }
        p_jsonOffset = charToUInt32LE(uInt32Buffer);

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteTitleOffset; // 9
            return false;
        }
        p_titlOffset = charToUInt32LE(uInt32Buffer);

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteDescOffset; // 10
            return false;
        }
        p_descOffset = charToUInt32LE(uInt32Buffer);

        char markerBuffer[4];
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteJpegMarker; // 11
            return false;
        }
        if (strncmp(markerBuffer, "JPEG", 4) != 0) {
            p_error = Error::IncorrectJpegMarker; // 12
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompletePhotoBuffer; // 13
            return false;
        }
        p_photoBuffer = charToUInt32LE(uInt32Buffer);

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompletePhotoSize; // 14
            return false;
        }
        p_photoSize = charToUInt32LE(uInt32Buffer);

        p_photoData = static_cast<char*>(malloc(p_photoSize));
        if (!p_photoData) {
            p_error = Error::PhotoMallocError; // 15
            return false;
        }
        size = readBuffer(data, p_photoData, &pos, p_photoSize, length);
        if (size != p_photoSize) {
            free(p_photoData);
            p_photoData = nullptr;
            p_error = Error::PhotoReadError; // 16
            return false;
        }
        p_photoLoaded = true;

        pos = p_jsonOffset + 264;
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteJsonMarker; // 17
            return false;
        }
        if (strncmp(markerBuffer, "JSON", 4) != 0) {
            p_error = Error::IncorrectJsonMarker; // 18
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteJsonBuffer; // 19
            return false;
        }
        p_jsonBuffer = charToUInt32LE(uInt32Buffer);

        char *t_jsonData = static_cast<char*>(malloc(p_jsonBuffer));
        if (!t_jsonData) {
            p_error = Error::JsonMallocError; // 20
            return false;
        }
        size = readBuffer(data, t_jsonData, &pos, p_jsonBuffer, length);
        if (size != p_jsonBuffer) {
            free(t_jsonData);
            p_error = Error::JsonReadError; // 21
            return false;
        }
        p_jsonString = std::string(t_jsonData);
        free(t_jsonData);

        pos = p_titlOffset + 264;
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteTitleMarker; // 22
            return false;
        }
        if (strncmp(markerBuffer, "TITL", 4) != 0) {
            p_error = Error::IncorrectTitleMarker; // 23
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteTitleBuffer; // 24
            return false;
        }
        p_titlBuffer = charToUInt32LE(uInt32Buffer);

        char *t_titlData = static_cast<char*>(malloc(p_titlBuffer));
        if (!t_titlData) {
            p_error = Error::TitleMallocError; // 25
            return false;
        }
        size = readBuffer(data, t_titlData, &pos, p_titlBuffer, length);
        if (size != p_titlBuffer) {
            free(t_titlData);
            p_error = Error::TitleReadError; // 26
            return false;
        }
        p_titleString = std::string(t_titlData);
        free(t_titlData);

        pos = p_descOffset + 264;
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteDescMarker; // 27
            return false;
        }
        if (strncmp(markerBuffer, "DESC", 4) != 0) {
            p_error = Error::IncorrectDescMarker; // 28
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteDescBuffer; // 29
            return false;
        }
        p_descBuffer = charToUInt32LE(uInt32Buffer);

        char *t_descData = static_cast<char*>(malloc(p_descBuffer));
        if (!t_descData) {
            p_error = Error::DescMallocError; // 30
            return false;
        }
        size = readBuffer(data, t_descData, &pos, p_descBuffer, length);
        if (size != p_descBuffer) {
            free(t_descData);
            p_error = Error::DescReadError; // 31
            return false;
        }
        p_descriptionString = std::string(t_descData);
        free(t_descData);

        pos = p_endOfFile + 260;
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteJendMarker; // 32
            return false;
        }
        if (strncmp(markerBuffer, "JEND", 4) != 0) {
            p_error = Error::IncorrectJendMarker; // 33
            return false;
        }

#ifdef RAGEPHOTO_BENCHMARK
        auto benchmark_parse_end = std::chrono::high_resolution_clock::now();
        auto benchmark_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(benchmark_parse_end - benchmark_parse_start);
        std::cout << "Benchmark: " << benchmark_ns.count() << "ns" << std::endl;
#endif

        p_error = Error::NoError; // 255
        return true;
    }
    p_error = Error::IncompatibleFormat; // 2
    return false;
}

RagePhoto::Error RagePhoto::error()
{
    return p_error;
}

const char* RagePhoto::photoData()
{
    if (p_photoLoaded)
        return p_photoData;
    else
        return nullptr;
}

const uint32_t RagePhoto::photoSize()
{
    if (p_photoLoaded)
        return p_photoSize;
    else
        return 0;
}

const std::string RagePhoto::description()
{
    return p_descriptionString;
}

const std::string RagePhoto::json()
{
    return p_jsonString;
}

const std::string RagePhoto::header()
{
    return p_photoString;
}

const std::string RagePhoto::title()
{
    return p_titleString;
}

size_t RagePhoto::readBuffer(const char *input, char *output, size_t *pos, size_t len)
{
#ifdef READ_USE_FOR
    for (size_t i = 0; i < len; i++) {
        output[i] = input[*pos+i];
    }
#else
    memcpy(output, &input[*pos], sizeof(char) * len);
#endif
    *pos = *pos + len;
    return len;
}

size_t RagePhoto::readBuffer(const char *input, char *output, size_t *pos, size_t len, size_t inputLen)
{
    size_t readLen = 0;
    if (*pos >= inputLen)
        return readLen;
    readLen = inputLen - *pos;
    if (readLen > len)
        readLen = len;
#ifdef READ_USE_FOR
    for (size_t i = 0; i < readLen; i++) {
        output[i] = input[*pos+i];
    }
#else
    memcpy(output, &input[*pos], sizeof(char) * readLen);
#endif
    *pos = *pos + readLen;
    return readLen;
}

uint32_t RagePhoto::charToUInt32BE(char *x)
{
    return (static_cast<unsigned char>(x[0]) << 24 |
            static_cast<unsigned char>(x[1]) << 16 |
            static_cast<unsigned char>(x[2]) << 8 |
            static_cast<unsigned char>(x[3]));
}

uint32_t RagePhoto::charToUInt32LE(char *x)
{
    return (static_cast<unsigned char>(x[3]) << 24 |
            static_cast<unsigned char>(x[2]) << 16 |
            static_cast<unsigned char>(x[1]) << 8 |
            static_cast<unsigned char>(x[0]));
}

void RagePhoto::uInt32ToCharBE(uint32_t x, char *y)
{
    y[0] = x >> 24;
    y[1] = x >> 16;
    y[2] = x >> 8;
    y[3] = x;
}

void RagePhoto::uInt32ToCharLE(uint32_t x, char *y)
{
    y[0] = x;
    y[1] = x >> 8;
    y[2] = x >> 16;
    y[3] = x >> 24;
}
