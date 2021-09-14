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
    /** Parsing and set errors */
    enum class Error : uint8_t {
        DescMallocError = 30,
        DescReadError = 31,
        IncompatibleFormat = 2,
        IncompleteChecksum = 6,
        IncompleteDescBuffer = 29,
        IncompleteDescMarker = 27,
        IncompleteDescOffset = 10,
        IncompleteEOF = 7,
        IncompleteHeader = 3,
        IncompleteJendMarker = 32,
        IncompleteJpegMarker = 11,
        IncompleteJsonBuffer = 19,
        IncompleteJsonMarker = 17,
        IncompleteJsonOffset = 8,
        IncompletePhotoBuffer = 13,
        IncompletePhotoSize = 14,
        IncompleteTitleBuffer = 24,
        IncompleteTitleMarker = 22,
        IncompleteTitleOffset = 9,
        IncorrectDescMarker = 28,
        IncorrectJendMarker = 33,
        IncorrectJpegMarker = 12,
        IncorrectJsonMarker = 18,
        IncorrectTitleMarker = 23,
        JsonMallocError = 20,
        JsonReadError = 21,
        NoError = 255,
        NoFormatIdentifier = 1,
        PhotoMallocError = 15,
        PhotoReadError = 16,
        TitleMallocError = 25,
        TitleReadError = 26,
        UnicodeHeaderError = 5,
        UnicodeInitError = 4,
        Uninitialised = 0,
    };
    /** Photo Formats */
    enum class PhotoFormat : uint32_t {
        GTA5 = 0x01000000U, /**< GTA V Photo Format */
        RDR2 = 0x04000000U, /**< RDR 2 Photo Format */
        Undefined = 0, /**< Undefined Format */
    };
    RagePhoto();
    ~RagePhoto();
    void clear(); /**< Resets the RagePhoto instance to default values. */
    bool load(const char *data, size_t length); /**< Loads a Photo stored inside a const char*. */
    bool load(const std::string &data); /**< Loads a Photo stored inside a std::string. */
    Error error(); /**< Returns the last error occurred. */
    PhotoFormat format(); /**< Returns the Photo Format (GTA V or RDR 2). */
    const char *photoData(); /**< Returns the Photo JPEG data. */
    const uint32_t photoSize(); /**< Returns the Photo JPEG data size. */
    const std::string description(); /**< Returns the Photo description. */
    const std::string json(); /**< Returns the Photo JSON data. */
    const std::string header(); /**< Returns the Photo header. */
    const std::string title(); /**< Returns the Photo title. */
    void setDescription(const std::string &description, uint32_t bufferSize = 0); /**< Sets the Photo description. */
    void setJson(const std::string &json, uint32_t bufferSize = 0); /**< Sets the Photo JSON data. */
    void setHeader(const std::string &header); /**< Sets the Photo header. (expert only) */
    /** Sets the Photo JPEG data.
    * @param data JPEG data
    * @param size JPEG data size
    */
    bool setPhotoData(const char *data, uint32_t size, uint32_t bufferSize = 0);
    /** Sets the Photo JPEG data.
    * @param data JPEG data
    */
    bool setPhotoData(const std::string &data, uint32_t bufferSize = 0);
    void setTitle(const std::string &title, uint32_t bufferSize = 0); /**< Sets the Photo title. */

protected:
    inline void moveOffsets();
    inline size_t readBuffer(const char *input, char *output, size_t *pos, size_t len, size_t inputLen);
    inline uint32_t charToUInt32BE(char *x);
    inline uint32_t charToUInt32LE(char *x);
    inline void uInt32ToCharBE(uint32_t x, char *y);
    inline void uInt32ToCharLE(uint32_t x, char *y);
    bool p_photoLoaded;
    char* p_photoData;
    Error p_error;
    PhotoFormat p_photoFormat;
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
