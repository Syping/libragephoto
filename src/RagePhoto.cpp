/*****************************************************************************
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
*****************************************************************************/

#include "RagePhoto.h"
#ifdef LIBRAGEPHOTO_C_API
#include "RagePhotoC.h"
#endif

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>

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

/* BEGIN OF STATIC LIBRARY FUNCTIONS */
inline size_t readBuffer(const char *input, void *output, size_t *pos, size_t outputLen, size_t inputLen)
{
    size_t readLen = 0;
    if (*pos >= inputLen)
        return 0;
    readLen = inputLen - *pos;
    if (readLen > outputLen)
        readLen = outputLen;
    std::memcpy(output, &input[*pos], readLen);
    *pos = *pos + readLen;
    return readLen;
}

inline size_t writeBuffer(const void *input, char *output, size_t *pos, size_t outputLen, size_t inputLen)
{
    const size_t maxLen = outputLen - *pos;
    size_t writeLen = inputLen;
    if (*pos >= outputLen)
        return 0;
    if (inputLen > maxLen)
        writeLen = maxLen;
    std::memcpy(&output[*pos], input, writeLen);
    *pos = *pos + writeLen;
    return writeLen;
}

inline bool writeDataChar(const char *input, char **output)
{
    const size_t src_s = strlen(input) + 1;
    if (*output) {
        const size_t dst_s = strlen(*output) + 1;
        if (dst_s > src_s) {
            char *t_output = static_cast<char*>(std::realloc(*output, src_s));
            if (!t_output) {
                return false;
            }
            *output = t_output;
            std::memcpy(*output, input, src_s);
        }
        else if (dst_s < src_s) {
            char *t_output = static_cast<char*>(std::malloc(src_s));
            if (!t_output) {
                return false;
            }
            std::free(*output);
            *output = t_output;
            std::memcpy(*output, input, src_s);
        }
        else {
            std::memcpy(*output, input, src_s);
        }
    }
    else {
        char *t_output = static_cast<char*>(std::malloc(src_s));
        if (!t_output) {
            return false;
        }
        *output = t_output;
        std::memcpy(*output, input, src_s);
    }
    return true;
}

inline uint32_t charToUInt32LE(char *x)
{
    return (static_cast<unsigned char>(x[3]) << 24 |
            static_cast<unsigned char>(x[2]) << 16 |
            static_cast<unsigned char>(x[1]) << 8 |
            static_cast<unsigned char>(x[0]));
}

inline void uInt32ToCharLE(uint32_t x, char *y)
{
    y[0] = x;
    y[1] = x >> 8;
    y[2] = x >> 16;
    y[3] = x >> 24;
}
/* END OF STATIC LIBRARY FUNCTIONS */

/* BEGIN OF RAGEPHOTO CLASS */
RagePhoto::RagePhoto()
{
#if __cplusplus >= 201707L
    m_data = new RagePhotoData {
            .jpeg = nullptr,
            .description = nullptr,
            .json = nullptr,
            .title = nullptr,
            .error = 0,
            .photoFormat = 0,
            .unnamedSum1 = 0,
            .unnamedSum2 = 0
    };
#else
    m_data = new RagePhotoData { 0 };
#endif
    setBufferDefault();
}

RagePhoto::~RagePhoto()
{
    std::free(m_data->jpeg);
    std::free(m_data->description);
    std::free(m_data->json);
    std::free(m_data->header);
    std::free(m_data->title);
    delete m_data;
}

void RagePhoto::clear()
{
    std::free(m_data->jpeg);
    m_data->jpeg = nullptr;
    std::free(m_data->description);
    m_data->description = nullptr;
    std::free(m_data->json);
    m_data->json = nullptr;
    std::free(m_data->header);
    m_data->header = nullptr;
    std::free(m_data->title);
    m_data->title = nullptr;
    m_data->error = 0;
    m_data->photoFormat = 0;
    m_data->unnamedSum1 = 0;
    m_data->unnamedSum2 = 0;
    setBufferDefault();
}

