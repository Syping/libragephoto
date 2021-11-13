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
#ifdef LIBRAGEPHOTO_C_API
#include "RagePhotoC.h"
#endif

#include <fstream>
#include <iostream>
#include <iterator>
#include <cstdlib>
#include <cstring>

#ifdef RAGEPHOTO_BENCHMARK
#include <chrono>
#endif

#if defined UNICODE_CODECVT
#include <codecvt>
#include <locale>
#elif defined UNICODE_ICONV
#include <iconv.h>
#elif defined UNICODE_WINCVT
#include <windows.h>
#include <stringapiset.h>
#endif

RagePhoto::RagePhoto()
{
    m_data.photoLoaded = false;
    m_data.photoData = nullptr;
    setBufferDefault();
}

RagePhoto::~RagePhoto()
{
    free(m_data.photoData);
}

void RagePhoto::clear()
{
    if (m_data.photoLoaded) {
        free(m_data.photoData);
        m_data.photoData = nullptr;
        m_data.photoLoaded = false;
    }
    m_data.description.clear();
    m_data.json.clear();
    m_data.header.clear();
    m_data.title.clear();
    m_data.error = 0U;
    m_data.photoFormat = 0UL;
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
        m_data.error = static_cast<uint8_t>(Error::NoFormatIdentifier); // 1
        return false;
    }

#if __BYTE_ORDER == __LITTLE_ENDIAN
    memcpy(&m_data.photoFormat, uInt32Buffer, 4);
#else
    m_data.photoFormat = charToUInt32LE(uInt32Buffer);
#endif
    if (m_data.photoFormat == PhotoFormat::GTA5 || m_data.photoFormat == PhotoFormat::RDR2) {
#if defined UNICODE_ICONV || defined UNICODE_CODECVT || defined UNICODE_WINCVT
        char photoHeader[256];
        size = readBuffer(data, photoHeader, &pos, 256, length);
        if (size != 256) {
            m_data.error = static_cast<uint8_t>(Error::IncompleteHeader); // 3
            return false;
        }

#if defined UNICODE_CODECVT
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
        m_data.header = convert.to_bytes(reinterpret_cast<char16_t*>(photoHeader));
        if (convert.converted() == 0) {
            m_data.error = static_cast<uint8_t>(Error::UnicodeHeaderError); // 5
            return false;
        }
#elif defined UNICODE_ICONV
        iconv_t iconv_in = iconv_open("UTF-8", "UTF-16LE");
        if (iconv_in == (iconv_t)-1) {
            m_data.error = static_cast<uint8_t>(Error::UnicodeInitError); // 4
            return false;
        }
        char photoHeader_string[256];
        size_t src_s = sizeof(photoHeader);
        size_t dst_s = sizeof(photoHeader_string);
        char *src = photoHeader;
        char *dst = photoHeader_string;
        const size_t ret = iconv(iconv_in, &src, &src_s, &dst, &dst_s);
        iconv_close(iconv_in);
        if (ret == static_cast<size_t>(-1)) {
            m_data.error = static_cast<uint8_t>(Error::UnicodeHeaderError); // 5
            return false;
        }
        m_data.header = std::string(photoHeader_string);
#elif defined UNICODE_WINCVT
        char photoHeader_string[256];
        const int converted = WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<wchar_t*>(photoHeader), -1, photoHeader_string, 256, NULL, NULL);
        if (converted == 0) {
            m_data.error = static_cast<uint8_t>(Error::UnicodeHeaderError); // 5
            return false;
        }
        m_data.header = std::string(photoHeader_string);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data.error = static_cast<uint8_t>(Error::IncompleteChecksum); // 6
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&m_data.headerSum, uInt32Buffer, 4);
#else
        m_data.headerSum = charToUInt32LE(uInt32Buffer);
#endif

        if (m_data.photoFormat == PhotoFormat::RDR2)
            pos = pos + 8;
        const size_t headerSize = pos;

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data.error = static_cast<uint8_t>(Error::IncompleteEOF); // 7
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&m_data.endOfFile, uInt32Buffer, 4);
#else
        m_data.endOfFile = charToUInt32LE(uInt32Buffer);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data.error = static_cast<uint8_t>(Error::IncompleteJsonOffset); // 8
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&m_data.jsonOffset, uInt32Buffer, 4);
#else
        m_data.jsonOffset = charToUInt32LE(uInt32Buffer);
