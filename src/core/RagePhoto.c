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

#include "RagePhoto.h"
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef RAGEPHOTO_BENCHMARK
#ifdef _WIN32
#include <windows.h>
#include <profileapi.h>
#else
#include <time.h>
#endif
#endif

#if defined(UNICODE_ICONV)
#include <iconv.h>
#elif defined(UNICODE_WINCVT)
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
            char *t_output = (char*)realloc(*output, src_s);
            if (!t_output) {
                return false;
            }
            *output = t_output;
            memcpy(*output, input, src_s);
        }
        else if (dst_s < src_s) {
            char *t_output = (char*)malloc(src_s);
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
        char *t_output = (char*)malloc(src_s);
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
    return ((unsigned char)(x[3]) << 24 |
            (unsigned char)(x[2]) << 16 |
            (unsigned char)(x[1]) << 8 |
            (unsigned char)(x[0]));
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
ragephoto_t ragephoto_open()
{
    RagePhotoInstance *instance = (RagePhotoInstance*)malloc(sizeof(RagePhotoInstance));
    if (!instance)
        return NULL;
    instance->data = (RagePhotoData*)malloc(sizeof(RagePhotoData));
    if (!instance->data) {
        free(instance);
        return NULL;
    }
    memset(instance->data, 0, sizeof(RagePhotoData));
    instance->parser = (RagePhotoFormatParser*)malloc(sizeof(RagePhotoFormatParser));
    if (!instance->parser) {
        free(instance->data);
        free(instance);
        return NULL;
    }
    memset(instance->parser, 0, sizeof(RagePhotoFormatParser));
    ragephotodata_setbufferdefault(instance->data);
    return (ragephoto_t)instance;
}

void ragephoto_close(ragephoto_t instance_t)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    free(instance->data->jpeg);
    free(instance->data->description);
    free(instance->data->json);
    free(instance->data->header);
    free(instance->data->title);
    free(instance->data);
    free(instance->parser);
    free(instance);
}

void ragephoto_addparser(ragephoto_t instance_t, RagePhotoFormatParser *rp_parser)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    if (rp_parser) {
        RagePhotoFormatParser n_parser;
        memset(&n_parser, 0, sizeof(RagePhotoFormatParser));
        if (!memcmp(&n_parser, rp_parser, sizeof(RagePhotoFormatParser)))
            return;
        size_t length;
        for (length = 0; memcmp(&n_parser, &instance->parser[length], sizeof(RagePhotoFormatParser)); length++);
        RagePhotoFormatParser *t_parser = (RagePhotoFormatParser*)realloc(instance->parser, (length + 2 * sizeof(RagePhotoFormatParser)));
        if (!t_parser)
            return;
        instance->parser = t_parser;
        memcpy(&instance->parser[length], rp_parser, sizeof(RagePhotoFormatParser));
        memset(&instance->parser[length + 1], 0, sizeof(RagePhotoFormatParser));
    }
}

void ragephotodata_clear(RagePhotoData *rp_data)
{
    free(rp_data->jpeg);
    free(rp_data->description);
    free(rp_data->json);
    free(rp_data->header);
    free(rp_data->title);
    memset(rp_data, 0, sizeof(RagePhotoData));
    ragephotodata_setbufferdefault(rp_data);
}

void ragephoto_clear(ragephoto_t instance_t)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    ragephotodata_clear(instance->data);
}

RagePhotoData* ragephoto_getphotodata(ragephoto_t instance_t)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    return instance->data;
}

bool ragephotodata_load(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser, const char *data, size_t length)
{
#ifdef RAGEPHOTO_BENCHMARK
#ifdef _WIN32
    LARGE_INTEGER freq, benchmark_parse_start, benchmark_parse_end;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&benchmark_parse_start);
#else
    struct timespec benchmark_parse_start, benchmark_parse_end;
    clock_gettime(CLOCK_MONOTONIC, &benchmark_parse_start);
#endif
#endif

    // Avoid data conflicts
    ragephotodata_clear(rp_data);

    size_t pos = 0;
    char uInt32Buffer[4];
    size_t size = readBuffer(data, uInt32Buffer, &pos, 4, length);
    if (size != 4) {
        rp_data->error = RAGEPHOTO_ERROR_NOFORMATIDENTIFIER; // 1
        return false;
    }

#if __BYTE_ORDER == __LITTLE_ENDIAN
    memcpy(&rp_data->photoFormat, uInt32Buffer, 4);
#else
    rp_data->photoFormat = charToUInt32LE(uInt32Buffer);