RagePhotoData* RagePhoto::data()
{
    return m_data;
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
        m_data->error = Error::NoFormatIdentifier; // 1
        return false;
    }

#if __BYTE_ORDER == __LITTLE_ENDIAN
    std::memcpy(&m_data->photoFormat, uInt32Buffer, 4);
#else
    m_data->photoFormat = charToUInt32LE(uInt32Buffer);
#endif
    if (m_data->photoFormat == PhotoFormat::GTA5 || m_data->photoFormat == PhotoFormat::RDR2) {
#if defined UNICODE_ICONV || defined UNICODE_CODECVT || defined UNICODE_WINCVT
        char photoHeader[256];
        size = readBuffer(data, photoHeader, &pos, 256, length);
        if (size != 256) {
            m_data->error = Error::IncompleteHeader; // 3
            return false;
        }

#if defined UNICODE_CODECVT
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
        const std::string photoHeader_string = convert.to_bytes(reinterpret_cast<char16_t*>(photoHeader));
        if (convert.converted() == 0) {
            m_data->error = Error::UnicodeHeaderError; // 6
            return false;
        }
        const size_t photoHeader_size = photoHeader_string.size() + 1;
        m_data->header = static_cast<char*>(std::malloc(photoHeader_size));
        if (!m_data->header) {
            m_data->error = Error::HeaderMallocError; // 4
            return false;
        }
        std::memcpy(m_data->header, photoHeader_string.c_str(), photoHeader_size);
#elif defined UNICODE_ICONV
        iconv_t iconv_in = iconv_open("UTF-8", "UTF-16LE");
        if (iconv_in == (iconv_t)-1) {
            m_data->error = Error::UnicodeInitError; // 4
            return false;
        }
        m_data->header = static_cast<char*>(std::malloc(256));
        if (!m_data->header) {
            m_data->error = Error::HeaderMallocError; // 4
            iconv_close(iconv_in);
            return false;
        }
        size_t src_s = sizeof(photoHeader);
        size_t dst_s = 256;
        char *src = photoHeader;
        char *dst = m_data->header;
        const size_t ret = iconv(iconv_in, &src, &src_s, &dst, &dst_s);
        iconv_close(iconv_in);
        if (ret == static_cast<size_t>(-1)) {
            m_data->error = Error::UnicodeHeaderError; // 6
            return false;
        }
#elif defined UNICODE_WINCVT
        m_data->header = static_cast<char*>(std::malloc(256));
        if (!m_data->header) {
            m_data->error = Error::HeaderMallocError; // 4
            return false;
        }
        const int converted = WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<wchar_t*>(photoHeader), -1, m_data->header, 256, NULL, NULL);
        if (converted == 0) {
            std::free(m_data->header);
            m_data->header = nullptr;
            m_data->error = Error::UnicodeHeaderError; // 6
            return false;
        }
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data->error = Error::IncompleteChecksum; // 7
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(&m_data->headerSum, uInt32Buffer, 4);
#else
        m_data->headerSum = charToUInt32LE(uInt32Buffer);
#endif

        if (m_data->photoFormat == PhotoFormat::RDR2) {
            size = readBuffer(data, uInt32Buffer, &pos, 4, length);
            if (size != 4) {
                m_data->error = Error::IncompleteChecksum; // 7
                return false;
            }
#if __BYTE_ORDER == __LITTLE_ENDIAN
            std::memcpy(&m_data->unnamedSum1, uInt32Buffer, 4);
#else
            m_data->unnamedSum1 = charToUInt32LE(uInt32Buffer);
#endif

            size = readBuffer(data, uInt32Buffer, &pos, 4, length);
            if (size != 4) {
                m_data->error = Error::IncompleteChecksum; // 7
                return false;
            }
#if __BYTE_ORDER == __LITTLE_ENDIAN
            std::memcpy(&m_data->unnamedSum2, uInt32Buffer, 4);
#else
            m_data->unnamedSum2 = charToUInt32LE(uInt32Buffer);
#endif
        }
        const size_t headerSize = pos;

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data->error = Error::IncompleteEOF; // 8
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(&m_data->endOfFile, uInt32Buffer, 4);
#else
        m_data->endOfFile = charToUInt32LE(uInt32Buffer);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data->error = Error::IncompleteJsonOffset; // 9
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(&m_data->jsonOffset, uInt32Buffer, 4);
#else
        m_data->jsonOffset = charToUInt32LE(uInt32Buffer);
