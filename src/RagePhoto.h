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

#ifdef __cplusplus
#include "libragephoto_global.h"
#include "RagePhotoData.h"
#include <unordered_map>
#include <functional>
#include <iostream>
#include <cstdint>
#include <cstdio>

typedef std::function<bool(const char*, size_t, RagePhotoData*)> RagePhotoLoadFunc;

/**
* \brief GTA V and RDR 2 Photo Parser.
*/
class LIBRAGEPHOTO_EXPORT RagePhoto
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
    enum class Error : uint8_t {
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
    Error error() const; /**< Returns the last error occurred. */
    uint32_t format() const; /**< Returns the Photo Format (GTA V or RDR 2). */
    const std::string photo() const; /**< Returns the Photo JPEG data. */
    const char *photoData() const; /**< Returns the Photo JPEG data. */
    uint32_t photoSize() const; /**< Returns the Photo JPEG data size. */
    const std::string& description() const; /**< Returns the Photo description. */
    const std::string& json() const; /**< Returns the Photo JSON data. */
    const std::string& header() const; /**< Returns the Photo header. */
    const std::string& title() const; /**< Returns the Photo title. */
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
    void setDescription(const std::string &description, uint32_t bufferSize = 0); /**< Sets the Photo description. */
    void setFormat(uint32_t photoFormat); /**< Sets the Photo Format (GTA V or RDR 2). */
    void setFormatLoadFunction(uint32_t photoFormat, RagePhotoLoadFunc func); /**< Sets a custom Photo Format load function. */
    void setJson(const std::string &json, uint32_t bufferSize = 0); /**< Sets the Photo JSON data. */
    void setHeader(const std::string &header, uint32_t headerSum); /**< Sets the Photo header. (EXPERT ONLY) */
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
    void setTitle(const std::string &title, uint32_t bufferSize = 0); /**< Sets the Photo title. */

protected:
    static size_t readBuffer(const char *input, void *output, size_t *pos, size_t len, size_t inputLen);
    static size_t writeBuffer(const void *input, char *output, size_t *pos, size_t len, size_t inputLen);
    static uint32_t charToUInt32LE(char *x);
    static void uInt32ToCharLE(uint32_t x, char *y);
    std::unordered_map<uint8_t, RagePhotoLoadFunc> m_loadFuncs;
    RagePhotoData m_data;
};
#else
#define LIBRAGEPHOTO_C_API
#endif