#endif
    if (rp_data->photoFormat == RAGEPHOTO_FORMAT_GTA5 || rp_data->photoFormat == RAGEPHOTO_FORMAT_RDR2) {
#if defined(UNICODE_ICONV) || defined(UNICODE_WINCVT)
        char photoHeader[256];
        size = readBuffer(data, photoHeader, &pos, 256, length);
        if (size != 256) {
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETEHEADER; // 3
            return false;
        }

#if defined(UNICODE_ICONV)
        iconv_t iconv_in = iconv_open("UTF-8", "UTF-16LE");
        if (iconv_in == (iconv_t)-1) {
            rp_data->error = RAGEPHOTO_ERROR_UNICODEINITERROR; // 4
            return false;
        }
        rp_data->header = (char*)malloc(256);
        if (!rp_data->header) {
            rp_data->error = RAGEPHOTO_ERROR_HEADERMALLOCERROR; // 4
            iconv_close(iconv_in);
            return false;
        }
        size_t src_s = sizeof(photoHeader);
        size_t dst_s = 256;
        char *src = photoHeader;
        char *dst = rp_data->header;
        const size_t ret = iconv(iconv_in, &src, &src_s, &dst, &dst_s);
        iconv_close(iconv_in);
        if (ret == (size_t)-1) {
            rp_data->error = RAGEPHOTO_ERROR_UNICODEHEADERERROR; // 6
            return false;
        }
#elif defined(UNICODE_WINCVT)
        rp_data->header = (char*)malloc(256);
        if (!rp_data->header) {
            rp_data->error = RAGEPHOTO_ERROR_HEADERMALLOCERROR; // 4
            return false;
        }
        const int converted = WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)photoHeader, -1, rp_data->header, 256, NULL, NULL);
        if (converted == 0) {
            free(rp_data->header);
            rp_data->header = NULL;
            rp_data->error = RAGEPHOTO_ERROR_UNICODEHEADERERROR; // 6
            return false;
        }
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETECHECKSUM; // 7
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->headerSum, uInt32Buffer, 4);
#else
        rp_data->headerSum = charToUInt32LE(uInt32Buffer);
#endif

        if (rp_data->photoFormat == RAGEPHOTO_FORMAT_RDR2) {
            char formatCheckBuffer[4];
            size = readBuffer(data, formatCheckBuffer, &pos, 4, length);
            if (size != 4) {
                rp_data->error = RAGEPHOTO_ERROR_INCOMPLETECHECKSUM; // 7
                return false;
            }
            char n_formatCheckBuffer[4];
            memset(&n_formatCheckBuffer, 0, 4);
            if (memcmp(formatCheckBuffer, n_formatCheckBuffer, 4)) {
                rp_data->error = RAGEPHOTO_ERROR_INCOMPATIBLEFORMAT; // 2
                return false;
            }

            size = readBuffer(data, uInt32Buffer, &pos, 4, length);
            if (size != 4) {
                rp_data->error = RAGEPHOTO_ERROR_INCOMPLETECHECKSUM; // 7
                return false;
            }
#if __BYTE_ORDER == __LITTLE_ENDIAN
            memcpy(&rp_data->headerSum2, uInt32Buffer, 4);
#else
            rp_data->headerSum2 = charToUInt32LE(uInt32Buffer);
#endif
        }
        const size_t headerSize = pos;

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETEEOF; // 8
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->endOfFile, uInt32Buffer, 4);
#else
        rp_data->endOfFile = charToUInt32LE(uInt32Buffer);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETEJSONOFFSET; // 9
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->jsonOffset, uInt32Buffer, 4);
#else
        rp_data->jsonOffset = charToUInt32LE(uInt32Buffer);
#endif
        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETETITLEOFFSET; // 10
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->titlOffset, uInt32Buffer, 4);
#else
        rp_data->titlOffset = charToUInt32LE(uInt32Buffer);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETEDESCOFFSET; // 11
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
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETEJPEGMARKER; // 12
            return false;
        }
        if (memcmp(markerBuffer, "JPEG", 4)) {
            rp_data->error = RAGEPHOTO_ERROR_INCORRECTJPEGMARKER; // 13
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETEPHOTOBUFFER; // 14
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->jpegBuffer, uInt32Buffer, 4);
#else
        rp_data->jpegBuffer = charToUInt32LE(uInt32Buffer);
#endif

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETEPHOTOSIZE; // 15
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->jpegSize, uInt32Buffer, 4);
#else
        rp_data->jpegSize = charToUInt32LE(uInt32Buffer);
#endif

        rp_data->jpeg = (char*)malloc(rp_data->jpegSize);
        if (!rp_data->jpeg) {
            rp_data->error = RAGEPHOTO_ERROR_PHOTOMALLOCERROR; // 16
            return false;
        }
        size = readBuffer(data, rp_data->jpeg, &pos, rp_data->jpegSize, length);
        if (size != rp_data->jpegSize) {
            free(rp_data->jpeg);
            rp_data->jpeg = NULL;
            rp_data->error = RAGEPHOTO_ERROR_PHOTOREADERROR; // 17
            return false;
        }

        pos += rp_data->jpegBuffer - rp_data->jpegSize;
        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETEJSONMARKER; // 18
            return false;
        }
        if (memcmp(markerBuffer, "JSON", 4)) {
            rp_data->error = RAGEPHOTO_ERROR_INCORRECTJSONMARKER; // 19
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETEJSONBUFFER; // 20
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->jsonBuffer, uInt32Buffer, 4);
#else
        rp_data->jsonBuffer = charToUInt32LE(uInt32Buffer);
