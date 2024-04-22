/*****************************************************************************
* libragephoto RAGE Photo Parser
* Copyright (C) 2021-2024 Syping
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

#include "ragephoto_cxx.hpp"
#ifdef LIBRAGEPHOTO_CXX_C
#include "RagePhoto.h"
#include <cinttypes>
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

/* CLASSIC RAGEPHOTO TYPEDEF */
typedef ragephoto::photo RagePhoto;

/* RAGEPHOTO LIBRARY GLOBALS */
int libraryflags = 0;
const char* nullchar = "";

/* BEGIN OF STATIC LIBRARY FUNCTIONS */
inline size_t readBuffer(const char *input, void *output, size_t *pos, size_t outputLen, size_t inputLen)
{
    size_t readLen = 0;
    if (*pos >= inputLen)
        return 0;
    readLen = inputLen - *pos;
    if (readLen > outputLen)
        readLen = outputLen;
    memcpy(output, &input[*pos], readLen);
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
    memcpy(&output[*pos], input, writeLen);
    *pos = *pos + writeLen;
    return writeLen;
}

inline size_t zeroBuffer(char *output, size_t *pos, size_t outputLen, size_t inputLen)
{
    const size_t maxLen = outputLen - *pos;
    size_t zeroLen = inputLen;
    if (*pos >= outputLen)
        return 0;
    if (inputLen > maxLen)
        zeroLen = maxLen;
    memset(&output[*pos], 0, zeroLen);
    *pos = *pos + zeroLen;
    return zeroLen;
}

