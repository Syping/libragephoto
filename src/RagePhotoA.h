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

#ifndef RAGEPHOTOA_H
#define RAGEPHOTOA_H

#ifdef __cplusplus
#include "RagePhotoC.h"
#include <cstdlib>
#include <iostream>

/**
* \brief ABI Stable Wrapper for RagePhoto.
*
* Using RagePhotoA instead of RagePhoto allows your library or application to survive more changes in the RagePhoto class,
* disadvantages include that it doesn't always include the newest features, performance is about the same.
*/
class RagePhotoA
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
    /** Sign Initials */
    enum SignInitials : uint32_t {
        SIGTA5 = 0xE47AB81CUL, /**< GTA V Sign Initial */
    };
    RagePhotoA() {
        instance = ragephoto_open();
    }
    ~RagePhotoA() {
        ragephoto_close(instance);
    }
    /** Add a custom defined RagePhotoFormatParser. */
    void addParser(RagePhotoFormatParser *rp_parser) {
        ragephoto_addparser(instance, rp_parser);
    }
    /** Resets the RagePhotoData object to default values. */
    static void clear(RagePhotoData *rp_data) {
        ragephotodata_clear(rp_data);
    }
    /** Resets the RagePhotoData object to default values. */
    void clear() {
        ragephoto_clear(instance);
    }
    /** Returns the internal RagePhotoData object. */
    RagePhotoData* data() {
        return ragephoto_getphotodata(instance);
    }
    /** Loads a Photo from a const char*. */
    static bool load(const char *data, size_t size, RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser) {
        return ragephotodata_load(rp_data, rp_parser, data, size);
    }
    /** Loads a Photo from a const char*.
    * \param data Photo data
    * \param size Photo data size
    */
    bool load(const char *data, size_t size) {
        return ragephoto_load(instance, data, size);
    }
    /** Loads a Photo from a std::string.
    * \param data Photo data
    */
    bool load(const std::string &data) {
        return ragephoto_load(instance, data.data(), data.size());
    }
    /** Loads a Photo from a file.
    * \param filename File to load
    */
    bool loadFile(const char *filename) {
        return ragephoto_loadfile(instance, filename);
    }
    /** Returns the last error occurred. */
    int32_t error() const {
        return ragephoto_error(instance);
    }
    /** Returns the Photo Format (GTA V or RDR 2). */
    uint32_t format() const {
        return ragephoto_getphotoformat(instance);
    }
    /** Returns the Photo JPEG data. */
    const std::string jpeg() const {
        const char *jpegData = ragephoto_getphotojpeg(instance);
        if (jpegData)
            return std::string(jpegData, ragephoto_getphotosize(instance));
        else
            return std::string();
    }
#if (__cplusplus >= 201703L)
    /** Returns the Photo JPEG data. */
    const std::string_view jpeg_view() const {
        const char *jpegData = ragephoto_getphotojpeg(instance);
        if (jpegData)
            return std::string_view(jpegData, ragephoto_getphotosize(instance));
        else
            return std::string_view();
    }