#endif

        rp_data->json = (char*)malloc(rp_data->jsonBuffer);
        if (!rp_data->json) {
            rp_data->error = RAGEPHOTO_ERROR_JSONMALLOCERROR; // 21
            return false;
        }
        size = readBuffer(data, rp_data->json, &pos, rp_data->jsonBuffer, length);
        if (size != rp_data->jsonBuffer) {
            free(rp_data->json);
            rp_data->json = NULL;
            rp_data->error = RAGEPHOTO_ERROR_JSONREADERROR; // 22
            return false;
        }

        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETETITLEMARKER; // 23
            return false;
        }
        if (memcmp(markerBuffer, "TITL", 4)) {
            rp_data->error = RAGEPHOTO_ERROR_INCORRECTTITLEMARKER; // 24
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETETITLEBUFFER; // 25
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->titlBuffer, uInt32Buffer, 4);
#else
        rp_data->titlBuffer = charToUInt32LE(uInt32Buffer);
#endif

        rp_data->title = (char*)malloc(rp_data->titlBuffer);
        if (!rp_data->title) {
            rp_data->error = RAGEPHOTO_ERROR_TITLEMALLOCERROR; // 26
            return false;
        }
        size = readBuffer(data, rp_data->title, &pos, rp_data->titlBuffer, length);
        if (size != rp_data->titlBuffer) {
            free(rp_data->title);
            rp_data->title = NULL;
            rp_data->error = RAGEPHOTO_ERROR_TITLEREADERROR; // 27
            return false;
        }

        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETEDESCMARKER; // 28
            return false;
        }
        if (memcmp(markerBuffer, "DESC", 4)) {
            rp_data->error = RAGEPHOTO_ERROR_INCORRECTDESCMARKER; // 29
            return false;
        }

        size = readBuffer(data, uInt32Buffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETEDESCBUFFER; // 30
            return false;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(&rp_data->descBuffer, uInt32Buffer, 4);
#else
        rp_data->descBuffer = charToUInt32LE(uInt32Buffer);
#endif

        rp_data->description = (char*)malloc(rp_data->descBuffer);
        if (!rp_data->description) {
            rp_data->error = RAGEPHOTO_ERROR_DESCMALLOCERROR; // 31
            return false;
        }
        size = readBuffer(data, rp_data->description, &pos, rp_data->descBuffer, length);
        if (size != rp_data->descBuffer) {
            free(rp_data->description);
            rp_data->description = NULL;
            rp_data->error = RAGEPHOTO_ERROR_DESCREADERROR; // 32
            return false;
        }

        size = readBuffer(data, markerBuffer, &pos, 4, length);
        if (size != 4) {
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETEJENDMARKER; // 33
            return false;
        }
        if (memcmp(markerBuffer, "JEND", 4)) {
            rp_data->error = RAGEPHOTO_ERROR_INCORRECTJENDMARKER; // 34
            return false;
        }

#ifdef RAGEPHOTO_BENCHMARK
#ifdef _WIN32
        QueryPerformanceCounter(&benchmark_parse_end);
        const uint64_t benchmark_ns = (benchmark_parse_end.QuadPart - benchmark_parse_start.QuadPart) * INT64_C(1000000000) / freq.QuadPart;
#else
        clock_gettime(CLOCK_MONOTONIC, &benchmark_parse_end);
        const uint64_t benchmark_ns = (UINT64_C(1000000000) * benchmark_parse_end.tv_sec + benchmark_parse_end.tv_nsec) -
                                      (UINT64_C(1000000000) * benchmark_parse_start.tv_sec + benchmark_parse_start.tv_nsec);
#endif
        printf("Benchmark: %" PRIu64 "ns\n", benchmark_ns);
#endif

#ifdef RAGEPHOTO_DEBUG
        const uint32_t jsonOffset = rp_data->jpegBuffer + UINT32_C(28);
        const uint32_t titlOffset = jsonOffset + rp_data->jsonBuffer + UINT32_C(8);
        const uint32_t descOffset = titlOffset + rp_data->titlBuffer + UINT32_C(8);
        const uint32_t endOfFile = descOffset + rp_data->descBuffer + UINT32_C(12);
        printf("header: %s\n", rp_data->header);
        printf("headerSum: %" PRIu32 "\n", rp_data->headerSum);
        printf("headerSum2: %" PRIu32 "\n", rp_data->headerSum2);
        printf("photoBuffer: %" PRIu32 "\n", rp_data->jpegBuffer);
        printf("descBuffer: %" PRIu32 "\n", rp_data->descBuffer);
        printf("descOffset: %" PRIu32 " (%" PRIu32 ")\n", rp_data->descOffset, descOffset);
        printf("description: %s\n", rp_data->description);
        printf("jsonBuffer: %" PRIu32 "\n", rp_data->jsonBuffer);
        printf("jsonOffset: %" PRIu32 " (%" PRIu32 ")\n", rp_data->jsonOffset, jsonOffset);
        printf("json: %s\n", rp_data->json);
        printf("sign: %" PRIu64 "\n", ragephotodata_getphotosign(rp_data));
        printf("titlBuffer: %" PRIu32 "\n", rp_data->titlBuffer);
        printf("titlOffset: %" PRIu32 " (%" PRIu32 ")\n", rp_data->titlOffset, titlOffset);
        printf("title: %s\n", rp_data->title);
        printf("eofOffset: %" PRIu32 " (%" PRIu32 ")\n", rp_data->endOfFile, endOfFile);
        printf("size: %zu / %zu\n", length, ragephotodata_getsavesize(rp_data, NULL));
#endif

        rp_data->error = RAGEPHOTO_ERROR_NOERROR; // 255
        return true;
#else
        printf("UTF-16LE decoding support missing\n");
        rp_data->error = RAGEPHOTO_ERROR_UNICODEINITERROR; // 4
        return false;
#endif
    }
    else if (rp_parser) {
        RagePhotoFormatParser n_parser;
        memset(&n_parser, 0, sizeof(RagePhotoFormatParser));
        for (size_t i = 0; memcmp(&n_parser, &rp_parser[i], sizeof(RagePhotoFormatParser)); i++) {
            if (rp_data->photoFormat == rp_parser[i].photoFormat)
                if (rp_parser[i].funcLoad)
                    return (rp_parser[i].funcLoad)(rp_data, data, length);
        }
    }
    rp_data->error = RAGEPHOTO_ERROR_INCOMPATIBLEFORMAT; // 2
    return false;
}