inline bool writeDataChar(const char *input, char **output)
{
    const size_t src_s = strlen(input) + 1;
    if (*output) {
        const size_t dst_s = strlen(*output) + 1;
        if (dst_s > src_s) {
            char *t_output = static_cast<char*>(realloc(*output, src_s));
            if (!t_output) {
                return false;
            }
            *output = t_output;
            memcpy(*output, input, src_s);
        }
        else if (dst_s < src_s) {
            char *t_output = static_cast<char*>(malloc(src_s));
            if (!t_output) {
                return false;
            }
            free(*output);
            *output = t_output;
            memcpy(*output, input, src_s);
        }
        else {
            memcpy(*output, input, src_s);
        }
    }
    else {
        char *t_output = static_cast<char*>(malloc(src_s));
        if (!t_output) {
            return false;
        }
        *output = t_output;
        memcpy(*output, input, src_s);
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

inline uint32_t joaatFromInitial(const char *data, size_t size, uint32_t init_val)
{
    uint32_t val = init_val;
    for (size_t i = 0; i != size; i++) {
        val += data[i];
        val += val << 10;
        val ^= val >> 6;
    }
    val += val << 3;
    val ^= val >> 11;
    val += val << 15;
    return val;
}
/* END OF STATIC LIBRARY FUNCTIONS */

/* BEGIN OF RAGEPHOTO CLASS */
RagePhoto::photo()
{
    m_data = static_cast<RagePhotoData*>(malloc(sizeof(RagePhotoData)));
    if (!m_data)
        throw std::runtime_error("RagePhotoData data struct can't be allocated");
    memset(m_data, 0, sizeof(RagePhotoData));
    m_parser = static_cast<RagePhotoFormatParser*>(malloc(sizeof(RagePhotoFormatParser)));
    if (!m_parser)
        throw std::runtime_error("RagePhotoFormatParser parser struct can't be allocated");
    memset(m_parser, 0, sizeof(RagePhotoFormatParser));
    setBufferDefault(m_data);
}

RagePhoto::~photo()
{
    free(m_data->jpeg);
    free(m_data->description);
    free(m_data->json);
    free(m_data->header);
    free(m_data->title);
    free(m_data);
    free(m_parser);
}

void RagePhoto::addParser(RagePhotoFormatParser *rp_parser)
{
    if (rp_parser) {
        RagePhotoFormatParser n_parser[1]{};
        if (!memcmp(&n_parser[0], rp_parser, sizeof(RagePhotoFormatParser)))
            return;
        size_t length;
        for (length = 0; memcmp(&n_parser[0], &m_parser[length], sizeof(RagePhotoFormatParser)); length++);
        RagePhotoFormatParser *t_parser = static_cast<RagePhotoFormatParser*>(realloc(m_parser, (length + 2 * sizeof(RagePhotoFormatParser))));
        if (!t_parser)
            throw std::runtime_error("RagePhotoFormatParser array can't be expanded");
        m_parser = t_parser;
        memcpy(&m_parser[length], rp_parser, sizeof(RagePhotoFormatParser));
        memset(&m_parser[length + 1], 0, sizeof(RagePhotoFormatParser));
    }
}

void RagePhoto::clear(RagePhotoData *rp_data)
{
    free(rp_data->jpeg);
    free(rp_data->description);
    free(rp_data->json);
    free(rp_data->header);
    free(rp_data->title);
    memset(rp_data, 0, sizeof(RagePhotoData));
    setBufferDefault(rp_data);
}

void RagePhoto::clear()
{
    clear(m_data);
}

RagePhotoData* RagePhoto::data()
{
    return m_data;
}

bool RagePhoto::load(const char *data, size_t length, RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser)
{
#ifdef RAGEPHOTO_BENCHMARK
    auto benchmark_parse_start = std::chrono::steady_clock::now();
#endif

    // Avoid data conflicts
    clear(rp_data);

    size_t pos = 0;
    char uInt32Buffer[4];
    size_t size = readBuffer(data, uInt32Buffer, &pos, 4, length);
    if (size != 4) {
        rp_data->error = Error::NoFormatIdentifier; // 1
        return false;
    }

#if __BYTE_ORDER == __LITTLE_ENDIAN
    memcpy(&rp_data->photoFormat, uInt32Buffer, 4);
#else
    rp_data->photoFormat = charToUInt32LE(uInt32Buffer);
#endif
    if (rp_data->photoFormat == PhotoFormat::GTA5 || rp_data->photoFormat == PhotoFormat::RDR2) {
#if defined UNICODE_ICONV || defined UNICODE_CODECVT || defined UNICODE_WINCVT
        char photoHeader[256];
        size = readBuffer(data, photoHeader, &pos, 256, length);
        if (size != 256) {
            rp_data->error = Error::IncompleteHeader; // 3
            return false;
        }

#if defined UNICODE_CODECVT
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
        const std::string photoHeader_string = convert.to_bytes(reinterpret_cast<char16_t*>(photoHeader));
        if (convert.converted() == 0) {
            rp_data->error = Error::UnicodeHeaderError; // 6
            return false;
        }
        const size_t photoHeader_size = photoHeader_string.size() + 1;
        rp_data->header = static_cast<char*>(malloc(photoHeader_size));
        if (!rp_data->header) {
            rp_data->error = Error::HeaderMallocError; // 4
            return false;
        }
        memcpy(rp_data->header, photoHeader_string.c_str(), photoHeader_size);
#elif defined UNICODE_ICONV
        iconv_t iconv_in = iconv_open("UTF-8", "UTF-16LE");
        if (iconv_in == (iconv_t)-1) {
            rp_data->error = Error::UnicodeInitError; // 4
            return false;
        }
        rp_data->header = static_cast<char*>(malloc(256));
        if (!rp_data->header) {
            rp_data->error = Error::HeaderMallocError; // 4
            iconv_close(iconv_in);
            return false;
        }
        size_t src_s = sizeof(photoHeader);
        size_t dst_s = 256;
        char *src = photoHeader;
        char *dst = rp_data->header;
        const size_t ret = iconv(iconv_in, &src, &src_s, &dst, &dst_s);
        iconv_close(iconv_in);
        if (ret == static_cast<size_t>(-1)) {
            rp_data->error = Error::UnicodeHeaderError; // 6
            return false;
        }
#elif defined UNICODE_WINCVT
        rp_data->header = static_cast<char*>(malloc(256));
        if (!rp_data->header) {
            rp_data->error = Error::HeaderMallocError; // 4
            return false;
        }
        const int converted = WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<wchar_t*>(photoHeader), -1, rp_data->header, 256, NULL, NULL);
        if (converted == 0) {
            free(rp_data->header);
            rp_data->header = nullptr;
            rp_data->error = Error::UnicodeHeaderError; // 6
            return false;
        }
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = Error::IncompleteChecksum; // 7
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->headerSum, uInt32Buffer, 4);
#else
        rp_data->headerSum = charToUInt32LE(uInt32Buffer);
#endif

        if (rp_data->photoFormat == PhotoFormat::RDR2) {
            char formatCheckBuffer[4];
            size = readBuffer(data, formatCheckBuffer, &pos, 4, length);
            if (size != 4) {
                rp_data->error = Error::IncompleteChecksum; // 7
                return false;
            }
            char n_formatCheckBuffer[4]{};
            if (memcmp(formatCheckBuffer, n_formatCheckBuffer, 4)) {
                rp_data->error = Error::IncompatibleFormat; // 2
                return false;
            }

            size = readBuffer(data, uInt32Buffer, &pos, 4, length);
            if (size != 4) {
                rp_data->error = Error::IncompleteChecksum; // 7
                return false;
            }
#if __BYTE_ORDER == __LITTLE_ENDIAN
            memcpy(&rp_data->headerSum2, uInt32Buffer, 4);
#else
            rp_data->headerSum2 = charToUInt32LE(uInt32Buffer);
#endif
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = Error::IncompleteEOF; // 8
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->endOfFile, uInt32Buffer, 4);
#else
        rp_data->endOfFile = charToUInt32LE(uInt32Buffer);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = Error::IncompleteJsonOffset; // 9
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->jsonOffset, uInt32Buffer, 4);
#else
        rp_data->jsonOffset = charToUInt32LE(uInt32Buffer);
#endif
        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = Error::IncompleteTitleOffset; // 10
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->titlOffset, uInt32Buffer, 4);
#else
        rp_data->titlOffset = charToUInt32LE(uInt32Buffer);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = Error::IncompleteDescOffset; // 11
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->descOffset, uInt32Buffer, 4);
#else
        rp_data->descOffset = charToUInt32LE(uInt32Buffer);
#endif

        char markerBuffer[4];
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = Error::IncompleteJpegMarker; // 12
            return false;
        }
        if (memcmp(markerBuffer, "JPEG", 4)) {
            rp_data->error = Error::IncorrectJpegMarker; // 13
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = Error::IncompletePhotoBuffer; // 14
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->jpegBuffer, uInt32Buffer, 4);
#else
        rp_data->jpegBuffer = charToUInt32LE(uInt32Buffer);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = Error::IncompletePhotoSize; // 15
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->jpegSize, uInt32Buffer, 4);
#else
        rp_data->jpegSize = charToUInt32LE(uInt32Buffer);
