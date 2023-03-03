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
#include "RagePhotoLibrary.h"
#include "RagePhotoTypedefs.h"
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstdio>

/**
* \brief GTA V and RDR 2 Photo Parser.
*/
class LIBRAGEPHOTO_CXX_BINDING RagePhoto
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
    enum Error : int32_t {
        DescBufferTight = 39, /**< Description Buffer is too tight */
        DescMallocError = 31, /**< Description Buffer can't be allocated */
        DescReadError = 32, /**< Description can't be read successfully */
        HeaderBufferTight = 35, /**< Header Buffer is too tight */
        HeaderMallocError = 4, /**< Header Buffer can't be allocated */
        IncompatibleFormat = 2, /**< Format is incompatible */
        IncompleteChecksum = 7, /**< Header checksum is incomplete */
        IncompleteDescBuffer = 30, /**< Description Buffer Size is incomplete */
        IncompleteDescMarker = 28, /**< Description Marker is incomplete */
        IncompleteDescOffset = 11, /**< Description Offset is incomplete */
        IncompleteEOF = 8, /**< End Of File Offset is incomplete */
        IncompleteHeader = 3, /**< Header is incomplete */
        IncompleteJendMarker = 33, /**< JEND Marker is incomplete */
        IncompleteJpegMarker = 12, /**< JPEG Marker is incomplete */
        IncompleteJsonBuffer = 20, /**< JSON Buffer Size is incomplete */
        IncompleteJsonMarker = 18, /**< JSON Marker incomplete */
        IncompleteJsonOffset = 9, /**< JSON Offset incomplete */
        IncompletePhotoBuffer = 14, /**< Photo Buffer Size is incomplete */
        IncompletePhotoSize = 15, /**< Photo Size is incomplete */
        IncompleteTitleBuffer = 25, /**< Title Buffer Size is incomplete */
        IncompleteTitleMarker = 23, /**< Title Marker is incomplete */
        IncompleteTitleOffset = 10, /**< Title Offset is incomplete */
        IncorrectDescMarker = 29, /**< Description Marker is incorrect */
        IncorrectJendMarker = 34, /**< JEND Marker is incorrect */
        IncorrectJpegMarker = 13, /**< JPEG Marker is incorrect */
        IncorrectJsonMarker = 19, /**< JSON Marker is incorrect */
        IncorrectTitleMarker = 24, /**< Title Marker is incorrect */
        JsonBufferTight = 37, /**< JSON Buffer is too tight */
        JsonMallocError = 21, /**< JSON Buffer can't be allocated */
        JsonReadError = 22, /**< JSON can't be read successfully */
        NoError = 255, /**< Finished without errors */
        NoFormatIdentifier = 1, /**< No format detected, empty file */
        PhotoBufferTight = 36, /**< Photo Buffer is too tight */
        PhotoMallocError = 16, /**< Photo Buffer can't be allocated */
        PhotoReadError = 17, /**< Photo can't be read */
        TitleBufferTight = 38, /**< Title Buffer is too tight */
        TitleMallocError = 26, /**< Title Buffer can't be allocated */
        TitleReadError = 27, /**< Title can't be read */
        UnicodeInitError = 5, /**< Failed to initialise Unicode decoder */
        UnicodeHeaderError = 6, /**< Header can't be encoded/decoded successfully */
        Uninitialised = 0, /**< Uninitialised, file access failed */
    };
    /** Photo Formats */
    enum PhotoFormat : uint32_t {
        GTA5 = 0x01000000UL, /**< GTA V Photo Format */
        RDR2 = 0x04000000UL, /**< RDR 2 Photo Format */
    };
    RagePhoto();
    ~RagePhoto();
    void addParser(RagePhotoFormatParser *rp_parser); /**< Add a custom defined RagePhotoFormatParser. */
    static void clear(RagePhotoData *rp_data); /**< Resets the RagePhotoData object to default values. */
    void clear(); /**< Resets the RagePhotoData object to default values. */
    RagePhotoData* data(); /**< Returns the internal RagePhotoData object. */
    static bool load(const char *data, size_t size, RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser); /**< Loads a Photo from a const char*. */
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
    int32_t error() const; /**< Returns the last error occurred. */
    uint32_t format() const; /**< Returns the Photo Format (GTA V or RDR 2). */
    const std::string jpeg() const; /**< Returns the Photo JPEG data. */
#if (RAGEPHOTO_CXX_STD >= 17) && (__cplusplus >= 201703L)
    const std::string_view jpeg_view() const; /**< Returns the Photo JPEG data. */
#endif
    const char* jpegData() const; /**< Returns the Photo JPEG data. */
    uint32_t jpegSize() const; /**< Returns the Photo JPEG data size. */
    const char* description() const; /**< Returns the Photo description. */
    const char* json() const; /**< Returns the Photo JSON data. */
    const char* header() const; /**< Returns the Photo header. */
    const char* title() const; /**< Returns the Photo title. */
    static const char* version(); /**< Returns the library version. */
    static bool save(char *data, uint32_t photoFormat, RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser); /**< Saves a Photo to a char*. */
    static bool save(char *data, RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser); /**< Saves a Photo to a char*. */
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
    static size_t saveSize(uint32_t photoFormat, RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser); /**< Returns the Photo save file size. */
    static size_t saveSize(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser); /**< Returns the Photo save file size. */
    size_t saveSize(uint32_t photoFormat); /**< Returns the Photo save file size. */
    size_t saveSize(); /**< Returns the Photo save file size. */
    static void setBufferDefault(RagePhotoData *rp_data); /**< Sets all cross-format Buffer to default size. */
    void setBufferDefault(); /**< Sets all cross-format Buffer to default size. */
    static void setBufferOffsets(RagePhotoData *rp_data); /**< Moves all Buffer offsets to correct position. */
    void setBufferOffsets(); /**< Moves all Buffer offsets to correct position. */
    bool setData(RagePhotoData *rp_data, bool takeCopy = true); /**< Sets the internal RagePhotoData object. */
    void setDescription(const char *description, uint32_t bufferSize = 0); /**< Sets the Photo description. */
    void setFormat(uint32_t photoFormat); /**< Sets the Photo Format (GTA V or RDR 2). */
    /** Sets the Photo JPEG data.
    * \param data JPEG data
    * \param size JPEG data size
    * \param bufferSize JPEG buffer size
    */
    bool setJpeg(const char *data, uint32_t size, uint32_t bufferSize = 0);
    /** Sets the Photo JPEG data.
    * \param data JPEG data
    * \param bufferSize JPEG buffer size
    */
    bool setJpeg(const std::string &data, uint32_t bufferSize = 0);
    void setJson(const char *json, uint32_t bufferSize = 0); /**< Sets the Photo JSON data. */
    void setHeader(const char *header, uint32_t headerSum); /**< Sets the Photo header. (EXPERT ONLY) */
    void setTitle(const char *title, uint32_t bufferSize = 0); /**< Sets the Photo title. */

private:
    RagePhotoData *m_data;
    RagePhotoFormatParser *m_parser;
};
#else
#include "RagePhotoC.h"
#endif // __cplusplus

#endif // RAGEPHOTO_H
