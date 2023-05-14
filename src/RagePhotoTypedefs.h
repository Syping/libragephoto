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

#ifndef RAGEPHOTOTYPEDEFS_H
#define RAGEPHOTOTYPEDEFS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/** RagePhoto data struct for storing internal data. */
typedef struct RagePhotoData {
    char* jpeg; /**< Pointer for internal JPEG buffer */
    char* description; /**< Pointer for internal Description buffer */
    char* json; /**< Pointer for internal JSON buffer */
    char* header; /**< Pointer for internal Header buffer */
    char* title; /**< Pointer for internal Title buffer */
    int32_t error; /**< RagePhoto error code */
    uint32_t descBuffer; /**< Description buffer length */
    uint32_t descOffset; /**< Description buffer offset */
    uint32_t endOfFile; /**< End Of File offset */
    uint32_t headerSum; /**< Checksum of the header 1 */
    uint32_t headerSum2; /**< Checksum of the header 2 (RDR 2 only) */
    uint32_t jpegBuffer; /**< JPEG buffer length */
    uint32_t jpegSize; /**< Internal JPEG buffer length and size of JPEG */
    uint32_t jsonBuffer; /**< JSON buffer length */
    uint32_t jsonOffset; /**< JSON buffer offset */
    uint32_t photoFormat; /**< Photo file format magic */
    uint32_t titlBuffer; /**< Title buffer length */
    uint32_t titlOffset; /**< Title buffer offset */
} RagePhotoData;

/** RagePhoto load function typedef. */
typedef bool (*ragephoto_loadfunc_t)(RagePhotoData*, const char*, size_t);

/** RagePhoto save function typedef (char* allocated by caller). */
typedef bool (*ragephoto_savefunc_t)(RagePhotoData*, char*, uint32_t);

/** RagePhoto save function typedef (char* allocated by function). */
typedef bool (*ragephoto_savepfunc_t)(RagePhotoData*, char**, uint32_t);

/** RagePhoto saveSize function typedef. */
typedef size_t (*ragephoto_saveszfunc_t)(RagePhotoData*, uint32_t);

/** RagePhoto format parser struct for registering custom formats. */
typedef struct RagePhotoFormatParser {
    uint32_t photoFormat; /**< Photo file format magic */
    ragephoto_loadfunc_t funcLoad; /**< Pointer to load function */
    ragephoto_savefunc_t funcSave; /**< Pointer to save function */
    ragephoto_savepfunc_t funcSavep; /**< Pointer to savep function */
    ragephoto_saveszfunc_t funcSaveSz; /**< Pointer to saveSize function */
} RagePhotoFormatParser;

/* RagePhoto default sizes */
#define RAGEPHOTO_DEFAULT_GTA5_PHOTOBUFFER 524288UL /**< GTA V default Photo Buffer Size */
#define RAGEPHOTO_DEFAULT_RDR2_PHOTOBUFFER 1048576UL /**< RDR 2 default Photo Buffer Size */
#define RAGEPHOTO_DEFAULT_DESCBUFFER 256UL /**< Default Description Buffer Size */
#define RAGEPHOTO_DEFAULT_JSONBUFFER 3072UL /**< Default JSON Buffer Size */
#define RAGEPHOTO_DEFAULT_TITLBUFFER 256UL /**< Default Title Buffer Size */
#define RAGEPHOTO_GTA5_HEADERSIZE 264UL /**< GTA V Header Size */
#define RAGEPHOTO_RDR2_HEADERSIZE 272UL /**< RDR 2 Header Size */

