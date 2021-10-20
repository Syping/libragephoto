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

#ifdef CODECVT_COMPATIBLE
#include <codecvt>
#include <locale>
#elif defined ICONV_COMPATIBLE
#include <iconv.h>
#endif

RagePhoto::RagePhoto()
{
    p_photoLoaded = false;
    p_photoData = nullptr;
    setBufferDefault();
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
    p_photoFormat = PhotoFormat::Undefined;
    setBufferDefault();
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

#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint32_t format;
    memcpy(&format, uInt32Buffer, 4);
#else
    uint32_t format = charToUInt32LE(uInt32Buffer);
#endif
    if (format == static_cast<uint32_t>(PhotoFormat::GTA5) || format == static_cast<uint32_t>(PhotoFormat::RDR2)) {
#if defined CODECVT_COMPATIBLE || defined ICONV_COMPATIBLE
        p_photoFormat = static_cast<PhotoFormat>(format);

        char photoHeader[256];
        size = readBuffer(data, photoHeader, &pos, 256, length);
        if (size != 256) {
            p_error = Error::IncompleteHeader; // 3
            return false;
        }

#ifdef CODECVT_COMPATIBLE
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, std::little_endian>,char16_t> convert;
        p_photoString = convert.to_bytes(reinterpret_cast<char16_t*>(photoHeader));
#elif defined ICONV_COMPATIBLE
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
        const size_t ret = iconv(iconv_in, &src, &src_s, &dst, &dst_s);
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
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&p_headerSum, uInt32Buffer, 4);
#else
        p_headerSum = charToUInt32LE(uInt32Buffer);
#endif

        if (p_photoFormat == PhotoFormat::RDR2)
            pos = pos + 8;
        const size_t headerSize = pos;

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteEOF; // 7
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&p_endOfFile, uInt32Buffer, 4);
#else
        p_endOfFile = charToUInt32LE(uInt32Buffer);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteJsonOffset; // 8
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&p_jsonOffset, uInt32Buffer, 4);
#else
        p_jsonOffset = charToUInt32LE(uInt32Buffer);
#endif
        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteTitleOffset; // 9
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&p_titlOffset, uInt32Buffer, 4);
#else
        p_titlOffset = charToUInt32LE(uInt32Buffer);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompleteDescOffset; // 10
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&p_descOffset, uInt32Buffer, 4);
#else
        p_descOffset = charToUInt32LE(uInt32Buffer);
#endif

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
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&p_photoBuffer, uInt32Buffer, 4);
#else
        p_photoBuffer = charToUInt32LE(uInt32Buffer);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            p_error = Error::IncompletePhotoSize; // 14
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&p_photoSize, uInt32Buffer, 4);
#else
        p_photoSize = charToUInt32LE(uInt32Buffer);
#endif

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

        pos = p_jsonOffset + headerSize;
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
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&p_jsonBuffer, uInt32Buffer, 4);
#else
        p_jsonBuffer = charToUInt32LE(uInt32Buffer);
#endif

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

        pos = p_titlOffset + headerSize;
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
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&p_titlBuffer, uInt32Buffer, 4);
#else
        p_titlBuffer = charToUInt32LE(uInt32Buffer);
#endif

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

        pos = p_descOffset + headerSize;
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
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&p_descBuffer, uInt32Buffer, 4);
#else
        p_descBuffer = charToUInt32LE(uInt32Buffer);
#endif

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

        pos = p_endOfFile + headerSize - 4;
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

#ifdef RAGEPHOTO_DEBUG
        std::cout << "header: " << p_photoString << std::endl;
        std::cout << "headerSum: " << p_headerSum << std::endl;
        std::cout << "photoBuffer: " << p_photoBuffer << std::endl;
        std::cout << "descBuffer: " << p_descBuffer << std::endl;
        std::cout << "descOffset: " << p_descOffset << std::endl;
        std::cout << "jsonBuffer: " << p_jsonBuffer << std::endl;
        std::cout << "jsonOffset: " << p_jsonOffset << std::endl;
        std::cout << "titlBuffer: " << p_titlBuffer << std::endl;
        std::cout << "titlOffset: " << p_titlOffset << std::endl;
        std::cout << "eofOffset: " << p_endOfFile << std::endl;
        std::cout << "moveOffsets()" << std::endl;
        moveOffsets();
        std::cout << "descOffset: " << p_descOffset << std::endl;
        std::cout << "jsonOffset: " << p_jsonOffset << std::endl;
        std::cout << "titlOffset: " << p_titlOffset << std::endl;
        std::cout << "eofOffset: " << p_endOfFile << std::endl;
        std::cout << "calc size: " << saveSize() << std::endl;
        std::cout << "real size: " << length << std::endl;
