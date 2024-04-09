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

/** RagePhoto instance struct for storing data and format parser pointer.
* \struct RagePhotoInstance RagePhoto.h
*/
typedef struct RagePhotoInstance {
    RagePhotoData *data; /**< Pointer for internal data */
    RagePhotoFormatParser *parser; /**< Pointer for internal format parser */
} RagePhotoInstance;

/* RagePhoto default sizes */
#define RAGEPHOTO_DEFAULT_GTA5_PHOTOBUFFER UINT32_C(524288) /**< GTA V default Photo Buffer Size */
#define RAGEPHOTO_DEFAULT_RDR2_PHOTOBUFFER UINT32_C(1048576) /**< RDR 2 default Photo Buffer Size */
#define RAGEPHOTO_DEFAULT_DESCBUFFER UINT32_C(256) /**< Default Description Buffer Size */
#define RAGEPHOTO_DEFAULT_JSONBUFFER UINT32_C(3072) /**< Default JSON Buffer Size */
#define RAGEPHOTO_DEFAULT_TITLBUFFER UINT32_C(256) /**< Default Title Buffer Size */
#define RAGEPHOTO_GTA5_HEADERSIZE UINT32_C(264) /**< GTA V Header Size */
#define RAGEPHOTO_RDR2_HEADERSIZE UINT32_C(272) /**< RDR 2 Header Size */

/* RagePhoto error codes */
#define RAGEPHOTO_ERROR_DESCBUFFERTIGHT INT32_C(39) /**< Description Buffer is too tight */
#define RAGEPHOTO_ERROR_DESCMALLOCERROR INT32_C(31) /**< Description Buffer can't be allocated */
#define RAGEPHOTO_ERROR_DESCREADERROR INT32_C(32) /**< Description can't be read successfully */
#define RAGEPHOTO_ERROR_HEADERBUFFERTIGHT INT32_C(35) /**< Header Buffer is too tight */
#define RAGEPHOTO_ERROR_HEADERMALLOCERROR INT32_C(4) /**< Header Buffer can't be allocated */
#define RAGEPHOTO_ERROR_INCOMPATIBLEFORMAT INT32_C(2) /**< Format is incompatible */
#define RAGEPHOTO_ERROR_INCOMPLETECHECKSUM INT32_C(7) /**< Header checksum is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEDESCBUFFER INT32_C(30) /**< Description Buffer Size is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEDESCMARKER INT32_C(28) /**< Description Marker is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEDESCOFFSET INT32_C(11) /**< Description Offset is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEEOF INT32_C(8) /**< End Of File Offset is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEHEADER INT32_C(3) /**< Header is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEJENDMARKER INT32_C(33) /**< JEND Marker is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEJPEGMARKER INT32_C(12) /**< JPEG Marker is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEJSONBUFFER INT32_C(20) /**< JSON Buffer Size is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEJSONMARKER INT32_C(18) /**< JSON Marker incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEJSONOFFSET INT32_C(9) /**< JSON Offset incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEPHOTOBUFFER INT32_C(14) /**< Photo Buffer Size is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETEPHOTOSIZE INT32_C(15) /**< Photo Size is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETETITLEBUFFER INT32_C(25) /**< Title Buffer Size is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETETITLEMARKER INT32_C(23) /**< Title Marker is incomplete */
#define RAGEPHOTO_ERROR_INCOMPLETETITLEOFFSET INT32_C(10) /**< Title Offset is incomplete */
#define RAGEPHOTO_ERROR_INCORRECTDESCMARKER INT32_C(29) /**< Description Marker is incorrect */
#define RAGEPHOTO_ERROR_INCORRECTJENDMARKER INT32_C(34) /**< JEND Marker is incorrect */
#define RAGEPHOTO_ERROR_INCORRECTJPEGMARKER INT32_C(13) /**< JPEG Marker is incorrect */
#define RAGEPHOTO_ERROR_INCORRECTJSONMARKER INT32_C(19) /**< JSON Marker is incorrect */
#define RAGEPHOTO_ERROR_INCORRECTTITLEMARKER INT32_C(24) /**< Title Marker is incorrect */
#define RAGEPHOTO_ERROR_JSONBUFFERTIGHT INT32_C(37) /**< JSON Buffer is too tight */
#define RAGEPHOTO_ERROR_JSONMALLOCERROR INT32_C(21) /**< JSON Buffer can't be allocated */
#define RAGEPHOTO_ERROR_JSONREADERROR INT32_C(22) /**< JSON can't be read successfully */
#define RAGEPHOTO_ERROR_NOERROR INT32_C(255) /**< Finished without errors */
#define RAGEPHOTO_ERROR_NOFORMATIDENTIFIER INT32_C(1) /**< No format detected, empty file */
#define RAGEPHOTO_ERROR_PHOTOBUFFERTIGHT INT32_C(36) /**< Photo Buffer is too tight */
#define RAGEPHOTO_ERROR_PHOTOMALLOCERROR INT32_C(16) /**< Photo Buffer can't be allocated */
#define RAGEPHOTO_ERROR_PHOTOREADERROR INT32_C(17) /**< Photo can't be read */
#define RAGEPHOTO_ERROR_TITLEBUFFERTIGHT INT32_C(38) /**< Title Buffer is too tight */
#define RAGEPHOTO_ERROR_TITLEMALLOCERROR INT32_C(26) /**< Title Buffer can't be allocated */
#define RAGEPHOTO_ERROR_TITLEREADERROR INT32_C(27) /**< Title can't be read */
#define RAGEPHOTO_ERROR_UNICODEINITERROR INT32_C(5) /**< Failed to initialise Unicode decoder */
#define RAGEPHOTO_ERROR_UNICODEHEADERERROR INT32_C(6) /**< Header can't be encoded/decoded successfully */
#define RAGEPHOTO_ERROR_UNINITIALISED INT32_C(0) /**< Uninitialised, file access failed */

/* RagePhoto formats */
#define RAGEPHOTO_FORMAT_GTA5 UINT32_C(0x01000000) /**< GTA V Photo Format */
#define RAGEPHOTO_FORMAT_RDR2 UINT32_C(0x04000000) /**< RDR 2 Photo Format */

/* RagePhoto sign initials */
#define RAGEPHOTO_SIGNINITIAL_GTA5 UINT32_C(0xE47AB81C) /**< GTA V Sign Initial */
#define RAGEPHOTO_SIGNINITIAL_RDR2 UINT32_C(0x00FEEB1E) /**< RDR 2 Sign Initial */

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // RAGEPHOTOTYPEDEFS_H
