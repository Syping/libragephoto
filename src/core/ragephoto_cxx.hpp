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
namespace cxx_abi {

/**
* \brief GTA V and RDR 2 Photo Parser.
*/
class LIBRAGEPHOTO_CXX_PUBLIC photo
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
        DescBufferTight = 39L, /**< Description Buffer is too tight */
        DescMallocError = 31L, /**< Description Buffer can't be allocated */
        DescReadError = 32L, /**< Description can't be read successfully */
        HeaderBufferTight = 35L, /**< Header Buffer is too tight */
        HeaderMallocError = 4L, /**< Header Buffer can't be allocated */
        IncompatibleFormat = 2L, /**< Format is incompatible */
        IncompleteChecksum = 7L, /**< Header checksum is incomplete */
        IncompleteDescBuffer = 30L, /**< Description Buffer Size is incomplete */
        IncompleteDescMarker = 28L, /**< Description Marker is incomplete */
        IncompleteDescOffset = 11L, /**< Description Offset is incomplete */
        IncompleteEOF = 8L, /**< End Of File Offset is incomplete */
        IncompleteHeader = 3L, /**< Header is incomplete */
        IncompleteJendMarker = 33L, /**< JEND Marker is incomplete */
        IncompleteJpegMarker = 12L, /**< JPEG Marker is incomplete */
        IncompleteJsonBuffer = 20L, /**< JSON Buffer Size is incomplete */
        IncompleteJsonMarker = 18L, /**< JSON Marker incomplete */
        IncompleteJsonOffset = 9L, /**< JSON Offset incomplete */
        IncompletePhotoBuffer = 14L, /**< Photo Buffer Size is incomplete */
        IncompletePhotoSize = 15L, /**< Photo Size is incomplete */
        IncompleteTitleBuffer = 25L, /**< Title Buffer Size is incomplete */
        IncompleteTitleMarker = 23L, /**< Title Marker is incomplete */
        IncompleteTitleOffset = 10L, /**< Title Offset is incomplete */
        IncorrectDescMarker = 29L, /**< Description Marker is incorrect */
        IncorrectJendMarker = 34L, /**< JEND Marker is incorrect */
        IncorrectJpegMarker = 13L, /**< JPEG Marker is incorrect */
        IncorrectJsonMarker = 19L, /**< JSON Marker is incorrect */
        IncorrectTitleMarker = 24L, /**< Title Marker is incorrect */
        JsonBufferTight = 37L, /**< JSON Buffer is too tight */
        JsonMallocError = 21L, /**< JSON Buffer can't be allocated */
        JsonReadError = 22L, /**< JSON can't be read successfully */
        NoError = 255L, /**< Finished without errors */
        NoFormatIdentifier = 1L, /**< No format detected, empty file */
        PhotoBufferTight = 36L, /**< Photo Buffer is too tight */
        PhotoMallocError = 16L, /**< Photo Buffer can't be allocated */
        PhotoReadError = 17L, /**< Photo can't be read */
        TitleBufferTight = 38L, /**< Title Buffer is too tight */
        TitleMallocError = 26L, /**< Title Buffer can't be allocated */
        TitleReadError = 27L, /**< Title can't be read */
        UnicodeInitError = 5L, /**< Failed to initialise Unicode decoder */
        UnicodeHeaderError = 6L, /**< Header can't be encoded/decoded successfully */
        Uninitialised = 0L, /**< Uninitialised, file access failed */
    };
    /** Photo Formats */
    enum PhotoFormat : uint32_t {
        GTA5 = 0x01000000UL, /**< GTA V Photo Format */
        RDR2 = 0x04000000UL, /**< RDR 2 Photo Format */
    };
    /** Sign Initials */
    enum SignInitials : uint32_t {
        SIGTA5 = 0xE47AB81CUL, /**< GTA V Sign Initial */
        SIRDR2 = 0x00FEEB1EUL, /**< RDR 2 Sign Initial */
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

} // cxx_abi
} // ragephoto
#endif // __cplusplus

#endif // RAGEPHOTO_CXX_HPP