#endif
    /** Returns the Photo JPEG data. */
    const char* jpegData() const {
        return ragephoto_getphotojpeg(instance);
    }
    /** Returns the Photo JPEG sign. */
    static uint64_t jpegSign(uint32_t photoFormat, RagePhotoData *rp_data) {
        return ragephotodata_getphotosignf(rp_data, photoFormat);
    }
    /** Returns the Photo JPEG sign. */
    static uint64_t jpegSign(RagePhotoData *rp_data) {
        return ragephotodata_getphotosign(rp_data);
    }
    /** Returns the Photo JPEG sign. */
    uint64_t jpegSign(uint32_t photoFormat) const {
        return ragephoto_getphotosignf(instance, photoFormat);
    }
    /** Returns the Photo JPEG sign. */
    uint64_t jpegSign() const {
        return ragephoto_getphotosign(instance);
    }
    /** Returns the Photo JPEG data size. */
    uint32_t jpegSize() const {
        return ragephoto_getphotosize(instance);
    }
    /** Returns the Photo description. */
    const char* description() const {
        return ragephoto_getphotodesc(instance);
    }
    /** Returns the Photo JSON data. */
    const char* json() const {
        return ragephoto_getphotojson(instance);
    }
    /** Returns the Photo header. */
    const char* header() const {
        return ragephoto_getphotoheader(instance);
    }
    /** Returns the Photo title. */
    const char* title() const {
        return ragephoto_getphototitle(instance);
    }
    /** Returns the library version. */
    static const char* version() {
        return ragephoto_version();
    }
    /** Saves a Photo to a char*. */
    static bool save(char *data, uint32_t photoFormat, RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser) {
        return ragephotodata_savef(rp_data, rp_parser, data, photoFormat);
    }
    /** Saves a Photo to a char*. */
    static bool save(char *data, RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser) {
        return ragephotodata_save(rp_data, rp_parser, data);
    }
    /** Saves a Photo to a char*.
    * \param data Photo data
    * \param photoFormat Photo Format (GTA V or RDR 2)
    */
    bool save(char *data, uint32_t photoFormat) {
        return ragephoto_savef(instance, data, photoFormat);
    }
    /** Saves a Photo to a char*.
    * \param data Photo data
    */
    bool save(char *data) {
        return ragephoto_save(instance, data);
    }
    /** Saves a Photo to a std::string.
    * \param photoFormat Photo Format (GTA V or RDR 2)
    * \param ok \p true when saved successfully
    */
    const std::string save(uint32_t photoFormat, bool *ok = nullptr) {
        std::string sdata;
        const size_t size = ragephoto_getsavesizef(instance, photoFormat);
        if (size == 0) {
            if (ok)
                *ok = false;
            return sdata;
        }
        sdata.resize(size);
        const bool saved = ragephoto_savef(instance, &sdata[0], photoFormat);
        if (ok)
            *ok = saved;
        return sdata;
    }
    /** Saves a Photo to a std::string.
    * \param ok \p true when saved successfully
    */
    const std::string save(bool *ok = nullptr) {
        return save(ragephoto_getphotoformat(instance), ok);
    }
    /** Saves a Photo to a file. */
    bool saveFile(const char *filename, uint32_t photoFormat) {
        return ragephoto_savefilef(instance, filename, photoFormat);
    }
    /** Saves a Photo to a file. */
    bool saveFile(const char *filename) {
        return ragephoto_savefile(instance, filename);
    }
    /** Returns the Photo save file size. */
    static size_t saveSize(uint32_t photoFormat, RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser) {
        return ragephotodata_getsavesizef(rp_data, rp_parser, photoFormat);
    }
    /** Returns the Photo save file size. */
    static size_t saveSize(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser) {
        return ragephotodata_getsavesize(rp_data, rp_parser);
    }
    /** Returns the Photo save file size. */
    size_t saveSize(uint32_t photoFormat) {
        return ragephoto_getsavesizef(instance, photoFormat);
    }
    /** Returns the Photo save file size. */
    size_t saveSize() {
        return ragephoto_getsavesize(instance);
    }
    /** Sets all cross-format Buffer to default size. */
    static void setBufferDefault(RagePhotoData *rp_data) {
        ragephotodata_setbufferdefault(rp_data);
    }
    /** Sets all cross-format Buffer to default size. */
    void setBufferDefault() {
        ragephoto_setbufferdefault(instance);
    }
    /** Moves all Buffer offsets to correct position. */
    static void setBufferOffsets(RagePhotoData *rp_data) {
        ragephotodata_setbufferoffsets(rp_data);
    }
    /** Moves all Buffer offsets to correct position. */
    void setBufferOffsets() {
        ragephoto_setbufferoffsets(instance);
    }
    /** Sets the internal RagePhotoData object. */
    bool setData(RagePhotoData *ragePhotoData, bool takeCopy = true) {
        if (takeCopy)
            return ragephoto_setphotodatac(instance, ragePhotoData);
        else
            return ragephoto_setphotodata(&instance, ragePhotoData);
    }
    /** Sets the Photo description. */
    void setDescription(const char *description, uint32_t bufferSize = 0) {
        ragephoto_setphotodesc(instance, description, bufferSize);
    }
    /** Sets the Photo Format (GTA V or RDR 2). */
    void setFormat(uint32_t photoFormat) {
        ragephoto_setphotoformat(instance, photoFormat);
    }
    /** Sets the Photo JPEG data.
    * \param data JPEG data
    * \param size JPEG data size
    * \param bufferSize JPEG buffer size
    */
    bool setJpeg(const char *data, uint32_t size, uint32_t bufferSize = 0) {
        return ragephoto_setphotojpeg(instance, data, size, bufferSize);
    }
    /** Sets the Photo JPEG data.
    * \param data JPEG data
    * \param bufferSize JPEG buffer size
    */
    bool setJpeg(const std::string &data, uint32_t bufferSize = 0) {
        return ragephoto_setphotojpeg(instance, data.data(), static_cast<uint32_t>(data.size()), bufferSize);
    }
    /** Sets the Photo JSON data. */
    void setJson(const char *json, uint32_t bufferSize = 0) {
        ragephoto_setphotojson(instance, json, bufferSize);
    }
    /** Sets the Photo header. (EXPERT ONLY) */
    void setHeader(const char *header, uint32_t headerSum) {
        ragephoto_setphotoheader(instance, header, headerSum);
    }
    /** Sets the Photo title. */
    void setTitle(const char *title, uint32_t bufferSize = 0) {
        ragephoto_setphototitle(instance, title, bufferSize);
    }

private:
    ragephoto_t instance;
};
#else
#include "RagePhotoC.h"
#endif // __cplusplus

#endif // RAGEPHOTOA_H