#endif
        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data->error = Error::IncompleteTitleOffset; // 10
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(&m_data->titlOffset, uInt32Buffer, 4);
#else
        m_data->titlOffset = charToUInt32LE(uInt32Buffer);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data->error = Error::IncompleteDescOffset; // 11
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(&m_data->descOffset, uInt32Buffer, 4);
#else
        m_data->descOffset = charToUInt32LE(uInt32Buffer);
#endif

        char markerBuffer[4];
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            m_data->error = Error::IncompleteJpegMarker; // 12
            return false;
        }
        if (strncmp(markerBuffer, "JPEG", 4) != 0) {
            m_data->error = Error::IncorrectJpegMarker; // 13
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data->error = Error::IncompletePhotoBuffer; // 14
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(&m_data->photoBuffer, uInt32Buffer, 4);
#else
        m_data->photoBuffer = charToUInt32LE(uInt32Buffer);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data->error = Error::IncompletePhotoSize; // 15
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(&m_data->jpegSize, uInt32Buffer, 4);
#else
        m_data->jpegSize = charToUInt32LE(uInt32Buffer);
#endif

        m_data->jpeg = static_cast<char*>(std::malloc(m_data->jpegSize));
        if (!m_data->jpeg) {
            m_data->error = Error::PhotoMallocError; // 16
            return false;
        }
        size = readBuffer(data, m_data->jpeg, &pos, m_data->jpegSize, length);
        if (size != m_data->jpegSize) {
            std::free(m_data->jpeg);
            m_data->jpeg = nullptr;
            m_data->error = Error::PhotoReadError; // 17
            return false;
        }

        pos = m_data->jsonOffset + headerSize;
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            m_data->error = Error::IncompleteJsonMarker; // 18
            return false;
        }
        if (strncmp(markerBuffer, "JSON", 4) != 0) {
            m_data->error = Error::IncorrectJsonMarker; // 19
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data->error = Error::IncompleteJsonBuffer; // 20
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(&m_data->jsonBuffer, uInt32Buffer, 4);
#else
        m_data->jsonBuffer = charToUInt32LE(uInt32Buffer);
#endif

        m_data->json = static_cast<char*>(std::malloc(m_data->jsonBuffer));
        if (!m_data->json) {
            m_data->error = Error::JsonMallocError; // 21
            return false;
        }
        size = readBuffer(data, m_data->json, &pos, m_data->jsonBuffer, length);
        if (size != m_data->jsonBuffer) {
            std::free(m_data->json);
            m_data->json = nullptr;
            m_data->error = Error::JsonReadError; // 22
            return false;
        }

        pos = m_data->titlOffset + headerSize;
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            m_data->error = Error::IncompleteTitleMarker; // 23
            return false;
        }
        if (strncmp(markerBuffer, "TITL", 4) != 0) {
            m_data->error = Error::IncorrectTitleMarker; // 24
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data->error = Error::IncompleteTitleBuffer; // 25
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(&m_data->titlBuffer, uInt32Buffer, 4);
#else
        m_data->titlBuffer = charToUInt32LE(uInt32Buffer);
#endif

        m_data->title = static_cast<char*>(std::malloc(m_data->titlBuffer));
        if (!m_data->title) {
            m_data->error = Error::TitleMallocError; // 26
            return false;
        }
        size = readBuffer(data, m_data->title, &pos, m_data->titlBuffer, length);
        if (size != m_data->titlBuffer) {
            std::free(m_data->title);
            m_data->title = nullptr;
            m_data->error = Error::TitleReadError; // 27
            return false;
        }

        pos = m_data->descOffset + headerSize;
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            m_data->error = Error::IncompleteDescMarker; // 28
            return false;
        }
        if (strncmp(markerBuffer, "DESC", 4) != 0) {
            m_data->error = Error::IncorrectDescMarker; // 29
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            m_data->error = Error::IncompleteDescBuffer; // 30
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(&m_data->descBuffer, uInt32Buffer, 4);
#else
        m_data->descBuffer = charToUInt32LE(uInt32Buffer);
#endif

        m_data->description = static_cast<char*>(std::malloc(m_data->descBuffer));
        if (!m_data->description) {
            m_data->error = Error::DescMallocError; // 31
            return false;
        }
        size = readBuffer(data, m_data->description, &pos, m_data->descBuffer, length);
        if (size != m_data->descBuffer) {
            std::free(m_data->description);
            m_data->description = nullptr;
            m_data->error = Error::DescReadError; // 32
            return false;
        }

        pos = m_data->endOfFile + headerSize - 4;
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            m_data->error = Error::IncompleteJendMarker; // 33
            return false;
        }
        if (strncmp(markerBuffer, "JEND", 4) != 0) {
            m_data->error = Error::IncorrectJendMarker; // 34
            return false;
        }