bool ragephoto_load(ragephoto_t instance_t, const char *data, size_t size)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    return ragephotodata_load(instance->data, instance->parser, data, size);
}

bool ragephoto_loadfile(ragephoto_t instance_t, const char *filename)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
#if defined(_WIN32)
    FILE *file = NULL;
    fopen_s(&file, filename, "rb");
#else
    FILE *file = fopen(filename, "rb");
#endif
    if (!file)
        return false;
#if defined(_WIN64)
    int fseek_ret = _fseeki64(file, 0, SEEK_END);
#elif (defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64) || (defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L)
    int fseek_ret = fseeko(file, 0, SEEK_END);
#else
    int fseek_ret = fseek(file, 0, SEEK_END);
#endif
    if (fseek_ret == -1) {
        fclose(file);
        return false;
    }
#if defined(_WIN64)
    const long long fileSize = _ftelli64(file);
#elif (defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64) || (defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L)
    const _off_t fileSize = ftello(file);
#else
    const long fileSize = ftell(file);
#endif
    if (fileSize == -1) {
        fclose(file);
        return false;
    }
#if defined(_WIN64)
    fseek_ret = _fseeki64(file, 0, SEEK_SET);
#elif (defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64) || (defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L)
    fseek_ret = fseeko(file, 0, SEEK_SET);
#else
    fseek_ret = fseek(file, 0, SEEK_SET);
#endif
    if (fseek_ret == -1) {
        fclose(file);
        return false;
    }
    char *data = (char*)malloc(fileSize);
    if (!data) {
        fclose(file);
        return false;
    }
    const size_t readSize = fread(data, sizeof(char), fileSize, file);
    fclose(file);
    if (fileSize != readSize) {
        free(data);
        return false;
    }
    bool isLoaded = ragephotodata_load(instance->data, instance->parser, data, fileSize);
    free(data);
    return isLoaded;
}

int32_t ragephoto_error(ragephoto_t instance_t)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    return instance->data->error;
}

uint32_t ragephoto_getphotoformat(ragephoto_t instance_t)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    return instance->data->photoFormat;
}

const char* ragephoto_getphotojpeg(ragephoto_t instance_t)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    if (instance->data->jpeg)
        return instance->data->jpeg;
    return "";
}

uint64_t ragephotodata_getphotosignf(RagePhotoData *rp_data, uint32_t photoFormat)
{
    if (rp_data->jpeg) {
        if (photoFormat == RAGEPHOTO_FORMAT_GTA5)
            return (UINT64_C(0x100000000000000) | joaatFromInitial(rp_data->jpeg, rp_data->jpegSize, RAGEPHOTO_SIGNINITIAL_GTA5));
        else if (photoFormat == RAGEPHOTO_FORMAT_RDR2)
            return (UINT64_C(0x100000000000000) | joaatFromInitial(rp_data->jpeg, rp_data->jpegSize, RAGEPHOTO_SIGNINITIAL_RDR2));
    }
    return 0;
}

uint64_t ragephotodata_getphotosign(RagePhotoData *rp_data)
{
    return ragephotodata_getphotosignf(rp_data, rp_data->photoFormat);
}

uint64_t ragephoto_getphotosignf(ragephoto_t instance_t, uint32_t photoFormat)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    return ragephotodata_getphotosignf(instance->data, photoFormat);
}

uint64_t ragephoto_getphotosign(ragephoto_t instance_t)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    return ragephotodata_getphotosignf(instance->data, instance->data->photoFormat);
}