#endif
        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data.error = static_cast<uint8_t>(Error::IncompleteTitleOffset); // 9
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&m_data.titlOffset, uInt32Buffer, 4);
#else
        m_data.titlOffset = charToUInt32LE(uInt32Buffer);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data.error = static_cast<uint8_t>(Error::IncompleteDescOffset); // 10
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&m_data.descOffset, uInt32Buffer, 4);
#else
        m_data.descOffset = charToUInt32LE(uInt32Buffer);
#endif

        char markerBuffer[4];
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            m_data.error = static_cast<uint8_t>(Error::IncompleteJpegMarker); // 11
            return false;
        }
        if (strncmp(markerBuffer, "JPEG", 4) != 0) {
            m_data.error = static_cast<uint8_t>(Error::IncorrectJpegMarker); // 12
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data.error = static_cast<uint8_t>(Error::IncompletePhotoBuffer); // 13
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&m_data.photoBuffer, uInt32Buffer, 4);
#else
        m_data.photoBuffer = charToUInt32LE(uInt32Buffer);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data.error = static_cast<uint8_t>(Error::IncompletePhotoSize); // 14
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&m_data.photoSize, uInt32Buffer, 4);
#else
        m_data.photoSize = charToUInt32LE(uInt32Buffer);
#endif

        m_data.photoData = static_cast<char*>(malloc(m_data.photoSize));
        if (!m_data.photoData) {
            m_data.error = static_cast<uint8_t>(Error::PhotoMallocError); // 15
            return false;
        }
        size = readBuffer(data, m_data.photoData, &pos, m_data.photoSize, length);
        if (size != m_data.photoSize) {
            free(m_data.photoData);
            m_data.photoData = nullptr;
            m_data.error = static_cast<uint8_t>(Error::PhotoReadError); // 16
            return false;
        }
        m_data.photoLoaded = true;

        pos = m_data.jsonOffset + headerSize;
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            m_data.error = static_cast<uint8_t>(Error::IncompleteJsonMarker); // 17
            return false;
        }
        if (strncmp(markerBuffer, "JSON", 4) != 0) {
            m_data.error = static_cast<uint8_t>(Error::IncorrectJsonMarker); // 18
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data.error = static_cast<uint8_t>(Error::IncompleteJsonBuffer); // 19
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&m_data.jsonBuffer, uInt32Buffer, 4);
#else
        m_data.jsonBuffer = charToUInt32LE(uInt32Buffer);
#endif

        char *t_jsonData = static_cast<char*>(malloc(m_data.jsonBuffer));
        if (!t_jsonData) {
            m_data.error = static_cast<uint8_t>(Error::JsonMallocError); // 20
            return false;
        }
        size = readBuffer(data, t_jsonData, &pos, m_data.jsonBuffer, length);
        if (size != m_data.jsonBuffer) {
            free(t_jsonData);
            m_data.error = static_cast<uint8_t>(Error::JsonReadError); // 21
            return false;
        }
        m_data.json = std::string(t_jsonData);
        free(t_jsonData);

        pos = m_data.titlOffset + headerSize;
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            m_data.error = static_cast<uint8_t>(Error::IncompleteTitleMarker); // 22
            return false;
        }
        if (strncmp(markerBuffer, "TITL", 4) != 0) {
            m_data.error = static_cast<uint8_t>(Error::IncorrectTitleMarker); // 23
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data.error = static_cast<uint8_t>(Error::IncompleteTitleBuffer); // 24
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&m_data.titlBuffer, uInt32Buffer, 4);
#else
        m_data.titlBuffer = charToUInt32LE(uInt32Buffer);
#endif

        char *t_titlData = static_cast<char*>(malloc(m_data.titlBuffer));
        if (!t_titlData) {
            m_data.error = static_cast<uint8_t>(Error::TitleMallocError); // 25
            return false;
        }
        size = readBuffer(data, t_titlData, &pos, m_data.titlBuffer, length);
        if (size != m_data.titlBuffer) {
            free(t_titlData);
            m_data.error = static_cast<uint8_t>(Error::TitleReadError); // 26
            return false;
        }
        m_data.title = std::string(t_titlData);
        free(t_titlData);

        pos = m_data.descOffset + headerSize;
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            m_data.error = static_cast<uint8_t>(Error::IncompleteDescMarker); // 27
            return false;
        }
        if (strncmp(markerBuffer, "DESC", 4) != 0) {
            m_data.error = static_cast<uint8_t>(Error::IncorrectDescMarker); // 28
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data.error = static_cast<uint8_t>(Error::IncompleteDescBuffer); // 29
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&m_data.descBuffer, uInt32Buffer, 4);
#else
        m_data.descBuffer = charToUInt32LE(uInt32Buffer);