#endif

        rp_data->jpeg = static_cast<char*>(malloc(rp_data->jpegSize));
        if (!rp_data->jpeg) {
            rp_data->error = Error::PhotoMallocError; // 16
            return false;
        }
        size = readBuffer(data, rp_data->jpeg, &pos, rp_data->jpegSize, length);
        if (size != rp_data->jpegSize) {
            free(rp_data->jpeg);
            rp_data->jpeg = nullptr;
            rp_data->error = Error::PhotoReadError; // 17
            return false;
        }

        pos += rp_data->jpegBuffer - rp_data->jpegSize;
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = Error::IncompleteJsonMarker; // 18
            return false;
        }
        if (memcmp(markerBuffer, "JSON", 4)) {
            rp_data->error = Error::IncorrectJsonMarker; // 19
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = Error::IncompleteJsonBuffer; // 20
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->jsonBuffer, uInt32Buffer, 4);
#else
        rp_data->jsonBuffer = charToUInt32LE(uInt32Buffer);
#endif

        rp_data->json = static_cast<char*>(malloc(rp_data->jsonBuffer));
        if (!rp_data->json) {
            rp_data->error = Error::JsonMallocError; // 21
            return false;
        }
        size = readBuffer(data, rp_data->json, &pos, rp_data->jsonBuffer, length);
        if (size != rp_data->jsonBuffer) {
            free(rp_data->json);
            rp_data->json = nullptr;
            rp_data->error = Error::JsonReadError; // 22
            return false;
        }

        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = Error::IncompleteTitleMarker; // 23
            return false;
        }
        if (memcmp(markerBuffer, "TITL", 4)) {
            rp_data->error = Error::IncorrectTitleMarker; // 24
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = Error::IncompleteTitleBuffer; // 25
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->titlBuffer, uInt32Buffer, 4);
#else
        rp_data->titlBuffer = charToUInt32LE(uInt32Buffer);
#endif

        rp_data->title = static_cast<char*>(malloc(rp_data->titlBuffer));
        if (!rp_data->title) {
            rp_data->error = Error::TitleMallocError; // 26
            return false;
        }
        size = readBuffer(data, rp_data->title, &pos, rp_data->titlBuffer, length);
        if (size != rp_data->titlBuffer) {
            free(rp_data->title);
            rp_data->title = nullptr;
            rp_data->error = Error::TitleReadError; // 27
            return false;
        }

        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = Error::IncompleteDescMarker; // 28
            return false;
        }
        if (memcmp(markerBuffer, "DESC", 4)) {
            rp_data->error = Error::IncorrectDescMarker; // 29
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = Error::IncompleteDescBuffer; // 30
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->descBuffer, uInt32Buffer, 4);
#else
        rp_data->descBuffer = charToUInt32LE(uInt32Buffer);
#endif

        rp_data->description = static_cast<char*>(malloc(rp_data->descBuffer));
        if (!rp_data->description) {
            rp_data->error = Error::DescMallocError; // 31
            return false;
        }
        size = readBuffer(data, rp_data->description, &pos, rp_data->descBuffer, length);
        if (size != rp_data->descBuffer) {
            free(rp_data->description);
            rp_data->description = nullptr;
            rp_data->error = Error::DescReadError; // 32
            return false;
        }

        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = Error::IncompleteJendMarker; // 33
            return false;
        }
        if (memcmp(markerBuffer, "JEND", 4)) {
            rp_data->error = Error::IncorrectJendMarker; // 34
            return false;
        }

#ifdef RAGEPHOTO_BENCHMARK
        auto benchmark_parse_end = std::chrono::steady_clock::now();
        auto benchmark_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(benchmark_parse_end - benchmark_parse_start);
        std::cout << "Benchmark: " << benchmark_ns.count() << "ns" << std::endl;
#endif

#ifdef RAGEPHOTO_DEBUG
        const uint32_t jsonOffset = rp_data->jpegBuffer + UINT32_C(28);
        const uint32_t titlOffset = jsonOffset + rp_data->jsonBuffer + UINT32_C(8);
        const uint32_t descOffset = titlOffset + rp_data->titlBuffer + UINT32_C(8);
        const uint32_t endOfFile = descOffset + rp_data->descBuffer + UINT32_C(12);
        std::cout << "header: " << rp_data->header << std::endl;
        std::cout << "headerSum: " << rp_data->headerSum << std::endl;
        std::cout << "headerSum2: " << rp_data->headerSum2 << std::endl;
        std::cout << "photoBuffer: " << rp_data->jpegBuffer << std::endl;
        std::cout << "descBuffer: " << rp_data->descBuffer << std::endl;
        std::cout << "descOffset: " << rp_data->descOffset << " (" << descOffset << ")" << std::endl;
        std::cout << "description: " << rp_data->description << std::endl;
        std::cout << "jsonBuffer: " << rp_data->jsonBuffer << std::endl;
        std::cout << "jsonOffset: " << rp_data->jsonOffset << " (" << jsonOffset << ")" << std::endl;
        std::cout << "json: " << rp_data->json << std::endl;
        std::cout << "sign: " << jpegSign(rp_data) << std::endl;
        std::cout << "titlBuffer: " << rp_data->titlBuffer << std::endl;
        std::cout << "titlOffset: " << rp_data->titlOffset << " (" << titlOffset << ")" << std::endl;
        std::cout << "title: " << rp_data->title << std::endl;
        std::cout << "eofOffset: " << rp_data->endOfFile << " (" << endOfFile << ")" << std::endl;
        std::cout << "size: " << length << " (" << saveSize(rp_data, nullptr) << ")" << std::endl;
