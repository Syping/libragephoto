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

#ifndef RAGEPHOTO_H
#define RAGEPHOTO_H

#ifdef __cplusplus
#include "libragephoto_global.h"
#include "RagePhotoData.h"
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstdio>

/**
* \brief GTA V and RDR 2 Photo Parser.
*/
class LIBRAGEPHOTO_CXX_EXPORT RagePhoto
{
public:
    /** Default sizes */
    enum DefaultSize : uint32_t {
        DEFAULT_GTA5_PHOTOBUFFER = 524288UL, /**< GTA V default Photo Buffer Size */
        DEFAULT_RDR2_PHOTOBUFFER = 1048576UL, /**< RDR 2 default Photo Buffer Size */
        DEFAULT_DESCBUFFER = 256UL, /**< Default Description Buffer Size */
        DEFAULT_JSONBUFFER = 3072UL, /**< Default JSON Buffer Size */
        DEFAULT_TITLBUFFER = 256UL, /**< Default Title Buffer Size */
        GTA5_HEADERSIZE = 264UL, /**< GTA V Header Size */
        RDR2_HEADERSIZE = 272UL, /**< RDR 2 Header Size */
    };
    /** Parsing and set errors */
    enum Error : uint8_t {
        DescBufferTight = 38, /**< Description Buffer is too tight */
        DescMallocError = 30, /**< Description Buffer can't be allocated */
        DescReadError = 31, /**< Description can't be read successfully */
        HeaderBufferTight = 34, /**< Header Buffer is too tight */
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
        JsonBufferTight = 36, /**< JSON Buffer is too tight */
        JsonMallocError = 20, /**< JSON Buffer can't be allocated */
        JsonReadError = 21, /**< JSON can't be read successfully */
        NoError = 255, /**< Finished without errors */
        NoFormatIdentifier = 1, /**< No format detected, empty file */
        PhotoBufferTight = 35, /**< Photo Buffer is too tight */
        PhotoMallocError = 15, /**< Photo Buffer can't be allocated */
        PhotoReadError = 16, /**< Photo can't be read */
        TitleBufferTight = 37, /**< Title Buffer is too tight */
        TitleMallocError = 25, /**< Title Buffer can't be allocated */
        TitleReadError = 26, /**< Title can't be read */
        UnicodeHeaderError = 5, /**< Header can't be decoded */
        UnicodeInitError = 4, /**< Failed to initialise Unicode decoder */
        Uninitialised = 0, /**< Uninitialised, file access failed */
    };
    /** Photo Formats */
    enum PhotoFormat : uint32_t {
        GTA5 = 0x01000000UL, /**< GTA V Photo Format */
        RDR2 = 0x04000000UL, /**< RDR 2 Photo Format */
    };
    RagePhoto();
    ~RagePhoto();
    void clear(); /**< Resets the RagePhoto instance to default values. */
    RagePhotoData* data(); /**< Returns the internal RagePhotoData object. */
    /** Loads a Photo from a const char*.
    * \param data Photo data
    * \param size Photo data size
    */
    bool load(const char *data, size_t size);
    /** Loads a Photo from a std::string.
    * \param data Photo data
    */
    bool load(const std::string &data);
    /** Loads a Photo from a file.
    * \param filename File to load
    */
    bool loadFile(const std::string &filename);
    uint8_t error() const; /**< Returns the last error occurred. */
    uint32_t format() const; /**< Returns the Photo Format (GTA V or RDR 2). */
    const std::string photo() const; /**< Returns the Photo JPEG data. */
    const char *photoData() const; /**< Returns the Photo JPEG data. */
    uint32_t photoSize() const; /**< Returns the Photo JPEG data size. */
    const char* description() const; /**< Returns the Photo description. */
    const char* json() const; /**< Returns the Photo JSON data. */
    const char* header() const; /**< Returns the Photo header. */
    const char* title() const; /**< Returns the Photo title. */
    static const char* version(); /**< Returns the library version. */
    /** Saves a Photo to a char*.
    * \param data Photo data
    * \param photoFormat Photo Format (GTA V or RDR 2)
    */
    bool save(char *data, uint32_t photoFormat);
    /** Saves a Photo to a char*.
    * \param data Photo data
    */
    bool save(char *data);
    /** Saves a Photo to a std::string.
    * \param photoFormat Photo Format (GTA V or RDR 2)
    * \param ok \p true when saved successfully
    */
    const std::string save(uint32_t photoFormat, bool *ok = nullptr);
    /** Saves a Photo to a std::string.
    * \param ok \p true when saved successfully
    */
    const std::string save(bool *ok = nullptr);
    bool saveFile(const std::string &filename, uint32_t photoFormat); /**< Saves a Photo to a file. */
    bool saveFile(const std::string &filename); /**< Saves a Photo to a file. */
    static size_t saveSize(RagePhotoData *ragePhotoData, uint32_t photoFormat); /**< Returns the Photo save file size. */
    static size_t saveSize(RagePhotoData *ragePhotoData); /**< Returns the Photo save file size. */
    size_t saveSize(uint32_t photoFormat); /**< Returns the Photo save file size. */
    size_t saveSize(); /**< Returns the Photo save file size. */
    void setBufferDefault(); /**< Sets all cross-format Buffer to default size. */
    static void setBufferDefault(RagePhotoData *ragePhotoData); /**< Sets all cross-format Buffer to default size. */
    void setBufferOffsets(); /**< Moves all Buffer offsets to correct position. */
    static void setBufferOffsets(RagePhotoData *ragePhotoData); /**< Moves all Buffer offsets to correct position. */
    void setDescription(const char *description, uint32_t bufferSize = 0); /**< Sets the Photo description. */
    void setFormat(uint32_t photoFormat); /**< Sets the Photo Format (GTA V or RDR 2). */
    void setJson(const char *json, uint32_t bufferSize = 0); /**< Sets the Photo JSON data. */
    void setHeader(const char *header, uint32_t headerSum); /**< Sets the Photo header. (EXPERT ONLY) */
    /** Sets the Photo JPEG data.
    * \param data JPEG data
    * \param size JPEG data size
    * \param bufferSize JPEG buffer size
    */
    bool setPhoto(const char *data, uint32_t size, uint32_t bufferSize = 0);
    /** Sets the Photo JPEG data.
    * \param data JPEG data
    * \param bufferSize JPEG buffer size
    */
    bool setPhoto(const std::string &data, uint32_t bufferSize = 0);
    void setTitle(const char *title, uint32_t bufferSize = 0); /**< Sets the Photo title. */

private:
    RagePhotoData m_data;
};
#else
#include "RagePhotoC.h"
#endif // __cplusplus

#endif // RAGEPHOTO_H