#endif

        p_error = Error::NoError; // 255
        return true;
#else
        std::cout << "UTF-16LE decoding support missing" << std::endl;
        p_error = Error::UnicodeInitError; // 4
        return false;
#endif
    }
    p_error = Error::IncompatibleFormat; // 2
    return false;
}

bool RagePhoto::load(const std::string &data)
{
    return load(data.data(), data.size());
}

RagePhoto::Error RagePhoto::error()
{
    return p_error;
}

RagePhoto::PhotoFormat RagePhoto::format()
{
    return p_photoFormat;
}

const std::string RagePhoto::photo()
{
    if (p_photoLoaded)
        return std::string(p_photoData, p_photoSize);
    else
        return std::string();
}

const char* RagePhoto::photoData()
{
    if (p_photoLoaded)
        return p_photoData;
    else
        return nullptr;
}

uint32_t RagePhoto::photoSize()
{
    if (p_photoLoaded)
        return p_photoSize;
    else
        return 0UL;
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

bool RagePhoto::save(char *data, PhotoFormat photoFormat)
{
    if (photoFormat == PhotoFormat::GTA5 || photoFormat == PhotoFormat::RDR2) {
#if defined CODECVT_COMPATIBLE || defined ICONV_COMPATIBLE
#ifdef CODECVT_COMPATIBLE
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, std::little_endian>,char16_t> convert;
        std::u16string photoString = convert.from_bytes(p_photoString);
        const size_t photoHeader_size = photoString.size() * 2;
        if (photoHeader_size > 256) {
            p_error = Error::HeaderBufferTight; // 34
            return false;
        }
        char photoHeader[256];
        memcpy(photoHeader, photoString.data(), photoHeader_size);
#elif defined ICONV_COMPATIBLE
        iconv_t iconv_in = iconv_open("UTF-16LE", "UTF-8");
        if (iconv_in == (iconv_t)-1) {
            p_error = Error::UnicodeInitError; // 4
            return false;
        }
        char photoString[256];
        memcpy(photoString, p_photoString.data(), p_photoString.size());
        char photoHeader[256];
        size_t src_s = p_photoString.size();
        size_t dst_s = sizeof(photoHeader);
        char *src = photoString;
        char *dst = photoHeader;
        const size_t ret = iconv(iconv_in, &src, &src_s, &dst, &dst_s);
        iconv_close(iconv_in);
        if (ret == static_cast<size_t>(-1)) {
            p_error = Error::UnicodeHeaderError; // 5
            return false;
        }
        const size_t photoHeader_size = p_photoString.size() * 2;
        if (photoHeader_size > 256) {
            p_error = Error::HeaderBufferTight; // 34
            return false;
        }
#endif

        if (p_photoSize > p_photoBuffer) {
            p_error = Error::PhotoBufferTight; // 35
            return false;
        }

        const size_t jsonString_size = p_jsonString.size() + 1;
        if (jsonString_size > p_jsonBuffer) {
            p_error = Error::JsonBufferTight; // 36
            return false;
        }

        const size_t titlString_size = p_titleString.size() + 1;
        if (titlString_size > p_titlBuffer) {
            p_error = Error::TitleBufferTight; // 37
            return false;
        }

        const size_t descString_size = p_descriptionString.size() + 1;
        if (descString_size > p_descBuffer) {
            p_error = Error::DescBufferTight; // 38
            return false;
        }

        const size_t length = saveSize(photoFormat);
        size_t pos = 0;
        char uInt32Buffer[4];

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &photoFormat, 4);
#else
        uInt32ToCharLE(static_cast<uint32_t>(photoFormat), uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer(photoHeader, data, &pos, length, photoHeader_size);
        for (size_t i = photoHeader_size; i < 256; i++) {
            writeBuffer("\0", data, &pos, length, 1);
        }

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &p_headerSum, 4);
#else
        uInt32ToCharLE(p_headerSum, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        if (photoFormat == PhotoFormat::RDR2) {
            for (size_t i = 0; i < 8; i++) {
                writeBuffer("\0", data, &pos, length, 1);
            }
        }
        const size_t headerSize = pos;

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &p_endOfFile, 4);
#else
        uInt32ToCharLE(p_endOfFile, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &p_jsonOffset, 4);
#else
        uInt32ToCharLE(p_jsonOffset, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &p_titlOffset, 4);
#else
        uInt32ToCharLE(p_titlOffset, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &p_descOffset, 4);
#else
        uInt32ToCharLE(p_descOffset, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer("JPEG", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &p_photoBuffer, 4);
#else
        uInt32ToCharLE(p_photoBuffer, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &p_photoSize, 4);
#else
        uInt32ToCharLE(p_photoSize, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer(p_photoData, data, &pos, length, p_photoSize);
        for (size_t i = p_photoSize; i < p_photoBuffer; i++) {
            writeBuffer("\0", data, &pos, length, 1);
        }

        pos = p_jsonOffset + headerSize;
        writeBuffer("JSON", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &p_jsonBuffer, 4);
#else
        uInt32ToCharLE(p_jsonBuffer, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer(p_jsonString.data(), data, &pos, length, jsonString_size);
        for (size_t i = jsonString_size; i < p_jsonBuffer; i++) {
            writeBuffer("\0", data, &pos, length, 1);
        }

        pos = p_titlOffset + headerSize;
        writeBuffer("TITL", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &p_titlBuffer, 4);
#else
        uInt32ToCharLE(p_titlBuffer, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer(p_titleString.data(), data, &pos, length, titlString_size);
        for (size_t i = titlString_size; i < p_titlBuffer; i++) {
            writeBuffer("\0", data, &pos, length, 1);
        }

        pos = p_descOffset + headerSize;
        writeBuffer("DESC", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &p_descBuffer, 4);
#else
        uInt32ToCharLE(p_descBuffer, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer(p_descriptionString.data(), data, &pos, length, descString_size);
        for (size_t i = descString_size; i < p_descBuffer; i++) {
            writeBuffer("\0", data, &pos, length, 1);
        }

        pos = p_endOfFile + headerSize - 4;
        writeBuffer("JEND", data, &pos, length, 4);

        p_error = Error::NoError; // 255
        return true;
#else
        std::cout << "UTF-16LE encoding support missing" << std::endl;
        p_error = Error::UnicodeInitError; // 4
        return false;
#endif
    }

    p_error = Error::IncompatibleFormat; // 2
    return false;
}

bool RagePhoto::save(char *data)
{
    return save(data, p_photoFormat);
}

const std::string RagePhoto::save(PhotoFormat photoFormat, bool *ok)
{
    const size_t size = saveSize(photoFormat);
    if (size == 0) {
        if (ok)
            *ok = false;
        return std::string();
    }
    char *data = static_cast<char*>(malloc(size));
    if (!data) {
        if (ok)
            *ok = false;
        return std::string();
    }
    const bool saved = save(data, photoFormat);
    if (ok)
        *ok = saved;
    const std::string sdata = std::string(data, size);
    free(data);
    return sdata;
}

const std::string RagePhoto::save(bool *ok)
{
    return save(p_photoFormat, ok);
}

size_t RagePhoto::saveSize(PhotoFormat photoFormat)
{
    if (photoFormat == PhotoFormat::GTA5)
        return (p_photoBuffer + p_jsonBuffer + p_titlBuffer + p_descBuffer + GTA5_HEADERSIZE + 56UL);
    else if (photoFormat == PhotoFormat::RDR2)
        return (p_photoBuffer + p_jsonBuffer + p_titlBuffer + p_descBuffer + RDR2_HEADERSIZE + 56UL);
    else
        return 0;
}

size_t RagePhoto::saveSize()
{
    return saveSize(p_photoFormat);
}

void RagePhoto::setBufferDefault()
{
    p_descBuffer = DEFAULT_DESCBUFFER;
    p_jsonBuffer = DEFAULT_JSONBUFFER;
    p_titlBuffer = DEFAULT_TITLBUFFER;
    moveOffsets();
}

void RagePhoto::setDescription(const std::string &description, uint32_t bufferSize)
{
    p_descriptionString = description;
    if (bufferSize != 0) {
        p_descBuffer = bufferSize;
        moveOffsets();
    }
}

void RagePhoto::setFormat(PhotoFormat photoFormat)
{
    p_photoFormat = photoFormat;
}

void RagePhoto::setJson(const std::string &json, uint32_t bufferSize)
{
    p_jsonString = json;
    if (bufferSize != 0) {
        p_jsonBuffer = bufferSize;
        moveOffsets();
    }
}

void RagePhoto::setHeader(const std::string &header, uint32_t headerSum)
{
    p_photoString = header;
    p_headerSum = headerSum;
}

bool RagePhoto::setPhoto(const char *data, uint32_t size, uint32_t bufferSize)
{
    if (p_photoLoaded) {
        if (p_photoSize > size) {
            char *t_photoData = static_cast<char*>(realloc(p_photoData, size));
            if (!t_photoData) {
                p_error = Error::PhotoMallocError; // 15
                return false;
            }
            p_photoData = t_photoData;
            memcpy(p_photoData, data, size);
        }
        else if (p_photoSize < size) {
            free(p_photoData);
            p_photoData = static_cast<char*>(malloc(size));
            if (!p_photoData) {
                p_error = Error::PhotoMallocError; // 15
                p_photoLoaded = false;
                return false;
            }
            memcpy(p_photoData, data, size);
        }
        else {
            memcpy(p_photoData, data, size);
        }
    }
    else {
        p_photoData = static_cast<char*>(malloc(size));
        if (!p_photoData) {
            p_error = Error::PhotoMallocError; // 15
            return false;
        }
        memcpy(p_photoData, data, size);
        p_photoLoaded = true;
    }

    if (bufferSize != 0) {
        p_photoBuffer = bufferSize;
        moveOffsets();
    }

    p_error = Error::NoError; // 255
    return true;
}

bool RagePhoto::setPhoto(const std::string &data, uint32_t bufferSize)
{
    return setPhoto(data.data(), static_cast<uint32_t>(data.size()), bufferSize);
}

void RagePhoto::setTitle(const std::string &title, uint32_t bufferSize)
{
    p_titleString = title;
    if (bufferSize != 0) {
        p_titlBuffer = bufferSize;
        moveOffsets();
    }
}

void RagePhoto::moveOffsets()
{
    p_jsonOffset = p_photoBuffer + 28;
    p_titlOffset = p_jsonOffset + p_jsonBuffer + 8;
    p_descOffset = p_titlOffset + p_titlBuffer + 8;
    p_endOfFile = p_descOffset + p_descBuffer + 12;
}

size_t RagePhoto::readBuffer(const char *input, char *output, size_t *pos, size_t len, size_t inputLen)
{
    size_t readLen = 0;
    if (*pos >= inputLen)
        return 0;
    readLen = inputLen - *pos;
    if (readLen > len)
        readLen = len;
    memcpy(output, &input[*pos], readLen);
    *pos = *pos + readLen;
    return readLen;
}

size_t RagePhoto::writeBuffer(const char *input, char *output, size_t *pos, size_t len, size_t inputLen)
{
    const size_t maxLen = len - *pos;
    size_t writeLen = inputLen;
    if (*pos >= len)
        return 0;
    if (inputLen > maxLen)
        writeLen = maxLen;
    memcpy(&output[*pos], input, writeLen);
    *pos = *pos + writeLen;
    return writeLen;
}

uint32_t RagePhoto::charToUInt32LE(char *x)
{
    return (static_cast<unsigned char>(x[3]) << 24 |
            static_cast<unsigned char>(x[2]) << 16 |
            static_cast<unsigned char>(x[1]) << 8 |
            static_cast<unsigned char>(x[0]));
}

void RagePhoto::uInt32ToCharLE(uint32_t x, char *y)
{
    y[0] = x;
    y[1] = x >> 8;
    y[2] = x >> 16;
    y[3] = x >> 24;
}