#endif

        rp_data->error = Error::NoError; // 255
        return true;
#else
        std::cout << "UTF-16LE decoding support missing" << std::endl;
        rp_data->error = Error::UnicodeInitError; // 4
        return false;
#endif
    }
    else if (rp_parser) {
        RagePhotoFormatParser n_parser[1]{};
        for (size_t i = 0; memcmp(&n_parser[0], &rp_parser[i], sizeof(RagePhotoFormatParser)); i++) {
            if (rp_data->photoFormat == rp_parser[i].photoFormat)
                if (rp_parser[i].funcLoad)
                    return (rp_parser[i].funcLoad)(rp_data, data, length);
        }
    }
    rp_data->error = Error::IncompatibleFormat; // 2
    return false;
}

bool RagePhoto::load(const char *data, size_t size)
{
    return load(data, size, m_data, m_parser);
}

bool RagePhoto::load(const std::string &data)
{
    return load(data.data(), data.size(), m_data, m_parser);
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

int32_t RagePhoto::error() const
{
    return m_data->error;
}

uint32_t RagePhoto::format() const
{
    return m_data->photoFormat;
}

const std::string RagePhoto::jpeg() const
{
    if (m_data->jpeg)
        return std::string(m_data->jpeg, m_data->jpegSize);
    return std::string();
}

#if (RAGEPHOTO_CXX_STD >= 17) && (__cplusplus >= 201703L)
const std::string_view RagePhoto::jpeg_view() const
{
    if (m_data->jpeg)
        return std::string_view(m_data->jpeg, m_data->jpegSize);
    return std::string_view();
}
#endif

const char* RagePhoto::jpegData() const
{
    if (m_data->jpeg)
        return m_data->jpeg;
    return (libraryflags & RAGEPHOTO_FLAG_LEGACY_NULL_RETURN) ? nullptr : nullchar;
}

uint64_t RagePhoto::jpegSign(uint32_t photoFormat, RagePhotoData *rp_data)
{
    if (rp_data->jpeg) {
        if (photoFormat == PhotoFormat::GTA5)
            return (UINT64_C(0x100000000000000) | joaatFromInitial(rp_data->jpeg, rp_data->jpegSize, SignInitials::SIGTA5));
        else if (photoFormat == PhotoFormat::RDR2)
            return (UINT64_C(0x100000000000000) | joaatFromInitial(rp_data->jpeg, rp_data->jpegSize, SignInitials::SIRDR2));
    }
    return 0;
}

uint64_t RagePhoto::jpegSign(RagePhotoData *rp_data)
{
    return jpegSign(rp_data->photoFormat, rp_data);
}

uint64_t RagePhoto::jpegSign(uint32_t photoFormat) const
{
    return jpegSign(photoFormat, m_data);
}

uint64_t RagePhoto::jpegSign() const
{
    return jpegSign(m_data->photoFormat, m_data);
}

uint32_t RagePhoto::jpegSize() const
{
    if (m_data->jpeg)
        return m_data->jpegSize;
    return 0;
}

const char* RagePhoto::description() const
{
    if (m_data->description)
        return m_data->description;
    return (libraryflags & RAGEPHOTO_FLAG_LEGACY_NULL_RETURN) ? nullptr : nullchar;
}

const char* RagePhoto::header() const
{
    if (m_data->header)
        return m_data->header;
    return (libraryflags & RAGEPHOTO_FLAG_LEGACY_NULL_RETURN) ? nullptr : nullchar;
}

const char* RagePhoto::json() const
{
    if (m_data->json)
        return m_data->json;
    return (libraryflags & RAGEPHOTO_FLAG_LEGACY_NULL_RETURN) ? nullptr : nullchar;
}

const char* RagePhoto::title() const
{
    if (m_data->title)
        return m_data->title;
    return (libraryflags & RAGEPHOTO_FLAG_LEGACY_NULL_RETURN) ? nullptr : nullchar;
}

const char* RagePhoto::version()
{
    return RAGEPHOTO_VERSION;
}

bool RagePhoto::save(char *data, uint32_t photoFormat, RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser)
{
    if (photoFormat == PhotoFormat::GTA5 || photoFormat == PhotoFormat::RDR2) {
        if (!rp_data->header) {
            rp_data->error = Error::IncompleteHeader; // 3
            return false;
        }
#if defined UNICODE_ICONV || defined UNICODE_CODECVT || defined UNICODE_WINCVT
#if defined UNICODE_CODECVT
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
        std::u16string photoHeader_string = convert.from_bytes(rp_data->header);
        if (convert.converted() == 0) {
            rp_data->error = Error::UnicodeHeaderError; // 6
            return false;
        }
        const size_t photoHeader_size = photoHeader_string.size() * sizeof(char16_t);
        if (photoHeader_size > 256) {
            rp_data->error = Error::HeaderBufferTight; // 35
            return false;
        }
        const char *photoHeader = reinterpret_cast<const char*>(photoHeader_string.data());
#elif defined UNICODE_ICONV
        iconv_t iconv_in = iconv_open("UTF-16LE", "UTF-8");
        if (iconv_in == (iconv_t)-1) {
            rp_data->error = Error::UnicodeInitError; // 4
            return false;
        }
        char photoHeader[256]{};
        size_t src_s = strlen(rp_data->header);
        size_t dst_s = sizeof(photoHeader);
        char *src = rp_data->header;
        char *dst = photoHeader;
        const size_t ret = iconv(iconv_in, &src, &src_s, &dst, &dst_s);
        iconv_close(iconv_in);
        if (ret == static_cast<size_t>(-1)) {
            rp_data->error = Error::UnicodeHeaderError; // 6
            return false;
        }
        const size_t photoHeader_size = 256;
#elif defined UNICODE_WINCVT
        char photoHeader[256]{};
        const int converted = MultiByteToWideChar(CP_UTF8, 0, rp_data->header, static_cast<int>(strlen(rp_data->header)), reinterpret_cast<wchar_t*>(photoHeader), 256 / sizeof(wchar_t));
        if (converted == 0) {
            rp_data->error = Error::UnicodeHeaderError; // 6
            return false;
        }
        const size_t photoHeader_size = 256;
#endif

        const size_t length = saveSize(photoFormat, rp_data, nullptr);
        size_t pos = 0;

#if __BYTE_ORDER == __LITTLE_ENDIAN
        writeBuffer(&photoFormat, data, &pos, length, 4);
#else
        char uInt32Buffer[4];
        uInt32ToCharLE(photoFormat, uInt32Buffer);
        writeBuffer(uInt32Buffer, data, &pos, length, 4);
#endif

        writeBuffer(photoHeader, data, &pos, length, photoHeader_size);
        zeroBuffer(data, &pos, length, 256 - photoHeader_size);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        writeBuffer(&rp_data->headerSum, data, &pos, length, 4);
#else
        uInt32ToCharLE(rp_data->headerSum, uInt32Buffer);
        writeBuffer(uInt32Buffer, data, &pos, length, 4);
#endif

        if (photoFormat == PhotoFormat::RDR2) {
            zeroBuffer(data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
            writeBuffer(&rp_data->headerSum2, data, &pos, length, 4);
#else
            uInt32ToCharLE(rp_data->headerSum2, uInt32Buffer);
            writeBuffer(uInt32Buffer, data, &pos, length, 4);
#endif
        }

        const uint32_t jsonOffset = rp_data->jpegBuffer + UINT32_C(28);
        const uint32_t titlOffset = jsonOffset + rp_data->jsonBuffer + UINT32_C(8);
        const uint32_t descOffset = titlOffset + rp_data->titlBuffer + UINT32_C(8);
        const uint32_t endOfFile = descOffset + rp_data->descBuffer + UINT32_C(12);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        writeBuffer(&endOfFile, data, &pos, length, 4);
#else
        uInt32ToCharLE(endOfFile, uInt32Buffer);
        writeBuffer(uInt32Buffer, data, &pos, length, 4);
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
        writeBuffer(&jsonOffset, data, &pos, length, 4);
#else
        uInt32ToCharLE(jsonOffset, uInt32Buffer);
        writeBuffer(uInt32Buffer, data, &pos, length, 4);
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
        writeBuffer(&titlOffset, data, &pos, length, 4);
#else
        uInt32ToCharLE(titlOffset, uInt32Buffer);
        writeBuffer(uInt32Buffer, data, &pos, length, 4);
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
        writeBuffer(&descOffset, data, &pos, length, 4);
#else
        uInt32ToCharLE(descOffset, uInt32Buffer);
        writeBuffer(uInt32Buffer, data, &pos, length, 4);
#endif

        writeBuffer("JPEG", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        writeBuffer(&rp_data->jpegBuffer, data, &pos, length, 4);
#else
        uInt32ToCharLE(rp_data->jpegBuffer, uInt32Buffer);
        writeBuffer(uInt32Buffer, data, &pos, length, 4);
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
        writeBuffer(&rp_data->jpegSize, data, &pos, length, 4);
#else
        uInt32ToCharLE(rp_data->jpegSize, uInt32Buffer);
        writeBuffer(uInt32Buffer, data, &pos, length, 4);
#endif

        if (rp_data->jpeg) {
            if (rp_data->jpegSize > rp_data->jpegBuffer) {
                rp_data->error = Error::PhotoBufferTight; // 36
                return false;
            }
            writeBuffer(rp_data->jpeg, data, &pos, length, rp_data->jpegSize);
            zeroBuffer(data, &pos, length, rp_data->jpegBuffer - rp_data->jpegSize);
        }
        else {
            zeroBuffer(data, &pos, length, rp_data->jpegBuffer);
        }

        writeBuffer("JSON", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        writeBuffer(&rp_data->jsonBuffer, data, &pos, length, 4);
#else
        uInt32ToCharLE(rp_data->jsonBuffer, uInt32Buffer);
        writeBuffer(uInt32Buffer, data, &pos, length, 4);
#endif

        if (rp_data->json) {
            const size_t jsonString_size = strlen(rp_data->json) + 1;
            if (jsonString_size > rp_data->jsonBuffer) {
                rp_data->error = Error::JsonBufferTight; // 37
                return false;
            }
            writeBuffer(rp_data->json, data, &pos, length, jsonString_size);
            zeroBuffer(data, &pos, length, rp_data->jsonBuffer - jsonString_size);
        }
        else {
            zeroBuffer(data, &pos, length, rp_data->jsonBuffer);
        }

        writeBuffer("TITL", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        writeBuffer(&rp_data->titlBuffer, data, &pos, length, 4);
#else
        uInt32ToCharLE(rp_data->titlBuffer, uInt32Buffer);
        writeBuffer(uInt32Buffer, data, &pos, length, 4);
#endif

        if (rp_data->title) {
            const size_t titlString_size = strlen(rp_data->title) + 1;
            if (titlString_size > rp_data->titlBuffer) {
                rp_data->error = Error::TitleBufferTight; // 38
                return false;
            }
            writeBuffer(rp_data->title, data, &pos, length, titlString_size);
            zeroBuffer(data, &pos, length, rp_data->titlBuffer - titlString_size);
        }
        else {
            zeroBuffer(data, &pos, length, rp_data->titlBuffer);
        }

        writeBuffer("DESC", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        writeBuffer(&rp_data->descBuffer, data, &pos, length, 4);
#else
        uInt32ToCharLE(rp_data->descBuffer, uInt32Buffer);
        writeBuffer(uInt32Buffer, data, &pos, length, 4);
#endif

        if (rp_data->description) {
            const size_t descString_size = strlen(rp_data->description) + 1;
            if (descString_size > rp_data->descBuffer) {
                rp_data->error = Error::DescBufferTight; // 39
                return false;
            }
            writeBuffer(rp_data->description, data, &pos, length, descString_size);
            zeroBuffer(data, &pos, length, rp_data->descBuffer - descString_size);
        }
        else {
            zeroBuffer(data, &pos, length, rp_data->descBuffer);
        }

        writeBuffer("JEND", data, &pos, length, 4);

        rp_data->error = Error::NoError; // 255
        return true;
#else
        std::cout << "UTF-16LE encoding support missing" << std::endl;
        rp_data->error = Error::UnicodeInitError; // 4
        return false;
#endif
    }
    else if (rp_parser) {
        RagePhotoFormatParser n_parser[1]{};
        for (size_t i = 0; memcmp(&n_parser[0], &rp_parser[i], sizeof(RagePhotoFormatParser)); i++) {
            if (photoFormat == rp_parser[i].photoFormat)
                if (rp_parser[i].funcSave)
                    return (rp_parser[i].funcSave)(rp_data, data, photoFormat);
        }
    }

    rp_data->error = Error::IncompatibleFormat; // 2
    return false;
}

bool RagePhoto::save(char *data, RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser)
{
    return save(data, rp_data->photoFormat, rp_data, rp_parser);
}

bool RagePhoto::save(char *data, uint32_t photoFormat)
{
    return save(data, photoFormat, m_data, m_parser);
}

bool RagePhoto::save(char *data)
{
    return save(data, m_data->photoFormat, m_data, m_parser);
}

const std::string RagePhoto::save(uint32_t photoFormat, bool *ok)
{
    std::string sdata;
    const size_t size = saveSize(photoFormat, m_data, m_parser);
    if (size == 0) {
        if (ok)
            *ok = false;
        return sdata;
    }
    sdata.resize(size);
    const bool saved = save(&sdata[0], photoFormat, m_data, m_parser);
    if (ok)
        *ok = saved;
    return sdata;
}

const std::string RagePhoto::save(bool *ok)
{
    return save(m_data->photoFormat, ok);
}

bool RagePhoto::saveFile(const char *filename, uint32_t photoFormat)
{
    bool ok;
    const std::string sdata = save(photoFormat, &ok);
    if (ok) {
        std::ofstream ofs(filename, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!ofs.is_open()) {
            m_data->error = Error::Uninitialised; // 0
            return false;
        }
#if (RAGEPHOTO_CXX_STD >= 11) && (__cplusplus >= 201103L)
        ofs << sdata;
#else
        ofs.write(sdata.data(), sdata.size());
#endif
        ok = ofs.good();
        ofs.close();
        return ok;
    }
    else
        return false;
}

bool RagePhoto::saveFile(const char *filename)
{
    return saveFile(filename, m_data->photoFormat);
}

size_t RagePhoto::saveSize(uint32_t photoFormat, RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser)
{
    if (photoFormat == PhotoFormat::GTA5)
        return (rp_data->jpegBuffer + rp_data->jsonBuffer + rp_data->titlBuffer + rp_data->descBuffer + GTA5_HEADERSIZE + UINT32_C(56));
    else if (photoFormat == PhotoFormat::RDR2)
        return (rp_data->jpegBuffer + rp_data->jsonBuffer + rp_data->titlBuffer + rp_data->descBuffer + RDR2_HEADERSIZE + UINT32_C(56));
    else if (rp_parser) {
        RagePhotoFormatParser n_parser[1]{};
        for (size_t i = 0; memcmp(&n_parser[0], &rp_parser[i], sizeof(RagePhotoFormatParser)); i++) {
            if (photoFormat == rp_parser[i].photoFormat)
                if (rp_parser[i].funcSaveSz)
                    return (rp_parser[i].funcSaveSz)(rp_data, photoFormat);
        }
    }
    return 0;
}

size_t RagePhoto::saveSize(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser)
{
    return saveSize(rp_data->photoFormat, rp_data, rp_parser);
}

size_t RagePhoto::saveSize(uint32_t photoFormat)
{
    return saveSize(photoFormat, m_data, m_parser);
}

size_t RagePhoto::saveSize()
{
    return saveSize(m_data->photoFormat, m_data, m_parser);
}

void RagePhoto::setBufferDefault(RagePhotoData *rp_data)
{
    rp_data->descBuffer = DEFAULT_DESCBUFFER;
    rp_data->jsonBuffer = DEFAULT_JSONBUFFER;
    rp_data->titlBuffer = DEFAULT_TITLBUFFER;
    setBufferOffsets(rp_data);
}

void RagePhoto::setBufferDefault()
{
    setBufferDefault(m_data);
}

void RagePhoto::setBufferOffsets(RagePhotoData *rp_data)
{
    rp_data->jsonOffset = rp_data->jpegBuffer + 28;
    rp_data->titlOffset = rp_data->jsonOffset + rp_data->jsonBuffer + 8;
    rp_data->descOffset = rp_data->titlOffset + rp_data->titlBuffer + 8;
    rp_data->endOfFile = rp_data->descOffset + rp_data->descBuffer + 12;
}

void RagePhoto::setBufferOffsets()
{
    setBufferOffsets(m_data);
}

bool RagePhoto::setData(RagePhotoData *rp_data, bool takeCopy)
{
    if (m_data == rp_data)
        return true;

    if (takeCopy) {
        clear(m_data);
        m_data->photoFormat = rp_data->photoFormat;

        if (rp_data->header) {
            const size_t headerSize = strlen(rp_data->header) + 1;
            m_data->header = static_cast<char*>(malloc(headerSize));
            if (!m_data->header)
                return false;
            memcpy(m_data->header, rp_data->header, headerSize);
            m_data->headerSum = rp_data->headerSum;
            m_data->headerSum2 = rp_data->headerSum2;
        }

        if (rp_data->jpeg) {
            m_data->jpeg = static_cast<char*>(malloc(rp_data->jpegSize));
            if (!m_data->jpeg)
                return false;
            memcpy(m_data->jpeg, rp_data->jpeg, rp_data->jpegSize);
            m_data->jpegSize = rp_data->jpegSize;
            m_data->jpegBuffer = rp_data->jpegBuffer;
        }

        if (rp_data->json) {
            const size_t jsonSize = strlen(rp_data->json) + 1;
            m_data->json = static_cast<char*>(malloc(jsonSize));
            if (!m_data->json)
                return false;
            memcpy(m_data->json, rp_data->json, jsonSize);
            m_data->jsonBuffer = rp_data->jsonBuffer;
        }

        if (rp_data->title) {
            const size_t titleSize = strlen(rp_data->title) + 1;
            m_data->title = static_cast<char*>(malloc(titleSize));
            if (!m_data->title)
                return false;
            memcpy(m_data->title, rp_data->title, titleSize);
            m_data->titlBuffer = rp_data->titlBuffer;
        }

        if (rp_data->description) {
            const size_t descriptionSize = strlen(rp_data->description) + 1;
            m_data->description = static_cast<char*>(malloc(descriptionSize));
            if (!m_data->description)
                return false;
            memcpy(m_data->description, rp_data->description, descriptionSize);
            m_data->descBuffer = rp_data->descBuffer;
        }

        setBufferOffsets(m_data);
    }
    else {
        free(m_data->jpeg);
        free(m_data->description);
        free(m_data->json);
        free(m_data->header);
        free(m_data->title);
        free(m_data);

        m_data = rp_data;
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
        setBufferOffsets(m_data);
    }
    m_data->error = Error::NoError; // 255
}

void RagePhoto::setFormat(uint32_t photoFormat)
{
    m_data->photoFormat = photoFormat;
}

void RagePhoto::setHeader(const char *header, uint32_t headerSum, uint32_t headerSum2)
{
    if (!writeDataChar(header, &m_data->header)) {
        m_data->error = Error::HeaderMallocError; // 4
        return;
    }
    m_data->headerSum = headerSum;
    m_data->headerSum2 = headerSum2;
    m_data->error = Error::NoError; // 255
}

bool RagePhoto::setJpeg(const char *data, uint32_t size, uint32_t bufferSize)
{
    if (m_data->jpeg) {
        if (m_data->jpegSize > size) {
            char *t_photoData = static_cast<char*>(realloc(m_data->jpeg, size));
            if (!t_photoData) {
                m_data->error = Error::PhotoMallocError; // 16
                return false;
            }
            m_data->jpeg = t_photoData;
            memcpy(m_data->jpeg, data, size);
            m_data->jpegSize = size;
        }
        else if (m_data->jpegSize < size) {
            free(m_data->jpeg);
            m_data->jpeg = static_cast<char*>(malloc(size));
            if (!m_data->jpeg) {
                m_data->error = Error::PhotoMallocError; // 16
                return false;
            }
            memcpy(m_data->jpeg, data, size);
            m_data->jpegSize = size;
        }
        else {
            memcpy(m_data->jpeg, data, size);
        }
    }
    else {
        m_data->jpeg = static_cast<char*>(malloc(size));
        if (!m_data->jpeg) {
            m_data->error = Error::PhotoMallocError; // 16
            return false;
        }
        memcpy(m_data->jpeg, data, size);
        m_data->jpegSize = size;
    }

    if (bufferSize != 0) {
        m_data->jpegBuffer = bufferSize;
        setBufferOffsets(m_data);
    }

    m_data->error = Error::NoError; // 255
    return true;
}

bool RagePhoto::setJpeg(const std::string &data, uint32_t bufferSize)
{
    return setJpeg(data.data(), static_cast<uint32_t>(data.size()), bufferSize);
}

void RagePhoto::setJson(const char *json, uint32_t bufferSize)
{
    if (!writeDataChar(json, &m_data->json)) {
        m_data->error = Error::JsonMallocError; // 21
        return;
    }
    if (bufferSize != 0) {
        m_data->jsonBuffer = bufferSize;
        setBufferOffsets(m_data);
    }
    m_data->error = Error::NoError; // 255
}

void RagePhoto::setLibraryFlag(RagePhotoLibraryFlag flag, bool state)
{
    state ? (libraryflags |= flag) : (libraryflags &= ~flag);
}

void RagePhoto::setTitle(const char *title, uint32_t bufferSize)
{
    if (!writeDataChar(title, &m_data->title)) {
        m_data->error = Error::TitleMallocError; // 26
        return;
    }
    if (bufferSize != 0) {
        m_data->titlBuffer = bufferSize;
        setBufferOffsets(m_data);
    }
    m_data->error = Error::NoError; // 255
}

#ifdef LIBRAGEPHOTO_CXX_C
ragephoto_t ragephoto_open()
{
    try {
        return static_cast<ragephoto_t>(new RagePhoto);
    }
    catch (const std::exception &exception) {
        std::cerr << "[libragephoto] Exception thrown at ragephoto_open: " << exception.what() << std::endl;
        return nullptr;
    }
}

void ragephoto_addparser(ragephoto_t instance, RagePhotoFormatParser *rp_parser)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    try {
        ragePhoto->addParser(rp_parser);
    }
    catch (const std::exception &exception) {
        std::cerr << "[libragephoto] Exception thrown at ragephoto_addparser: " << exception.what() << std::endl;
    }
}

void ragephoto_clear(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    ragePhoto->clear();
}

void ragephotodata_clear(RagePhotoData *rp_data)
{
    RagePhoto::clear(rp_data);
}

bool ragephoto_load(ragephoto_t instance, const char *data, size_t size)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->load(data, size);
}

bool ragephotodata_load(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser, const char *data, size_t size)
{
    return RagePhoto::load(data, size, rp_data, rp_parser);
}

bool ragephoto_loadfile(ragephoto_t instance, const char *filename)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->loadFile(filename);
}

int32_t ragephoto_error(ragephoto_t instance)
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
    return ragePhoto->jpegData();
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

uint64_t ragephoto_getphotosign(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->jpegSign();
}

uint64_t ragephoto_getphotosignf(ragephoto_t instance, uint32_t photoFormat)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->jpegSign(photoFormat);
}

void ragephoto_getphotosigns(ragephoto_t instance, char *data, size_t size)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    snprintf(data, size, "%" PRIu64, ragePhoto->jpegSign());
}

void ragephoto_getphotosignsf(ragephoto_t instance, char *data, size_t size, uint32_t photoFormat)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    snprintf(data, size, "%" PRIu64, ragePhoto->jpegSign(photoFormat));
}