void ragephoto_getphotosigns(ragephoto_t instance, char *data, size_t size)
{
    snprintf(data, size, "%" PRIu64, ragephoto_getphotosign(instance));
}

void ragephoto_getphotosignsf(ragephoto_t instance, char *data, size_t size, uint32_t photoFormat)
{
    snprintf(data, size, "%" PRIu64, ragephoto_getphotosignf(instance, photoFormat));
}

uint32_t ragephoto_getphotosize(ragephoto_t instance_t)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    if (instance->data->jpeg)
        return instance->data->jpegSize;
    return 0;
}

const char* ragephoto_getphotodesc(ragephoto_t instance_t)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    if (instance->data->description)
        return instance->data->description;
    return "";
}

const char* ragephoto_getphotojson(ragephoto_t instance_t)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    if (instance->data->json)
        return instance->data->json;
    return "";
}

const char* ragephoto_getphotoheader(ragephoto_t instance_t)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    if (instance->data->header)
        return instance->data->header;
    return "";
}

const char* ragephoto_getphototitle(ragephoto_t instance_t)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    if (instance->data->title)
        return instance->data->title;
    return "";
}

const char* ragephoto_version()
{
    return RAGEPHOTO_VERSION;
}

bool ragephotodata_savef(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser, char *data, uint32_t photoFormat)
{
    if (photoFormat == RAGEPHOTO_FORMAT_GTA5 || photoFormat == RAGEPHOTO_FORMAT_RDR2) {
        if (!rp_data->header) {
            rp_data->error = RAGEPHOTO_ERROR_INCOMPLETEHEADER; // 3
            return false;
        }
#if defined(UNICODE_ICONV) || defined(UNICODE_WINCVT)
#if defined(UNICODE_ICONV)
        iconv_t iconv_in = iconv_open("UTF-16LE", "UTF-8");
        if (iconv_in == (iconv_t)-1) {
            rp_data->error = RAGEPHOTO_ERROR_UNICODEINITERROR; // 4
            return false;
        }
        char photoHeader[256];
        memset(&photoHeader, 0, 256);
        size_t src_s = strlen(rp_data->header);
        size_t dst_s = sizeof(photoHeader);
        char *src = rp_data->header;
        char *dst = photoHeader;
        const size_t ret = iconv(iconv_in, &src, &src_s, &dst, &dst_s);
        iconv_close(iconv_in);
        if (ret == (size_t)-1) {
            rp_data->error = RAGEPHOTO_ERROR_UNICODEHEADERERROR; // 6
            return false;
        }
        const size_t photoHeader_size = 256;
#elif defined(UNICODE_WINCVT)
        char photoHeader[256];
        memset(&photoHeader, 0, 256);
        const int converted = MultiByteToWideChar(CP_UTF8, 0, rp_data->header, (int)strlen(rp_data->header), (wchar_t*)photoHeader, 256 / sizeof(wchar_t));
        if (converted == 0) {
            rp_data->error = RAGEPHOTO_ERROR_UNICODEHEADERERROR; // 6
            return false;
        }
        const size_t photoHeader_size = 256;
#endif

        const size_t length = ragephotodata_getsavesizef(rp_data, NULL, photoFormat);
        size_t pos = 0;
        char uInt32Buffer[4];

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &photoFormat, 4);
#else
        uInt32ToCharLE(photoFormat, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer(photoHeader, data, &pos, length, photoHeader_size);
        zeroBuffer(data, &pos, length, 256 - photoHeader_size);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &rp_data->headerSum, 4);
#else
        uInt32ToCharLE(rp_data->headerSum, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        if (photoFormat == RAGEPHOTO_FORMAT_RDR2) {
            char n_formatCheckBuffer[4];
            memset(&n_formatCheckBuffer, 0, 4);
            writeBuffer(n_formatCheckBuffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
            memcpy(uInt32Buffer, &rp_data->headerSum2, 4);
#else
            uInt32ToCharLE(rp_data->headerSum2, uInt32Buffer);
#endif
            writeBuffer(uInt32Buffer, data, &pos, length, 4);
        }

        const uint32_t jsonOffset = rp_data->jpegBuffer + UINT32_C(28);
        const uint32_t titlOffset = jsonOffset + rp_data->jsonBuffer + UINT32_C(8);
        const uint32_t descOffset = titlOffset + rp_data->titlBuffer + UINT32_C(8);
        const uint32_t endOfFile = descOffset + rp_data->descBuffer + UINT32_C(12);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &endOfFile, 4);
#else
        uInt32ToCharLE(endOfFile, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &jsonOffset, 4);
#else
        uInt32ToCharLE(jsonOffset, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &titlOffset, 4);
#else
        uInt32ToCharLE(titlOffset, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &descOffset, 4);
#else
        uInt32ToCharLE(descOffset, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        writeBuffer("JPEG", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &rp_data->jpegBuffer, 4);
#else
        uInt32ToCharLE(rp_data->jpegBuffer, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &rp_data->jpegSize, 4);
#else
        uInt32ToCharLE(rp_data->jpegSize, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        if (rp_data->jpeg) {
            if (rp_data->jpegSize > rp_data->jpegBuffer) {
                rp_data->error = RAGEPHOTO_ERROR_PHOTOBUFFERTIGHT; // 36
                return false;
            }
            writeBuffer(rp_data->jpeg, data, &pos, length, rp_data->jpegSize);
            zeroBuffer(data, &pos, length, rp_data->jpegBuffer - rp_data->jpegSize);
        }
        else {
            zeroBuffer(data, &pos, length, rp_data->jpegBuffer - rp_data->jpegSize);
        }

        writeBuffer("JSON", data, &pos, length, 4);

#if __BYTE_ORDER == __LITTLE_ENDIAN
        memcpy(uInt32Buffer, &rp_data->jsonBuffer, 4);
#else
        uInt32ToCharLE(rp_data->jsonBuffer, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        if (rp_data->json) {
            const size_t jsonString_size = strlen(rp_data->json) + 1;
            if (jsonString_size > rp_data->jsonBuffer) {
                rp_data->error = RAGEPHOTO_ERROR_JSONBUFFERTIGHT; // 37
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
        memcpy(uInt32Buffer, &rp_data->titlBuffer, 4);
#else
        uInt32ToCharLE(rp_data->titlBuffer, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        if (rp_data->title) {
            const size_t titlString_size = strlen(rp_data->title) + 1;
            if (titlString_size > rp_data->titlBuffer) {
                rp_data->error = RAGEPHOTO_ERROR_TITLEBUFFERTIGHT; // 38
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
        memcpy(uInt32Buffer, &rp_data->descBuffer, 4);
#else
        uInt32ToCharLE(rp_data->descBuffer, uInt32Buffer);
#endif
        writeBuffer(uInt32Buffer, data, &pos, length, 4);

        if (rp_data->description) {
            const size_t descString_size = strlen(rp_data->description) + 1;
            if (descString_size > rp_data->descBuffer) {
                rp_data->error = RAGEPHOTO_ERROR_DESCBUFFERTIGHT; // 39
                return false;
            }
            writeBuffer(rp_data->description, data, &pos, length, descString_size);
            zeroBuffer(data, &pos, length, rp_data->descBuffer - descString_size);
        }
        else {
            zeroBuffer(data, &pos, length, rp_data->descBuffer);
        }

        writeBuffer("JEND", data, &pos, length, 4);

        rp_data->error = RAGEPHOTO_ERROR_NOERROR; // 255
        return true;
#else
        printf("UTF-16LE encoding support missing\n");
        rp_data->error = RAGEPHOTO_ERROR_UNICODEINITERROR; // 4
        return false;
#endif
    }
    else if (rp_parser) {
        RagePhotoFormatParser n_parser;
        memset(&n_parser, 0, sizeof(RagePhotoFormatParser));
        for (size_t i = 0; memcmp(&n_parser, &rp_parser[i], sizeof(RagePhotoFormatParser)); i++) {
            if (photoFormat == rp_parser[i].photoFormat)
                if (rp_parser[i].funcSave)
                    return (rp_parser[i].funcSave)(rp_data, data, photoFormat);
        }
    }

    rp_data->error = RAGEPHOTO_ERROR_INCOMPATIBLEFORMAT; // 2
    return false;
}

bool ragephotodata_save(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser, char *data)
{
    return ragephotodata_savef(rp_data, rp_parser, data, rp_data->photoFormat);
}

bool ragephoto_savef(ragephoto_t instance_t, char *data, uint32_t photoFormat)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    return ragephotodata_savef(instance->data, instance->parser, data, photoFormat);
}

bool ragephoto_save(ragephoto_t instance_t, char *data)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    return ragephotodata_savef(instance->data, instance->parser, data, instance->data->photoFormat);
}

bool ragephoto_savefilef(ragephoto_t instance_t, const char *filename, uint32_t photoFormat)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    const size_t fileSize = ragephotodata_getsavesizef(instance->data, instance->parser, photoFormat);
    char *data = (char*)malloc(fileSize);
    if (!data)
        return false;
    if (!ragephotodata_savef(instance->data, instance->parser, data, photoFormat)) {
        free(data);
        return false;
    }
#ifdef _WIN32
    FILE *file = NULL;
    fopen_s(&file, filename, "wb");
#else
    FILE *file = fopen(filename, "wb");
#endif
    if (!file) {
        free(data);
        return false;
    }
    const size_t writeSize = fwrite(data, sizeof(char), fileSize, file);
    fclose(file);
    free(data);
    return (fileSize == writeSize);
}

bool ragephoto_savefile(ragephoto_t instance_t, const char *filename)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    return ragephoto_savefilef(instance_t, filename, instance->data->photoFormat);
}

size_t ragephotodata_getsavesizef(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser, uint32_t photoFormat)
{
    if (photoFormat == RAGEPHOTO_FORMAT_GTA5)
        return (rp_data->jpegBuffer + rp_data->jsonBuffer + rp_data->titlBuffer + rp_data->descBuffer + RAGEPHOTO_GTA5_HEADERSIZE + UINT32_C(56));
    else if (photoFormat == RAGEPHOTO_FORMAT_RDR2)
        return (rp_data->jpegBuffer + rp_data->jsonBuffer + rp_data->titlBuffer + rp_data->descBuffer + RAGEPHOTO_RDR2_HEADERSIZE + UINT32_C(56));
    else if (rp_parser) {
        RagePhotoFormatParser n_parser;
        memset(&n_parser, 0, sizeof(RagePhotoFormatParser));
        for (size_t i = 0; memcmp(&n_parser, &rp_parser[i], sizeof(RagePhotoFormatParser)); i++) {
            if (photoFormat == rp_parser[i].photoFormat)
                if (rp_parser[i].funcSaveSz)
                    return (rp_parser[i].funcSaveSz)(rp_data, photoFormat);
        }
    }
    return 0;
}

size_t ragephotodata_getsavesize(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser)
{
    return ragephotodata_getsavesizef(rp_data, rp_parser, rp_data->photoFormat);
}

size_t ragephoto_getsavesizef(ragephoto_t instance_t, uint32_t photoFormat)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    return ragephotodata_getsavesizef(instance->data, instance->parser, photoFormat);
}

size_t ragephoto_getsavesize(ragephoto_t instance_t)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    return ragephotodata_getsavesizef(instance->data, instance->parser, instance->data->photoFormat);
}

void ragephotodata_setbufferdefault(RagePhotoData *rp_data)
{
    rp_data->descBuffer = RAGEPHOTO_DEFAULT_DESCBUFFER;
    rp_data->jsonBuffer = RAGEPHOTO_DEFAULT_JSONBUFFER;
    rp_data->titlBuffer = RAGEPHOTO_DEFAULT_TITLBUFFER;
    ragephotodata_setbufferoffsets(rp_data);
}

void ragephoto_setbufferdefault(ragephoto_t instance_t)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    ragephotodata_setbufferdefault(instance->data);
}

void ragephotodata_setbufferoffsets(RagePhotoData *rp_data)
{
    rp_data->jsonOffset = rp_data->jpegBuffer + 28;
    rp_data->titlOffset = rp_data->jsonOffset + rp_data->jsonBuffer + 8;
    rp_data->descOffset = rp_data->titlOffset + rp_data->titlBuffer + 8;
    rp_data->endOfFile = rp_data->descOffset + rp_data->descBuffer + 12;
}

void ragephoto_setbufferoffsets(ragephoto_t instance_t)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    ragephotodata_setbufferoffsets(instance->data);
}