#endif

        char *t_descData = static_cast<char*>(malloc(m_data.descBuffer));
        if (!t_descData) {
            m_data.error = static_cast<uint8_t>(Error::DescMallocError); // 30
            return false;
        }
        size = readBuffer(data, t_descData, &pos, m_data.descBuffer, length);
        if (size != m_data.descBuffer) {
            free(t_descData);
            m_data.error = static_cast<uint8_t>(Error::DescReadError); // 31
            return false;
        }
        m_data.description = std::string(t_descData);
        free(t_descData);

        pos = m_data.endOfFile + headerSize - 4;
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            m_data.error = static_cast<uint8_t>(Error::IncompleteJendMarker); // 32
            return false;
        }
        if (strncmp(markerBuffer, "JEND", 4) != 0) {
            m_data.error = static_cast<uint8_t>(Error::IncorrectJendMarker); // 33
            return false;
        }

#ifdef RAGEPHOTO_BENCHMARK
        auto benchmark_parse_end = std::chrono::high_resolution_clock::now();
        auto benchmark_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(benchmark_parse_end - benchmark_parse_start);
        std::cout << "Benchmark: " << benchmark_ns.count() << "ns" << std::endl;
#endif

#ifdef RAGEPHOTO_DEBUG
        std::cout << "header: " << m_data.header << std::endl;
        std::cout << "headerSum: " << m_data.headerSum << std::endl;
        std::cout << "photoBuffer: " << m_data.photoBuffer << std::endl;
        std::cout << "descBuffer: " << m_data.descBuffer << std::endl;
        std::cout << "descOffset: " << m_data.descOffset << std::endl;
        std::cout << "jsonBuffer: " << m_data.jsonBuffer << std::endl;
        std::cout << "jsonOffset: " << m_data.jsonOffset << std::endl;
        std::cout << "titlBuffer: " << m_data.titlBuffer << std::endl;
        std::cout << "titlOffset: " << m_data.titlOffset << std::endl;
        std::cout << "eofOffset: " << m_data.endOfFile << std::endl;
        std::cout << "setBufferOffsets()" << std::endl;
        setBufferOffsets();
        std::cout << "descOffset: " << m_data.descOffset << std::endl;
        std::cout << "jsonOffset: " << m_data.jsonOffset << std::endl;
        std::cout << "titlOffset: " << m_data.titlOffset << std::endl;
        std::cout << "eofOffset: " << m_data.endOfFile << std::endl;
        std::cout << "calc size: " << saveSize() << std::endl;
        std::cout << "real size: " << length << std::endl;
#endif

        m_data.error = static_cast<uint8_t>(Error::NoError); // 255
        return true;
#else
        std::cout << "UTF-16LE decoding support missing" << std::endl;
        m_data.error = Error::UnicodeInitError; // 4
        return false;
#endif
    }
    else {
        auto it = m_loadFuncs.find(m_data.photoFormat);
        if (it != m_loadFuncs.end())
            return it->second(data, length, &m_data);
    }
    m_data.error = static_cast<uint8_t>(Error::IncompatibleFormat); // 2
    return false;
}

bool RagePhoto::load(const std::string &data)
{
    return load(data.data(), data.size());
}

bool RagePhoto::loadFile(const std::string &filename)
{
    std::ifstream ifs(filename, std::ios::in | std::ios::binary);
    if (ifs.is_open()) {
        std::string sdata(std::istreambuf_iterator<char>{ifs}, {});
        ifs.close();
        return load(sdata);
    }
    else {
        m_data.error = static_cast<uint8_t>(Error::Uninitialised); // 0
        return false;
    }
}

RagePhoto::Error RagePhoto::error() const
{
    return static_cast<Error>(m_data.error);
}

uint32_t RagePhoto::format() const
{
    return m_data.photoFormat;
}

const std::string RagePhoto::photo() const
{
    if (m_data.photoLoaded)
        return std::string(m_data.photoData, m_data.photoSize);
    else
        return std::string();
}