uint64_t ragephotodata_getphotosign(RagePhotoData *rp_data)
{
    return RagePhoto::jpegSign(rp_data);
}

uint64_t ragephotodata_getphotosignf(RagePhotoData *rp_data, uint32_t photoFormat)
{
    return RagePhoto::jpegSign(photoFormat, rp_data);
}

uint32_t ragephoto_getphotosize(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->jpegSize();
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

size_t ragephotodata_getsavesize(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser)
{
    return RagePhoto::saveSize(rp_data, rp_parser);
}

size_t ragephoto_getsavesizef(ragephoto_t instance, uint32_t photoFormat)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->saveSize(photoFormat);
}

size_t ragephotodata_getsavesizef(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser, uint32_t photoFormat)
{
    return RagePhoto::saveSize(photoFormat, rp_data, rp_parser);
}

bool ragephoto_save(ragephoto_t instance, char *data)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->save(data);
}

bool ragephotodata_save(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser, char *data)
{
    return RagePhoto::save(data, rp_data->photoFormat, rp_data, rp_parser);
}

bool ragephoto_savef(ragephoto_t instance, char *data, uint32_t photoFormat)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->save(data, photoFormat);
}

bool ragephotodata_savef(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser, char *data, uint32_t photoFormat)
{
    return RagePhoto::save(data, photoFormat, rp_data, rp_parser);
}