bool ragephoto_setphotodata(ragephoto_t instance_t, RagePhotoData *rp_data)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    if (instance->data == rp_data)
        return true;

    free(instance->data->jpeg);
    free(instance->data->description);
    free(instance->data->json);
    free(instance->data->header);
    free(instance->data->title);
    free(instance->data);

    instance->data = rp_data;
    return true;
}

bool ragephoto_setphotodatac(ragephoto_t instance_t, RagePhotoData *rp_data)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    if (instance->data == rp_data)
        return true;

    ragephotodata_clear(instance->data);
    instance->data->photoFormat = rp_data->photoFormat;

    if (rp_data->header) {
        const size_t headerSize = strlen(rp_data->header) + 1;
        instance->data->header = (char*)malloc(headerSize);
        if (!instance->data->header)
            return false;
        memcpy(instance->data->header, rp_data->header, headerSize);
        instance->data->headerSum = rp_data->headerSum;
        instance->data->headerSum2 = rp_data->headerSum2;
    }

    if (rp_data->jpeg) {
        instance->data->jpeg = (char*)malloc(rp_data->jpegSize);
        if (!instance->data->jpeg)
            return false;
        memcpy(instance->data->jpeg, rp_data->jpeg, rp_data->jpegSize);
        instance->data->jpegSize = rp_data->jpegSize;
        instance->data->jpegBuffer = rp_data->jpegBuffer;
    }

    if (rp_data->json) {
        const size_t jsonSize = strlen(rp_data->json) + 1;
        instance->data->json = (char*)malloc(jsonSize);
        if (!instance->data->json)
            return false;
        memcpy(instance->data->json, rp_data->json, jsonSize);
        instance->data->jsonBuffer = rp_data->jsonBuffer;
    }

    if (rp_data->title) {
        const size_t titleSize = strlen(rp_data->title) + 1;
        instance->data->title = (char*)malloc(titleSize);
        if (!instance->data->title)
            return false;
        memcpy(instance->data->title, rp_data->title, titleSize);
        instance->data->titlBuffer = rp_data->titlBuffer;
    }

    if (rp_data->description) {
        const size_t descriptionSize = strlen(rp_data->description) + 1;
        instance->data->description = (char*)malloc(descriptionSize);
        if (!instance->data->description)
            return false;
        memcpy(instance->data->description, rp_data->description, descriptionSize);
        instance->data->descBuffer = rp_data->descBuffer;
    }

    ragephotodata_setbufferoffsets(instance->data);
    return true;
}