#ifdef RAGEPHOTO_BENCHMARK
        auto benchmark_parse_end = std::chrono::high_resolution_clock::now();
        auto benchmark_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(benchmark_parse_end - benchmark_parse_start);
        std::cout << "Benchmark: " << benchmark_ns.count() << "ns" << std::endl;
#endif

#ifdef RAGEPHOTO_DEBUG
        std::cout << "header: " << m_data->header << std::endl;
        std::cout << "headerSum: " << m_data->headerSum << std::endl;
        std::cout << "unnamedSum1: " << m_data->unnamedSum1 << std::endl;
        std::cout << "unnamedSum2: " << m_data->unnamedSum2 << std::endl;
        std::cout << "photoBuffer: " << m_data->photoBuffer << std::endl;
        std::cout << "descBuffer: " << m_data->descBuffer << std::endl;
        std::cout << "descOffset: " << m_data->descOffset << std::endl;
        std::cout << "description: " << m_data->description << std::endl;
        std::cout << "jsonBuffer: " << m_data->jsonBuffer << std::endl;
        std::cout << "jsonOffset: " << m_data->jsonOffset << std::endl;
        std::cout << "json: " << m_data->json << std::endl;
        std::cout << "titlBuffer: " << m_data->titlBuffer << std::endl;
        std::cout << "titlOffset: " << m_data->titlOffset << std::endl;
        std::cout << "title: " << m_data->title << std::endl;
        std::cout << "eofOffset: " << m_data->endOfFile << std::endl;
        std::cout << "setBufferOffsets()" << std::endl;
        setBufferOffsets();
        std::cout << "descOffset: " << m_data->descOffset << std::endl;
        std::cout << "jsonOffset: " << m_data->jsonOffset << std::endl;
        std::cout << "titlOffset: " << m_data->titlOffset << std::endl;
        std::cout << "eofOffset: " << m_data->endOfFile << std::endl;
        std::cout << "calc size: " << saveSize() << std::endl;
        std::cout << "real size: " << length << std::endl;
#endif

        m_data->error = Error::NoError; // 255
        return true;
#else
        std::cout << "UTF-16LE decoding support missing" << std::endl;
        m_data->error = Error::UnicodeInitError; // 4
        return false;
#endif
    }
    m_data->error = Error::IncompatibleFormat; // 2
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
        m_data->error = Error::Uninitialised; // 0
        return false;
    }
}

uint8_t RagePhoto::error() const
{
    return m_data->error;
}

uint32_t RagePhoto::format() const
{
    return m_data->photoFormat;
}

const std::string RagePhoto::photo() const
{
    if (m_data->jpeg)
        return std::string(m_data->jpeg, m_data->jpegSize);
    else
        return std::string();
}