const char* RagePhoto::photoData() const
{
    if (m_data.photoLoaded)
        return m_data.photoData;
    else
        return nullptr;
}

uint32_t RagePhoto::photoSize() const
{
    if (m_data.photoLoaded)
        return m_data.photoSize;
    else
        return 0UL;
}

const std::string& RagePhoto::description() const
{
    return m_data.description;
}

const std::string& RagePhoto::json() const
{
    return m_data.json;
}

const std::string& RagePhoto::header() const
{
    return m_data.header;
}

const std::string& RagePhoto::title() const
{
    return m_data.title;
}

const char* RagePhoto::version()
{
    return RAGEPHOTO_VERSION;
}

bool RagePhoto::save(char *data, uint32_t photoFormat)
{
    if (photoFormat == PhotoFormat::GTA5 || photoFormat == PhotoFormat::RDR2) {
#if defined UNICODE_ICONV || defined UNICODE_CODECVT || defined UNICODE_WINCVT
#if defined UNICODE_CODECVT
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
        std::u16string photoHeader_string = convert.from_bytes(m_data.header);
        if (convert.converted() == 0) {
            m_data.error = static_cast<uint8_t>(Error::UnicodeHeaderError); // 5
            return false;
        }
        const size_t photoHeader_size = photoHeader_string.size() * 2;
        if (photoHeader_size > 256) {
            m_data.error = static_cast<uint8_t>(Error::HeaderBufferTight); // 34
            return false;
        }
        const char *photoHeader = reinterpret_cast<const char*>(photoHeader_string.data());
#elif defined UNICODE_ICONV
        iconv_t iconv_in = iconv_open("UTF-16LE", "UTF-8");
        if (iconv_in == (iconv_t)-1) {
            m_data.error = static_cast<uint8_t>(Error::UnicodeInitError); // 4
            return false;
        }
        char photoHeader_string[256];
        memcpy(photoHeader_string, m_data.header.data(), m_data.header.size());
        char photoHeader[256]{};
        size_t src_s = m_data.header.size();
        size_t dst_s = sizeof(photoHeader);
        char *src = photoHeader_string;
        char *dst = photoHeader;
        const size_t ret = iconv(iconv_in, &src, &src_s, &dst, &dst_s);
        iconv_close(iconv_in);
        if (ret == static_cast<size_t>(-1)) {
            m_data.error = static_cast<uint8_t>(Error::UnicodeHeaderError); // 5
            return false;
        }
        const size_t photoHeader_size = 256;
#elif defined UNICODE_WINCVT
        char photoHeader[256]{};
        const int converted = MultiByteToWideChar(CP_UTF8, 0, m_data.header.data(), m_data.header.size(), reinterpret_cast<wchar_t*>(photoHeader), 256 / sizeof(wchar_t));
        if (converted == 0) {
            m_data.error = static_cast<uint8_t>(Error::UnicodeHeaderError); // 5
            return false;
        }
        const size_t photoHeader_size = 256;
#endif

        if (m_data.photoSize > m_data.photoBuffer) {
            m_data.error = static_cast<uint8_t>(Error::PhotoBufferTight); // 35
            return false;
        }

        const size_t jsonString_size = m_data.json.size() + 1;
        if (jsonString_size > m_data.jsonBuffer) {
            m_data.error = static_cast<uint8_t>(Error::JsonBufferTight); // 36
            return false;
        }

        const size_t titlString_size = m_data.title.size() + 1;
        if (titlString_size > m_data.titlBuffer) {
            m_data.error = static_cast<uint8_t>(Error::TitleBufferTight); // 37
            return false;
        }

        const size_t descString_size = m_data.description.size() + 1;
        if (descString_size > m_data.descBuffer) {
            m_data.error = static_cast<uint8_t>(Error::DescBufferTight); // 38
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
        memcpy(uInt32Buffer, &m_data.headerSum, 4);
#else
        uInt32ToCharLE(m_data.headerSum, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        if (photoFormat == PhotoFormat::RDR2) {
            for (size_t i = 0; i < 8; i++) {
                writeBuffer("\0", data, &pos, length, 1);
            }
        }
        const size_t headerSize = pos;

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &m_data.endOfFile, 4);
#else
        uInt32ToCharLE(m_data.endOfFile, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &m_data.jsonOffset, 4);
#else
        uInt32ToCharLE(m_data.jsonOffset, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &m_data.titlOffset, 4);
#else
        uInt32ToCharLE(m_data.titlOffset, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &m_data.descOffset, 4);
#else
        uInt32ToCharLE(m_data.descOffset, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer("JPEG", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &m_data.photoBuffer, 4);
#else
        uInt32ToCharLE(m_data.photoBuffer, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &m_data.photoSize, 4);
#else
        uInt32ToCharLE(m_data.photoSize, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer(m_data.photoData, data, &pos, length, m_data.photoSize);
        for (size_t i = m_data.photoSize; i < m_data.photoBuffer; i++) {
            writeBuffer("\0", data, &pos, length, 1);
        }

        pos = m_data.jsonOffset + headerSize;
        writeBuffer("JSON", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &m_data.jsonBuffer, 4);
#else
        uInt32ToCharLE(m_data.jsonBuffer, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer(m_data.json.c_str(), data, &pos, length, jsonString_size);
        for (size_t i = jsonString_size; i < m_data.jsonBuffer; i++) {
            writeBuffer("\0", data, &pos, length, 1);
        }

        pos = m_data.titlOffset + headerSize;
        writeBuffer("TITL", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &m_data.titlBuffer, 4);
#else
        uInt32ToCharLE(m_data.titlBuffer, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer(m_data.title.c_str(), data, &pos, length, titlString_size);
        for (size_t i = titlString_size; i < m_data.titlBuffer; i++) {
            writeBuffer("\0", data, &pos, length, 1);
        }

        pos = m_data.descOffset + headerSize;
        writeBuffer("DESC", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &m_data.descBuffer, 4);
#else
        uInt32ToCharLE(m_data.descBuffer, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer(m_data.description.c_str(), data, &pos, length, descString_size);
        for (size_t i = descString_size; i < m_data.descBuffer; i++) {
            writeBuffer("\0", data, &pos, length, 1);
        }

        pos = m_data.endOfFile + headerSize - 4;
        writeBuffer("JEND", data, &pos, length, 4);

        m_data.error = static_cast<uint8_t>(Error::NoError); // 255
        return true;
#else
        std::cout << "UTF-16LE encoding support missing" << std::endl;
        m_data.error = Error::UnicodeInitError; // 4
        return false;
#endif
    }

    m_data.error = static_cast<uint8_t>(Error::IncompatibleFormat); // 2
    return false;
}

bool RagePhoto::save(char *data)
{
    return save(data, m_data.photoFormat);
}

const std::string RagePhoto::save(uint32_t photoFormat, bool *ok)
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
    return save(m_data.photoFormat, ok);
}

bool RagePhoto::saveFile(const std::string &filename, uint32_t photoFormat)
{
    bool ok;
    const std::string sdata = save(photoFormat, &ok);
    if (ok) {
        std::ofstream ofs(filename, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!ofs.is_open()) {
            m_data.error = static_cast<uint8_t>(Error::Uninitialised); // 0
            return false;
        }
        ofs << sdata;
        ok = ofs.good();
        ofs.close();
        return ok;
    }
    else
        return false;
}

bool RagePhoto::saveFile(const std::string &filename)
{
    return saveFile(filename, m_data.photoFormat);
}

inline size_t RagePhoto::saveSize(RagePhotoData *ragePhotoData, uint32_t photoFormat)
{
    if (photoFormat == PhotoFormat::GTA5)
        return (ragePhotoData->photoBuffer + ragePhotoData->jsonBuffer + ragePhotoData->titlBuffer + ragePhotoData->descBuffer + GTA5_HEADERSIZE + 56UL);
    else if (photoFormat == PhotoFormat::RDR2)
        return (ragePhotoData->photoBuffer + ragePhotoData->jsonBuffer + ragePhotoData->titlBuffer + ragePhotoData->descBuffer + RDR2_HEADERSIZE + 56UL);
    else
        return 0;
}

inline size_t RagePhoto::saveSize(RagePhotoData *ragePhotoData)
{
    return saveSize(ragePhotoData, ragePhotoData->photoFormat);
}

inline size_t RagePhoto::saveSize(uint32_t photoFormat)
{
    return saveSize(&m_data, photoFormat);
}

inline size_t RagePhoto::saveSize()
{
    return saveSize(m_data.photoFormat);
}

inline void RagePhoto::setBufferDefault()
{
    setBufferDefault(&m_data);
}

inline void RagePhoto::setBufferDefault(RagePhotoData *ragePhotoData)
{
    ragePhotoData->descBuffer = DEFAULT_DESCBUFFER;
    ragePhotoData->jsonBuffer = DEFAULT_JSONBUFFER;
    ragePhotoData->titlBuffer = DEFAULT_TITLBUFFER;
    setBufferOffsets(ragePhotoData);
}

inline void RagePhoto::setBufferOffsets()
{
    setBufferOffsets(&m_data);
}

inline void RagePhoto::setBufferOffsets(RagePhotoData *ragePhotoData)
{
    ragePhotoData->jsonOffset = ragePhotoData->photoBuffer + 28;
    ragePhotoData->titlOffset = ragePhotoData->jsonOffset + ragePhotoData->jsonBuffer + 8;
    ragePhotoData->descOffset = ragePhotoData->titlOffset + ragePhotoData->titlBuffer + 8;
    ragePhotoData->endOfFile = ragePhotoData->descOffset + ragePhotoData->descBuffer + 12;
}

void RagePhoto::setDescription(const std::string &description, uint32_t bufferSize)
{
    m_data.description = description;
    if (bufferSize != 0) {
        m_data.descBuffer = bufferSize;
        setBufferOffsets();
    }
}

void RagePhoto::setFormat(uint32_t photoFormat)
{
    m_data.photoFormat = photoFormat;
}

void RagePhoto::setFormatLoadFunction(uint32_t photoFormat, RagePhotoLoadFunc func)
{
    m_loadFuncs[photoFormat] = func;
}

void RagePhoto::setJson(const std::string &json, uint32_t bufferSize)
{
    m_data.json = json;
    if (bufferSize != 0) {
        m_data.jsonBuffer = bufferSize;
        setBufferOffsets();
    }
}

void RagePhoto::setHeader(const std::string &header, uint32_t headerSum)
{
    m_data.header = header;
    m_data.headerSum = headerSum;
}

bool RagePhoto::setPhoto(const char *data, uint32_t size, uint32_t bufferSize)
{
    if (m_data.photoLoaded) {
        if (m_data.photoSize > size) {
            char *t_photoData = static_cast<char*>(realloc(m_data.photoData, size));
            if (!t_photoData) {
                m_data.error = static_cast<uint8_t>(Error::PhotoMallocError); // 15
                return false;
            }
            m_data.photoData = t_photoData;
            memcpy(m_data.photoData, data, size);
            m_data.photoSize = size;
        }
        else if (m_data.photoSize < size) {
            free(m_data.photoData);
            m_data.photoData = static_cast<char*>(malloc(size));
            if (!m_data.photoData) {
                m_data.error = static_cast<uint8_t>(Error::PhotoMallocError); // 15
                m_data.photoLoaded = false;
                return false;
            }
            memcpy(m_data.photoData, data, size);
            m_data.photoSize = size;
        }
        else {
            memcpy(m_data.photoData, data, size);
        }
    }
    else {
        m_data.photoData = static_cast<char*>(malloc(size));
        if (!m_data.photoData) {
            m_data.error = static_cast<uint8_t>(Error::PhotoMallocError); // 15
            return false;
        }
        memcpy(m_data.photoData, data, size);
        m_data.photoLoaded = true;
        m_data.photoSize = size;
    }

    if (bufferSize != 0) {
        m_data.photoBuffer = bufferSize;
        setBufferOffsets();
    }

    m_data.error = static_cast<uint8_t>(Error::NoError); // 255
    return true;
}

bool RagePhoto::setPhoto(const std::string &data, uint32_t bufferSize)
{
    return setPhoto(data.data(), static_cast<uint32_t>(data.size()), bufferSize);
}

void RagePhoto::setTitle(const std::string &title, uint32_t bufferSize)
{
    m_data.title = title;
    if (bufferSize != 0) {
        m_data.titlBuffer = bufferSize;
        setBufferOffsets();
    }
}

inline size_t RagePhoto::readBuffer(const char *input, void *output, size_t *pos, size_t len, size_t inputLen)
{
    size_t readLen = 0;
    if (*pos >= inputLen)
        return 0;
    readLen = inputLen - *pos;
    if (readLen > len)
        readLen = len;
    memcpy(output, reinterpret_cast<const void*>(&input[*pos]), readLen);
    *pos = *pos + readLen;
    return readLen;
}

inline size_t RagePhoto::writeBuffer(const void *input, char *output, size_t *pos, size_t len, size_t inputLen)
{
    const size_t maxLen = len - *pos;
    size_t writeLen = inputLen;
    if (*pos >= len)
        return 0;
    if (inputLen > maxLen)
        writeLen = maxLen;
    memcpy(reinterpret_cast<void*>(&output[*pos]), input, writeLen);
    *pos = *pos + writeLen;
    return writeLen;
}

inline uint32_t RagePhoto::charToUInt32LE(char *x)
{
    return (static_cast<unsigned char>(x[3]) << 24 |
            static_cast<unsigned char>(x[2]) << 16 |
            static_cast<unsigned char>(x[1]) << 8 |
            static_cast<unsigned char>(x[0]));
}

inline void RagePhoto::uInt32ToCharLE(uint32_t x, char *y)
{
    y[0] = x;
    y[1] = x >> 8;
    y[2] = x >> 16;
    y[3] = x >> 24;
}

#ifdef LIBRAGEPHOTO_C_API
ragephoto_t ragephoto_open()
{
    return static_cast<ragephoto_t>(new RagePhoto);
}

void ragephoto_clear(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    ragePhoto->clear();
}

int ragephoto_load(ragephoto_t instance, const char *data, size_t size)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->load(data, size);
}

int ragephoto_loadfile(ragephoto_t instance, const char *filename)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->loadFile(filename);
}

uint8_t ragephoto_error(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return static_cast<uint8_t>(ragePhoto->error());
}

uint32_t ragephoto_defpbuf_gta5()
{
    return RagePhoto::DEFAULT_GTA5_PHOTOBUFFER;
}

uint32_t ragephoto_defpbuf_rdr2()
{
    return RagePhoto::DEFAULT_RDR2_PHOTOBUFFER;
}

uint32_t ragephoto_format_gta5()
{
    return RagePhoto::GTA5;
}

uint32_t ragephoto_format_rdr2()
{
    return RagePhoto::RDR2;
}

const char* ragephoto_getphotodesc(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->description().c_str();
}

uint32_t ragephoto_getphotoformat(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->format();
}

const char* ragephoto_getphotojpeg(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->photoData();
}

const char* ragephoto_getphotojson(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->json().c_str();
}

const char* ragephoto_getphotoheader(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->header().c_str();
}

uint32_t ragephoto_getphotosize(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->photoSize();
}

const char* ragephoto_getphototitle(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->title().c_str();
}

size_t ragephoto_getsavesize(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->saveSize();
}

size_t ragephoto_getsavesizef(ragephoto_t instance, uint32_t photoFormat)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->saveSize(photoFormat);
}

int ragephoto_save(ragephoto_t instance, char *data)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->save(data);
}

int ragephoto_savef(ragephoto_t instance, char *data, uint32_t photoFormat)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->save(data, photoFormat);
}