void ragephoto_setphotodesc(ragephoto_t instance_t, const char *description, uint32_t bufferSize)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    if (!writeDataChar(description, &instance->data->description)) {
        instance->data->error = RAGEPHOTO_ERROR_DESCMALLOCERROR; // 31
        return;
    }
    if (bufferSize != 0) {
        instance->data->descBuffer = bufferSize;
        ragephoto_setbufferoffsets(instance->data);
    }
    instance->data->error = RAGEPHOTO_ERROR_NOERROR; // 255
}

void ragephoto_setphotoformat(ragephoto_t instance_t, uint32_t photoFormat)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    instance->data->photoFormat = photoFormat;
}

bool ragephoto_setphotojpeg(ragephoto_t instance_t, const char *data, uint32_t size, uint32_t bufferSize)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    if (instance->data->jpeg) {
        if (instance->data->jpegSize > size) {
            char *t_photoData = (char*)realloc(instance->data->jpeg, size);
            if (!t_photoData) {
                instance->data->error = RAGEPHOTO_ERROR_PHOTOMALLOCERROR; // 16
                return false;
            }
            instance->data->jpeg = t_photoData;
            memcpy(instance->data->jpeg, data, size);
            instance->data->jpegSize = size;
        }
        else if (instance->data->jpegSize < size) {
            free(instance->data->jpeg);
            instance->data->jpeg = (char*)malloc(size);
            if (!instance->data->jpeg) {
                instance->data->error = RAGEPHOTO_ERROR_PHOTOMALLOCERROR; // 16
                return false;
            }
            memcpy(instance->data->jpeg, data, size);
            instance->data->jpegSize = size;
        }
        else {
            memcpy(instance->data->jpeg, data, size);
        }
    }
    else {
        instance->data->jpeg = (char*)malloc(size);
        if (!instance->data->jpeg) {
            instance->data->error = RAGEPHOTO_ERROR_PHOTOMALLOCERROR; // 16
            return false;
        }
        memcpy(instance->data->jpeg, data, size);
        instance->data->jpegSize = size;
    }

    if (bufferSize != 0) {
        instance->data->jpegBuffer = bufferSize;
        ragephotodata_setbufferoffsets(instance->data);
    }

    instance->data->error = RAGEPHOTO_ERROR_NOERROR; // 255
    return true;
}

