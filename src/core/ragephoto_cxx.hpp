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

#ifndef RAGEPHOTO_CXX_HPP
#define RAGEPHOTO_CXX_HPP

#ifdef __cplusplus
#include "RagePhotoLibrary.h"
#include "RagePhotoTypedefs.h"
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstdio>

namespace ragephoto {

/**
* \brief GTA V and RDR 2 Photo Parser.
* \class ragephoto::photo RagePhoto.hpp RagePhoto
*/
class LIBRAGEPHOTO_CXX_PUBLIC photo
{
public:
    /** Default sizes */
    enum DefaultSize : uint32_t {
        DEFAULT_GTA5_PHOTOBUFFER = RAGEPHOTO_DEFAULT_GTA5_PHOTOBUFFER, /**< GTA V default Photo Buffer Size */
        DEFAULT_RDR2_PHOTOBUFFER = RAGEPHOTO_DEFAULT_RDR2_PHOTOBUFFER, /**< RDR 2 default Photo Buffer Size */
        DEFAULT_DESCBUFFER = RAGEPHOTO_DEFAULT_DESCBUFFER, /**< Default Description Buffer Size */
        DEFAULT_JSONBUFFER = RAGEPHOTO_DEFAULT_JSONBUFFER, /**< Default JSON Buffer Size */
        DEFAULT_TITLBUFFER = RAGEPHOTO_DEFAULT_TITLBUFFER, /**< Default Title Buffer Size */
        GTA5_HEADERSIZE = RAGEPHOTO_GTA5_HEADERSIZE, /**< GTA V Header Size */
        RDR2_HEADERSIZE = RAGEPHOTO_RDR2_HEADERSIZE, /**< RDR 2 Header Size */
    };
    /** Parsing and set errors */
    enum Error : int32_t {
        DescBufferTight = RAGEPHOTO_ERROR_DESCBUFFERTIGHT, /**< Description Buffer is too tight */
        DescMallocError = RAGEPHOTO_ERROR_DESCMALLOCERROR, /**< Description Buffer can't be allocated */
        DescReadError = RAGEPHOTO_ERROR_DESCREADERROR, /**< Description can't be read successfully */
        HeaderBufferTight = RAGEPHOTO_ERROR_HEADERBUFFERTIGHT, /**< Header Buffer is too tight */
        HeaderMallocError = RAGEPHOTO_ERROR_HEADERMALLOCERROR, /**< Header Buffer can't be allocated */
        IncompatibleFormat = RAGEPHOTO_ERROR_INCOMPATIBLEFORMAT, /**< Format is incompatible */
        IncompleteChecksum = RAGEPHOTO_ERROR_INCOMPLETECHECKSUM, /**< Header checksum is incomplete */
        IncompleteDescBuffer = RAGEPHOTO_ERROR_INCOMPLETEDESCBUFFER, /**< Description Buffer Size is incomplete */
        IncompleteDescMarker = RAGEPHOTO_ERROR_INCOMPLETEDESCMARKER, /**< Description Marker is incomplete */
        IncompleteDescOffset = RAGEPHOTO_ERROR_INCOMPLETEDESCOFFSET, /**< Description Offset is incomplete */
        IncompleteEOF = RAGEPHOTO_ERROR_INCOMPLETEEOF, /**< End Of File Offset is incomplete */
        IncompleteHeader = RAGEPHOTO_ERROR_INCOMPLETEHEADER, /**< Header is incomplete */
        IncompleteJendMarker = RAGEPHOTO_ERROR_INCOMPLETEJENDMARKER, /**< JEND Marker is incomplete */
        IncompleteJpegMarker = RAGEPHOTO_ERROR_INCOMPLETEJPEGMARKER, /**< JPEG Marker is incomplete */
        IncompleteJsonBuffer = RAGEPHOTO_ERROR_INCOMPLETEJSONBUFFER, /**< JSON Buffer Size is incomplete */
        IncompleteJsonMarker = RAGEPHOTO_ERROR_INCOMPLETEJSONMARKER, /**< JSON Marker incomplete */
        IncompleteJsonOffset = RAGEPHOTO_ERROR_INCOMPLETEJSONOFFSET, /**< JSON Offset incomplete */
        IncompletePhotoBuffer = RAGEPHOTO_ERROR_INCOMPLETEPHOTOBUFFER, /**< Photo Buffer Size is incomplete */
        IncompletePhotoSize = RAGEPHOTO_ERROR_INCOMPLETEPHOTOSIZE, /**< Photo Size is incomplete */
        IncompleteTitleBuffer = RAGEPHOTO_ERROR_INCOMPLETETITLEBUFFER, /**< Title Buffer Size is incomplete */
        IncompleteTitleMarker = RAGEPHOTO_ERROR_INCOMPLETETITLEMARKER, /**< Title Marker is incomplete */
        IncompleteTitleOffset = RAGEPHOTO_ERROR_INCOMPLETETITLEOFFSET, /**< Title Offset is incomplete */
        IncorrectDescMarker = RAGEPHOTO_ERROR_INCORRECTDESCMARKER, /**< Description Marker is incorrect */
        IncorrectJendMarker = RAGEPHOTO_ERROR_INCORRECTJENDMARKER, /**< JEND Marker is incorrect */
        IncorrectJpegMarker = RAGEPHOTO_ERROR_INCORRECTJPEGMARKER, /**< JPEG Marker is incorrect */
        IncorrectJsonMarker = RAGEPHOTO_ERROR_INCORRECTJSONMARKER, /**< JSON Marker is incorrect */
        IncorrectTitleMarker = RAGEPHOTO_ERROR_INCORRECTTITLEMARKER, /**< Title Marker is incorrect */
        JsonBufferTight = RAGEPHOTO_ERROR_JSONBUFFERTIGHT, /**< JSON Buffer is too tight */
        JsonMallocError = RAGEPHOTO_ERROR_JSONMALLOCERROR, /**< JSON Buffer can't be allocated */
        JsonReadError = RAGEPHOTO_ERROR_JSONREADERROR, /**< JSON can't be read successfully */
        NoError = RAGEPHOTO_ERROR_NOERROR, /**< Finished without errors */
        NoFormatIdentifier = RAGEPHOTO_ERROR_NOFORMATIDENTIFIER, /**< No format detected, empty file */
        PhotoBufferTight = RAGEPHOTO_ERROR_PHOTOBUFFERTIGHT, /**< Photo Buffer is too tight */
        PhotoMallocError = RAGEPHOTO_ERROR_PHOTOMALLOCERROR, /**< Photo Buffer can't be allocated */
        PhotoReadError = RAGEPHOTO_ERROR_PHOTOREADERROR, /**< Photo can't be read */
        TitleBufferTight = RAGEPHOTO_ERROR_TITLEBUFFERTIGHT, /**< Title Buffer is too tight */
        TitleMallocError = RAGEPHOTO_ERROR_TITLEMALLOCERROR, /**< Title Buffer can't be allocated */
        TitleReadError = RAGEPHOTO_ERROR_TITLEREADERROR, /**< Title can't be read */
        UnicodeInitError = RAGEPHOTO_ERROR_UNICODEINITERROR, /**< Failed to initialise Unicode decoder */
        UnicodeHeaderError = RAGEPHOTO_ERROR_UNICODEHEADERERROR, /**< Header can't be encoded/decoded successfully */
        Uninitialised = RAGEPHOTO_ERROR_UNINITIALISED, /**< Uninitialised, file access failed */
    };
    /** Photo Formats */
    enum PhotoFormat : uint32_t {
        GTA5 = RAGEPHOTO_FORMAT_GTA5, /**< GTA V Photo Format */
        RDR2 = RAGEPHOTO_FORMAT_RDR2, /**< RDR 2 Photo Format */
    };
    /** Sign Initials */
    enum SignInitials : uint32_t {
        SIGTA5 = RAGEPHOTO_SIGNINITIAL_GTA5, /**< GTA V Sign Initial */
        SIRDR2 = RAGEPHOTO_SIGNINITIAL_RDR2, /**< RDR 2 Sign Initial */
    };
    photo();
    ~photo();
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
    static uint64_t jpegSign(uint32_t photoFormat, RagePhotoData *rp_data); /**< Returns the Photo JPEG sign. */
    static uint64_t jpegSign(RagePhotoData *rp_data); /**< Returns the Photo JPEG sign. */
    uint64_t jpegSign(uint32_t photoFormat) const; /**< Returns the Photo JPEG sign. */
    uint64_t jpegSign() const; /**< Returns the Photo JPEG sign. */
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
    bool saveFile(const char *filename, uint32_t photoFormat); /**< Saves a Photo to a file. */
    bool saveFile(const char *filename); /**< Saves a Photo to a file. */
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
    void setHeader(const char *header, uint32_t headerSum, uint32_t headerSum2 = 0); /**< Sets the Photo header. */
    void setTitle(const char *title, uint32_t bufferSize = 0); /**< Sets the Photo title. */

private:
    RagePhotoData *m_data;
    RagePhotoFormatParser *m_parser;
};

} // ragephoto
#endif // __cplusplus

#endif // RAGEPHOTO_CXX_HPP
