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

#define DEFAULT_GTAV_PHOTOBUFFER 524288UL /**< GTA V default Photo Buffer Size */
#define DEFAULT_RDR2_PHOTOBUFFER 1048576UL /**< RDR 2 default Photo Buffer Size */
#define DEFAULT_DESCBUFFER 256UL /**< Default Description Buffer Size */
#define DEFAULT_JSONBUFFER 3072UL /**< Default JSON Buffer Size */
#define DEFAULT_TITLBUFFER 256UL /**< Default Title Buffer Size */

class LIBRAGEPHOTO_EXPORT RagePhoto
{
public:
    /** Parsing and set errors */
    enum class Error : uint8_t {
        DescMallocError = 30, /**< Description Buffer can't be allocated */
        DescReadError = 31, /**< Description can't be read successfully */
        IncompatibleFormat = 2, /**< Format is incompatible */
        IncompleteChecksum = 6, /**< Header checksum is incomplete */
        IncompleteDescBuffer = 29, /**< Description Buffer Size is incomplete */
        IncompleteDescMarker = 27, /**< Description Marker is incomplete */
        IncompleteDescOffset = 10, /**< Description Offset is incomplete */
        IncompleteEOF = 7, /**< End Of File Offset is incomplete */
        IncompleteHeader = 3, /**< Header is incomplete */
        IncompleteJendMarker = 32, /**< JEND Marker is incomplete */
        IncompleteJpegMarker = 11, /**< JPEG Marker is incomplete */
        IncompleteJsonBuffer = 19, /**< JSON Buffer Size is incomplete */
        IncompleteJsonMarker = 17, /**< JSON Marker incomplete */
        IncompleteJsonOffset = 8, /**< JSON Offset incomplete */
        IncompletePhotoBuffer = 13, /**< Photo Buffer Size is incomplete */
        IncompletePhotoSize = 14, /**< Photo Size is incomplete */
        IncompleteTitleBuffer = 24, /**< Title Buffer Size is incomplete */
        IncompleteTitleMarker = 22, /**< Title Marker is incomplete */
        IncompleteTitleOffset = 9, /**< Title Offset is incomplete */
        IncorrectDescMarker = 28, /**< Description Marker is incorrect */
        IncorrectJendMarker = 33, /**< JEND Marker is incorrect */
        IncorrectJpegMarker = 12, /**< JPEG Marker is incorrect */
        IncorrectJsonMarker = 18, /**< JSON Marker is incorrect */
        IncorrectTitleMarker = 23, /**< Title Marker is incorrect */
        JsonMallocError = 20, /**< JSON Buffer can't be allocated */
        JsonReadError = 21, /**< JSON can't be read successfully */
        NoError = 255, /**< Finished without errors */
        NoFormatIdentifier = 1, /**< No format detected, empty file */
        PhotoMallocError = 15, /**< Photo Buffer can't be allocated */
        PhotoReadError = 16, /**< Photo can't be read */
        TitleMallocError = 25, /**< Title Buffer can't be allocated */
        TitleReadError = 26, /**< Title can't be read */
        UnicodeHeaderError = 5, /**< Header can't be decoded */
        UnicodeInitError = 4, /**< Failed to initialise Unicode decoder */
        Uninitialised = 0, /**< Uninitialised */
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
    uint32_t photoSize(); /**< Returns the Photo JPEG data size. */
    const std::string description(); /**< Returns the Photo description. */
    const std::string json(); /**< Returns the Photo JSON data. */
    const std::string header(); /**< Returns the Photo header. */
    const std::string title(); /**< Returns the Photo title. */
    void setBufferDefault(); /**< Sets all cross-format Buffer to default size. */
    void setDescription(const std::string &description, uint32_t bufferSize = 0); /**< Sets the Photo description. */
    void setFormat(PhotoFormat photoFormat); /**< Sets the Photo Format (GTA V or RDR 2). */
    void setJson(const std::string &json, uint32_t bufferSize = 0); /**< Sets the Photo JSON data. */
    void setHeader(const std::string &header, uint32_t headerSum); /**< Sets the Photo header. (expert only) */
    /** Sets the Photo JPEG data.
    * \param data JPEG data
    * \param size JPEG data size
    * \param bufferSize JPEG buffer size
    */
    bool setPhotoData(const char *data, uint32_t size, uint32_t bufferSize = 0);
    /** Sets the Photo JPEG data.
    * \param data JPEG data
    * \param bufferSize JPEG buffer size
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