void ragephoto_setphotojson(ragephoto_t instance_t, const char *json, uint32_t bufferSize)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    if (!writeDataChar(json, &instance->data->json)) {
        instance->data->error = RAGEPHOTO_ERROR_JSONMALLOCERROR; // 21
        return;
    }
    if (bufferSize != 0) {
        instance->data->jsonBuffer = bufferSize;
        ragephotodata_setbufferoffsets(instance->data);
    }
    instance->data->error = RAGEPHOTO_ERROR_NOERROR; // 255
}

void ragephoto_setphotoheader(ragephoto_t instance_t, const char *header, uint32_t headerSum)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    if (!writeDataChar(header, &instance->data->header)) {
        instance->data->error = RAGEPHOTO_ERROR_HEADERMALLOCERROR; // 4
        return;
    }
    instance->data->headerSum = headerSum;
    instance->data->headerSum2 = 0;
    instance->data->error = RAGEPHOTO_ERROR_NOERROR; // 255
}

void ragephoto_setphotoheader2(ragephoto_t instance_t, const char *header, uint32_t headerSum, uint32_t headerSum2)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    if (!writeDataChar(header, &instance->data->header)) {
        instance->data->error = RAGEPHOTO_ERROR_HEADERMALLOCERROR; // 4
        return;
    }
    instance->data->headerSum = headerSum;
    instance->data->headerSum2 = headerSum2;
    instance->data->error = RAGEPHOTO_ERROR_NOERROR; // 255
}

void ragephoto_setphototitle(ragephoto_t instance_t, const char *title, uint32_t bufferSize)
{
    RagePhotoInstance *instance = (RagePhotoInstance*)instance_t;
    if (!writeDataChar(title, &instance->data->title)) {
        instance->data->error = RAGEPHOTO_ERROR_TITLEMALLOCERROR; // 26
        return;
    }
    if (bufferSize != 0) {
        instance->data->titlBuffer = bufferSize;
        ragephotodata_setbufferoffsets(instance->data);
    }
    instance->data->error = RAGEPHOTO_ERROR_NOERROR; // 255
}

uint32_t ragephoto_defpbuf_gta5()
{
    return RAGEPHOTO_DEFAULT_GTA5_PHOTOBUFFER;
}

uint32_t ragephoto_defpbuf_rdr2()
{
    return RAGEPHOTO_DEFAULT_RDR2_PHOTOBUFFER;
}

uint32_t ragephoto_format_gta5()
{
    return RAGEPHOTO_FORMAT_GTA5;
}

uint32_t ragephoto_format_rdr2()
{
    return RAGEPHOTO_FORMAT_RDR2;
}
/* END OF RAGEPHOTO CLASS */