int ragephoto_savefile(ragephoto_t instance, const char *filename)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->saveFile(filename);
}

int ragephoto_savefilef(ragephoto_t instance, const char *filename, uint32_t photoFormat)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->saveFile(filename, photoFormat);
}

void ragephoto_setbufferdefault(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    ragePhoto->setBufferDefault();
}

void ragephoto_setbufferoffsets(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    ragePhoto->setBufferOffsets();
}

void ragephoto_setphotodesc(ragephoto_t instance, const char *description, uint32_t bufferSize)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    ragePhoto->setDescription(description, bufferSize);
}

void ragephoto_setphotoformat(ragephoto_t instance, uint32_t photoFormat)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    ragePhoto->setFormat(photoFormat);
}

int ragephoto_setphotojpeg(ragephoto_t instance, const char *data, uint32_t size, uint32_t bufferSize)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->setPhoto(data, size, bufferSize);
}

void ragephoto_setphotojson(ragephoto_t instance, const char *json, uint32_t bufferSize)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    ragePhoto->setJson(json, bufferSize);
}

void ragephoto_setphotoheader(ragephoto_t instance, const char *header, uint32_t headerSum)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    ragePhoto->setHeader(header, headerSum);
}

void ragephoto_setphototitle(ragephoto_t instance, const char *title, uint32_t bufferSize)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    ragePhoto->setTitle(title, bufferSize);
}

void ragephoto_close(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    delete ragePhoto;
}

const char* ragephoto_version()
{
    return RAGEPHOTO_VERSION;
}
#endif