const char* RagePhoto::photoData() const
{
    return m_data->jpeg;
}

uint32_t RagePhoto::photoSize() const
{
    if (m_data->jpeg)
        return m_data->jpegSize;
    else
        return 0UL;
}

const char* RagePhoto::description() const
{
    return m_data->description;
}

const char* RagePhoto::json() const
{
    return m_data->json;
}

const char* RagePhoto::header() const
{
    return m_data->header;
}

const char* RagePhoto::title() const
{
    return m_data->title;
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
        std::u16string photoHeader_string = convert.from_bytes(m_data->header);
        if (convert.converted() == 0) {
            m_data->error = Error::UnicodeHeaderError; // 6
            return false;
        }
        const size_t photoHeader_size = photoHeader_string.size() * 2;
        if (photoHeader_size > 256) {
            m_data->error = Error::HeaderBufferTight; // 35
            return false;
        }
        const char *photoHeader = reinterpret_cast<const char*>(photoHeader_string.data());
#elif defined UNICODE_ICONV
        iconv_t iconv_in = iconv_open("UTF-16LE", "UTF-8");
        if (iconv_in == (iconv_t)-1) {
            m_data->error = Error::UnicodeInitError; // 4
            return false;
        }
        char photoHeader[256]{};
        size_t src_s = strlen(m_data->header);
        size_t dst_s = sizeof(photoHeader);
        char *src = m_data->header;
        char *dst = photoHeader;
        const size_t ret = iconv(iconv_in, &src, &src_s, &dst, &dst_s);
        iconv_close(iconv_in);
        if (ret == static_cast<size_t>(-1)) {
            m_data->error = Error::UnicodeHeaderError; // 6
            return false;
        }
        const size_t photoHeader_size = 256;
#elif defined UNICODE_WINCVT
        char photoHeader[256]{};
        const int converted = MultiByteToWideChar(CP_UTF8, 0, m_data->header, static_cast<int>(strlen(m_data->header)), reinterpret_cast<wchar_t*>(photoHeader), 256 / sizeof(wchar_t));
        if (converted == 0) {
            m_data->error = Error::UnicodeHeaderError; // 6
            return false;
        }
        const size_t photoHeader_size = 256;
#endif

        if (m_data->jpegSize > m_data->photoBuffer) {
            m_data->error = Error::PhotoBufferTight; // 36
            return false;
        }

        const size_t jsonString_size = strlen(m_data->json) + 1;
        if (jsonString_size > m_data->jsonBuffer) {
            m_data->error = Error::JsonBufferTight; // 37
            return false;
        }

        const size_t titlString_size = strlen(m_data->title) + 1;
        if (titlString_size > m_data->titlBuffer) {
            m_data->error = Error::TitleBufferTight; // 38
            return false;
        }

        const size_t descString_size = strlen(m_data->description) + 1;
        if (descString_size > m_data->descBuffer) {
            m_data->error = Error::DescBufferTight; // 39
            return false;
        }

        const size_t length = saveSize(photoFormat);
        size_t pos = 0;
        char uInt32Buffer[4];

#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(uInt32Buffer, &photoFormat, 4);
#else
        uInt32ToCharLE(static_cast<uint32_t>(photoFormat), uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer(photoHeader, data, &pos, length, photoHeader_size);
        for (size_t i = photoHeader_size; i < 256; i++) {
            writeBuffer("\0", data, &pos, length, 1);
        }

#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(uInt32Buffer, &m_data->headerSum, 4);
#else
        uInt32ToCharLE(m_data->headerSum, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        if (photoFormat == PhotoFormat::RDR2) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
            std::memcpy(uInt32Buffer, &m_data->unnamedSum1, 4);
#else
            uInt32ToCharLE(m_data->unnamedSum1, uInt32Buffer);
#endif
            writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
            std::memcpy(uInt32Buffer, &m_data->unnamedSum2, 4);
#else
            uInt32ToCharLE(m_data->unnamedSum2, uInt32Buffer);
#endif
            writeBuffer(uInt32Buffer, data, &pos, length, 4);
        }
        const size_t headerSize = pos;

#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(uInt32Buffer, &m_data->endOfFile, 4);
#else
        uInt32ToCharLE(m_data->endOfFile, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(uInt32Buffer, &m_data->jsonOffset, 4);
#else
        uInt32ToCharLE(m_data->jsonOffset, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(uInt32Buffer, &m_data->titlOffset, 4);
#else
        uInt32ToCharLE(m_data->titlOffset, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(uInt32Buffer, &m_data->descOffset, 4);
#else
        uInt32ToCharLE(m_data->descOffset, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer("JPEG", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(uInt32Buffer, &m_data->photoBuffer, 4);
#else
        uInt32ToCharLE(m_data->photoBuffer, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(uInt32Buffer, &m_data->jpegSize, 4);
#else
        uInt32ToCharLE(m_data->jpegSize, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer(m_data->jpeg, data, &pos, length, m_data->jpegSize);
        for (size_t i = m_data->jpegSize; i < m_data->photoBuffer; i++) {
            writeBuffer("\0", data, &pos, length, 1);
        }

        pos = m_data->jsonOffset + headerSize;
        writeBuffer("JSON", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(uInt32Buffer, &m_data->jsonBuffer, 4);
#else
        uInt32ToCharLE(m_data->jsonBuffer, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer(m_data->json, data, &pos, length, jsonString_size);
        for (size_t i = jsonString_size; i < m_data->jsonBuffer; i++) {
            writeBuffer("\0", data, &pos, length, 1);
        }

        pos = m_data->titlOffset + headerSize;
        writeBuffer("TITL", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(uInt32Buffer, &m_data->titlBuffer, 4);
#else
        uInt32ToCharLE(m_data->titlBuffer, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer(m_data->title, data, &pos, length, titlString_size);
        for (size_t i = titlString_size; i < m_data->titlBuffer; i++) {
            writeBuffer("\0", data, &pos, length, 1);
        }

        pos = m_data->descOffset + headerSize;
        writeBuffer("DESC", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        std::memcpy(uInt32Buffer, &m_data->descBuffer, 4);
#else
        uInt32ToCharLE(m_data->descBuffer, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer(m_data->description, data, &pos, length, descString_size);
        for (size_t i = descString_size; i < m_data->descBuffer; i++) {
            writeBuffer("\0", data, &pos, length, 1);
        }

        pos = m_data->endOfFile + headerSize - 4;
        writeBuffer("JEND", data, &pos, length, 4);

        m_data->error = Error::NoError; // 255
        return true;
#else
        std::cout << "UTF-16LE encoding support missing" << std::endl;
        m_data->error = Error::UnicodeInitError; // 4
        return false;
#endif
    }

    m_data->error = Error::IncompatibleFormat; // 2
    return false;
}

bool RagePhoto::save(char *data)
{
    return save(data, m_data->photoFormat);
}

const std::string RagePhoto::save(uint32_t photoFormat, bool *ok)
{
    std::string sdata;
    const size_t size = saveSize(photoFormat);
    if (size == 0) {
        if (ok)
            *ok = false;
        return sdata;
    }
    sdata.resize(size);
    const bool saved = save(&sdata[0], photoFormat);
    if (ok)
        *ok = saved;
    return sdata;
}

const std::string RagePhoto::save(bool *ok)
{
    return save(m_data->photoFormat, ok);
}

bool RagePhoto::saveFile(const std::string &filename, uint32_t photoFormat)
{
    bool ok;
    const std::string &sdata = save(photoFormat, &ok);
    if (ok) {
        std::ofstream ofs(filename, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!ofs.is_open()) {
            m_data->error = Error::Uninitialised; // 0
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
    return saveFile(filename, m_data->photoFormat);
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
    return saveSize(m_data, photoFormat);
}

inline size_t RagePhoto::saveSize()
{
    return saveSize(m_data->photoFormat);
}

inline void RagePhoto::setBufferDefault()
{
    setBufferDefault(m_data);
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
    setBufferOffsets(m_data);
}

inline void RagePhoto::setBufferOffsets(RagePhotoData *ragePhotoData)
{
    ragePhotoData->jsonOffset = ragePhotoData->photoBuffer + 28;
    ragePhotoData->titlOffset = ragePhotoData->jsonOffset + ragePhotoData->jsonBuffer + 8;
    ragePhotoData->descOffset = ragePhotoData->titlOffset + ragePhotoData->titlBuffer + 8;
    ragePhotoData->endOfFile = ragePhotoData->descOffset + ragePhotoData->descBuffer + 12;
}

bool RagePhoto::setData(RagePhotoData *ragePhotoData, bool takeOwnership)
{
    if (takeOwnership) {
        std::free(m_data->jpeg);
        std::free(m_data->description);
        std::free(m_data->json);
        std::free(m_data->header);
        std::free(m_data->title);
        delete m_data;

        m_data = ragePhotoData;
    }
    else {
        clear();

        m_data->photoFormat = ragePhotoData->photoFormat;

        if (ragePhotoData->header) {
            const size_t headerSize = strlen(ragePhotoData->header) + 1;
            m_data->header = static_cast<char*>(std::malloc(headerSize));
            if (!m_data->header)
                return false;
            std::memcpy(m_data->header, ragePhotoData->header, headerSize);
            m_data->headerSum = ragePhotoData->headerSum;
        }

        if (ragePhotoData->jpeg) {
            m_data->jpeg = static_cast<char*>(std::malloc(ragePhotoData->jpegSize));
            if (!m_data->jpeg)
                return false;
            std::memcpy(m_data->jpeg, ragePhotoData->jpeg, ragePhotoData->jpegSize);
            m_data->jpegSize = ragePhotoData->jpegSize;
            m_data->photoBuffer = ragePhotoData->photoBuffer;
        }

        if (ragePhotoData->json) {
            const size_t jsonSize = strlen(ragePhotoData->json) + 1;
            m_data->json = static_cast<char*>(std::malloc(jsonSize));
            if (!m_data->json)
                return false;
            std::memcpy(m_data->json, ragePhotoData->json, jsonSize);
            m_data->jsonBuffer = ragePhotoData->jsonBuffer;
        }

        if (ragePhotoData->title) {
            const size_t titleSize = strlen(ragePhotoData->title) + 1;
            m_data->title = static_cast<char*>(std::malloc(titleSize));
            if (!m_data->title)
                return false;
            std::memcpy(m_data->title, ragePhotoData->title, titleSize);
            m_data->titlBuffer = ragePhotoData->titlBuffer;
        }

        if (ragePhotoData->description) {
            const size_t descriptionSize = strlen(ragePhotoData->description) + 1;
            m_data->description = static_cast<char*>(std::malloc(descriptionSize));
            if (!m_data->description)
                return false;
            std::memcpy(m_data->description, ragePhotoData->description, descriptionSize);
            m_data->descBuffer = ragePhotoData->descBuffer;
        }

        m_data->unnamedSum1 = ragePhotoData->unnamedSum1;
        m_data->unnamedSum2 = ragePhotoData->unnamedSum2;

        setBufferOffsets();
    }

    return true;
}

void RagePhoto::setDescription(const char *description, uint32_t bufferSize)
{
    if (!writeDataChar(description, &m_data->description)) {
        m_data->error = Error::DescMallocError; // 31
        return;
    }
    if (bufferSize != 0) {
        m_data->descBuffer = bufferSize;
        setBufferOffsets();
    }
    m_data->error = Error::NoError; // 255
}

void RagePhoto::setFormat(uint32_t photoFormat)
{
    m_data->photoFormat = photoFormat;
}

void RagePhoto::setJson(const char *json, uint32_t bufferSize)
{
    if (!writeDataChar(json, &m_data->json)) {
        m_data->error = Error::JsonMallocError; // 21
        return;
    }
    if (bufferSize != 0) {
        m_data->jsonBuffer = bufferSize;
        setBufferOffsets();
    }
    m_data->error = Error::NoError; // 255
}

void RagePhoto::setHeader(const char *header, uint32_t headerSum)
{
    if (!writeDataChar(header, &m_data->header)) {
        m_data->error = Error::HeaderMallocError; // 4
        return;
    }
    m_data->headerSum = headerSum;
    m_data->error = Error::NoError; // 255
}

bool RagePhoto::setPhoto(const char *data, uint32_t size, uint32_t bufferSize)
{
    if (m_data->jpeg) {
        if (m_data->jpegSize > size) {
            char *t_photoData = static_cast<char*>(std::realloc(m_data->jpeg, size));
            if (!t_photoData) {
                m_data->error = Error::PhotoMallocError; // 16
                return false;
            }
            m_data->jpeg = t_photoData;
            std::memcpy(m_data->jpeg, data, size);
            m_data->jpegSize = size;
        }
        else if (m_data->jpegSize < size) {
            std::free(m_data->jpeg);
            m_data->jpeg = static_cast<char*>(std::malloc(size));
            if (!m_data->jpeg) {
                m_data->error = Error::PhotoMallocError; // 16
                return false;
            }
            std::memcpy(m_data->jpeg, data, size);
            m_data->jpegSize = size;
        }
        else {
            std::memcpy(m_data->jpeg, data, size);
        }
    }
    else {
        m_data->jpeg = static_cast<char*>(std::malloc(size));
        if (!m_data->jpeg) {
            m_data->error = Error::PhotoMallocError; // 16
            return false;
        }
        std::memcpy(m_data->jpeg, data, size);
        m_data->jpegSize = size;
    }

    if (bufferSize != 0) {
        m_data->photoBuffer = bufferSize;
        setBufferOffsets();
    }

    m_data->error = Error::NoError; // 255
    return true;
}

bool RagePhoto::setPhoto(const std::string &data, uint32_t bufferSize)
{
    return setPhoto(data.data(), static_cast<uint32_t>(data.size()), bufferSize);
}

void RagePhoto::setTitle(const char *title, uint32_t bufferSize)
{
    if (!writeDataChar(title, &m_data->title)) {
        m_data->error = Error::TitleMallocError; // 26
        return;
    }
    if (bufferSize != 0) {
        m_data->titlBuffer = bufferSize;
        setBufferOffsets();
    }
    m_data->error = Error::NoError; // 255
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

ragephoto_bool_t ragephoto_load(ragephoto_t instance, const char *data, size_t size)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->load(data, size);
}

ragephoto_bool_t ragephoto_loadfile(ragephoto_t instance, const char *filename)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->loadFile(filename);
}

uint8_t ragephoto_error(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->error();
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

RagePhotoData* ragephoto_getphotodata(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->data();
}

const char* ragephoto_getphotodesc(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->description();
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
    return ragePhoto->json();
}

const char* ragephoto_getphotoheader(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->header();
}

uint32_t ragephoto_getphotosize(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->photoSize();
}

const char* ragephoto_getphototitle(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->title();
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

ragephoto_bool_t ragephoto_save(ragephoto_t instance, char *data)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->save(data);
}

ragephoto_bool_t ragephoto_savef(ragephoto_t instance, char *data, uint32_t photoFormat)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->save(data, photoFormat);
}

ragephoto_bool_t ragephoto_savefile(ragephoto_t instance, const char *filename)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->saveFile(filename);
}

ragephoto_bool_t ragephoto_savefilef(ragephoto_t instance, const char *filename, uint32_t photoFormat)
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

void ragephoto_setphotodata(ragephoto_t instance, RagePhotoData *ragePhotoData, ragephoto_bool_t takeOwnership)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    ragePhoto->setData(ragePhotoData, takeOwnership);
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

ragephoto_bool_t ragephoto_setphotojpeg(ragephoto_t instance, const char *data, uint32_t size, uint32_t bufferSize)
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
/* END OF RAGEPHOTO CLASS */