#ifdef LIBRAGEPHOTO_C_API
#ifdef __cplusplus
extern "C" {
#else
#include "libragephoto_global.h"
#include <stdint.h>
#include <stdio.h>
#endif
/** \file RagePhoto.h */
/** RagePhoto C++ class typedef for C API. */
typedef void* ragephoto_t;

/** Opens a \p ragephoto_t instance. */
LIBRAGEPHOTO_EXPORT ragephoto_t ragephoto_open();

/** Resets the RagePhoto instance to default values.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_EXPORT void ragephoto_clear(ragephoto_t instance);

/** Loads a Photo from a const char*.
* \param instance \p ragephoto_t instance
* \param data Photo data
* \param size Photo data size
*/
LIBRAGEPHOTO_EXPORT int ragephoto_load(ragephoto_t instance, const char *data, size_t size);

/** Loads a Photo from a file.
* \param instance \p ragephoto_t instance
* \param filename File to load
*/
LIBRAGEPHOTO_EXPORT int ragephoto_loadfile(ragephoto_t instance, const char *filename);

/** Returns the last error occurred.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_EXPORT uint8_t ragephoto_error(ragephoto_t instance);

/** Returns the GTA V default Photo Buffer Size. */
LIBRAGEPHOTO_EXPORT uint32_t ragephoto_defpbuf_gta5();

/** Returns the RDR 2 default Photo Buffer Size. */
LIBRAGEPHOTO_EXPORT uint32_t ragephoto_defpbuf_rdr2();

/** Returns the GTA V Photo Format. */
LIBRAGEPHOTO_EXPORT uint32_t ragephoto_format_gta5();

/** Returns the RDR 2 Photo Format. */
LIBRAGEPHOTO_EXPORT uint32_t ragephoto_format_rdr2();

/** Returns the Photo description.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_EXPORT const char* ragephoto_getphotodesc(ragephoto_t instance);

/** Returns the Photo Format (GTA V or RDR 2).
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_EXPORT uint32_t ragephoto_getphotoformat(ragephoto_t instance);

/** Returns the Photo JPEG data.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_EXPORT const char* ragephoto_getphotojpeg(ragephoto_t instance);

/** Returns the Photo JSON data.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_EXPORT const char* ragephoto_getphotojson(ragephoto_t instance);

/** Returns the Photo header.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_EXPORT const char* ragephoto_getphotoheader(ragephoto_t instance);

/** Returns the Photo JPEG data size.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_EXPORT uint32_t ragephoto_getphotosize(ragephoto_t instance);

/** Returns the Photo title.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_EXPORT const char* ragephoto_getphototitle(ragephoto_t instance);

/** Returns the Photo save file size.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_EXPORT size_t ragephoto_getsavesize(ragephoto_t instance);

/** Returns the Photo save file size.
* \param instance \p ragephoto_t instance
* \param photoFormat Photo Format (GTA V or RDR 2)
*/
LIBRAGEPHOTO_EXPORT size_t ragephoto_getsavesizef(ragephoto_t instance, uint32_t photoFormat);

/** Saves a Photo to a char*.
* \param instance \p ragephoto_t instance
* \param data Photo data
*/
LIBRAGEPHOTO_EXPORT int ragephoto_save(ragephoto_t instance, char *data);

/** Saves a Photo to a char*.
* \param instance \p ragephoto_t instance
* \param data Photo data
* \param photoFormat Photo Format (GTA V or RDR 2)
*/
LIBRAGEPHOTO_EXPORT int ragephoto_savef(ragephoto_t instance, char *data, uint32_t photoFormat);

/** Saves a Photo to a file.
* \param instance \p ragephoto_t instance
* \param filename File to save
*/
LIBRAGEPHOTO_EXPORT int ragephoto_savefile(ragephoto_t instance, const char *filename);

/** Saves a Photo to a file.
* \param instance \p ragephoto_t instance
* \param filename File to save
* \param photoFormat Photo Format (GTA V or RDR 2)
*/
LIBRAGEPHOTO_EXPORT int ragephoto_savefilef(ragephoto_t instance, const char *filename, uint32_t photoFormat);

/** Sets all cross-format Buffer to default size.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_EXPORT void ragephoto_setbufferdefault(ragephoto_t instance);

/** Moves all Buffer offsets to correct position.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_EXPORT void ragephoto_setbufferoffsets(ragephoto_t instance);

/** Sets the Photo description.
* \param instance \p ragephoto_t instance
* \param description Description
* \param bufferSize Description buffer size \p USE \p 0
*/
LIBRAGEPHOTO_EXPORT void ragephoto_setphotodesc(ragephoto_t instance, const char *description, uint32_t bufferSize);

/** Sets the Photo Format (GTA V or RDR 2).
* \param instance \p ragephoto_t instance
* \param photoFormat Photo Format (GTA V or RDR 2)
*/
LIBRAGEPHOTO_EXPORT void ragephoto_setphotoformat(ragephoto_t instance, uint32_t photoFormat);

/** Sets the Photo JPEG data.
* \param instance \p ragephoto_t instance
* \param data JPEG data
* \param size JPEG data size
* \param bufferSize JPEG buffer size
*/
LIBRAGEPHOTO_EXPORT int ragephoto_setphotojpeg(ragephoto_t instance, const char *data, uint32_t size, uint32_t bufferSize);

/** Sets the Photo JSON data.
* \param instance \p ragephoto_t instance
* \param json JSON data
* \param bufferSize JSON data buffer size \p USE \p 0
*/
LIBRAGEPHOTO_EXPORT void ragephoto_setphotojson(ragephoto_t instance, const char *json, uint32_t bufferSize);

/** Sets the Photo header. (EXPERT ONLY) */
LIBRAGEPHOTO_EXPORT void ragephoto_setphotoheader(ragephoto_t instance, const char *header, uint32_t headerSum);

/** Sets the Photo title.
* \param instance \p ragephoto_t instance
* \param title Title
* \param bufferSize Title buffer size \p USE \p 0
*/
LIBRAGEPHOTO_EXPORT void ragephoto_setphototitle(ragephoto_t instance, const char *title, uint32_t bufferSize);

/** Closes a \p ragephoto_t instance.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_EXPORT void ragephoto_close(ragephoto_t instance);

/** Returns the library version. */
LIBRAGEPHOTO_EXPORT const char* ragephoto_version();

#ifdef __cplusplus
}
#endif
#endif

#endif // RAGEPHOTO_H