/* RagePhoto error codes */
#define RAGEPHOTO_ERROR_DESCBUFFERTIGHT 39L /**< Description Buffer is too tight */
#define RAGEPHOTO_ERROR_DESCMALLOCERROR 31L /**< Description Buffer can't be allocated */
#define RAGEPHOTO_ERROR_DESCREADERROR 32L /**< Description can't be read successfully */
#define RAGEPHOTO_ERROR_HEADERBUFFERTIGHT 35L /**< Header Buffer is too tight */
#define RAGEPHOTO_ERROR_HEADERMALLOCERROR 4L /**< Header Buffer can't be allocated */
#define RAGEPHOTO_ERROR_INCOMPATIBLEFORMAT 2L /**< Format is incompatible */
#define RAGEPHOTO_ERROR_INCOMPLETECHECKSUM 7L /**< Header checksum is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEDESCBUFFER 30L /**< Description Buffer Size is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEDESCMARKER 28L /**< Description Marker is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEDESCOFFSET 11L /**< Description Offset is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEEOF 8L /**< End Of File Offset is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEHEADER 3L /**< Header is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEJENDMARKER 33L /**< JEND Marker is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEJPEGMARKER 12L /**< JPEG Marker is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEJSONBUFFER 20L /**< JSON Buffer Size is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEJSONMARKER 18L /**< JSON Marker incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEJSONOFFSET 9L /**< JSON Offset incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEPHOTOBUFFER 14L /**< Photo Buffer Size is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEPHOTOSIZE 15L /**< Photo Size is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETETITLEBUFFER 25L /**< Title Buffer Size is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETETITLEMARKER 23L /**< Title Marker is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETETITLEOFFSET 10L /**< Title Offset is incomplete */
#define RAGEPHOTO_ERROR_INCORRECTDESCMARKER 29L /**< Description Marker is incorrect */
#define RAGEPHOTO_ERROR_INCORRECTJENDMARKER 34L /**< JEND Marker is incorrect */
#define RAGEPHOTO_ERROR_INCORRECTJPEGMARKER 13L /**< JPEG Marker is incorrect */
#define RAGEPHOTO_ERROR_INCORRECTJSONMARKER 19L /**< JSON Marker is incorrect */
#define RAGEPHOTO_ERROR_INCORRECTTITLEMARKER 24L /**< Title Marker is incorrect */
#define RAGEPHOTO_ERROR_JSONBUFFERTIGHT 37L /**< JSON Buffer is too tight */
#define RAGEPHOTO_ERROR_JSONMALLOCERROR 21L /**< JSON Buffer can't be allocated */
#define RAGEPHOTO_ERROR_JSONREADERROR 22L /**< JSON can't be read successfully */
#define RAGEPHOTO_ERROR_NOERROR 255L /**< Finished without errors */
#define RAGEPHOTO_ERROR_NOFORMATIDENTIFIER 1L /**< No format detected, empty file */
#define RAGEPHOTO_ERROR_PHOTOBUFFERTIGHT 36L /**< Photo Buffer is too tight */
#define RAGEPHOTO_ERROR_PHOTOMALLOCERROR 16L /**< Photo Buffer can't be allocated */
#define RAGEPHOTO_ERROR_PHOTOREADERROR 17L /**< Photo can't be read */
#define RAGEPHOTO_ERROR_TITLEBUFFERTIGHT 38L /**< Title Buffer is too tight */
#define RAGEPHOTO_ERROR_TITLEMALLOCERROR 26L /**< Title Buffer can't be allocated */
#define RAGEPHOTO_ERROR_TITLEREADERROR 27L /**< Title can't be read */
#define RAGEPHOTO_ERROR_UNICODEINITERROR 5L /**< Failed to initialise Unicode decoder */
#define RAGEPHOTO_ERROR_UNICODEHEADERERROR 6L /**< Header can't be encoded/decoded successfully */
#define RAGEPHOTO_ERROR_UNINITIALISED 0L /**< Uninitialised, file access failed */

/* RagePhoto formats */
#define RAGEPHOTO_FORMAT_GTA5 0x01000000UL /**< GTA V Photo Format */
#define RAGEPHOTO_FORMAT_RDR2 0x04000000UL /**< RDR 2 Photo Format */

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // RAGEPHOTOTYPEDEFS_H