bool ragephoto_savefile(ragephoto_t instance, const char *filename)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->saveFile(filename);
}

bool ragephoto_savefilef(ragephoto_t instance, const char *filename, uint32_t photoFormat)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->saveFile(filename, photoFormat);
}

void ragephoto_setbufferdefault(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    ragePhoto->setBufferDefault();
}

void ragephotodata_setbufferdefault(RagePhotoData *rp_data)
{
    RagePhoto::setBufferDefault(rp_data);
}

void ragephoto_setbufferoffsets(ragephoto_t instance)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    ragePhoto->setBufferOffsets();
}

void ragephotodata_setbufferoffsets(RagePhotoData *rp_data)
{
    RagePhoto::setBufferOffsets(rp_data);
}

void ragephoto_setlibraryflag(RagePhotoLibraryFlag flag, bool state)
{
    RagePhoto::setLibraryFlag(flag, state);
}

bool ragephoto_setphotodata(ragephoto_t instance, RagePhotoData *rp_data)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->setData(rp_data, false);
}

bool ragephoto_setphotodatac(ragephoto_t instance, RagePhotoData *rp_data)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->setData(rp_data, true);
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

bool ragephoto_setphotojpeg(ragephoto_t instance, const char *data, uint32_t size, uint32_t bufferSize)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    return ragePhoto->setJpeg(data, size, bufferSize);
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

void ragephoto_setphotoheader2(ragephoto_t instance, const char *header, uint32_t headerSum, uint32_t headerSum2)
{
    RagePhoto *ragePhoto = static_cast<RagePhoto*>(instance);
    ragePhoto->setHeader(header, headerSum, headerSum2);
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
