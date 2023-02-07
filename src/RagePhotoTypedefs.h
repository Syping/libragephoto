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
    uint32_t descBuffer; /**< Photo Description buffer length */
    uint32_t descOffset; /**< Photo Description buffer offset */
    uint32_t endOfFile; /**< Photo End Of File offset */
    uint32_t headerSum; /**< Photo Checksum of the header */
    uint32_t jpegBuffer; /**< Photo JPEG buffer length */
    uint32_t jpegSize; /**< Internal JPEG buffer length and size of JPEG */
    uint32_t jsonBuffer; /**< Photo JSON buffer length */
    uint32_t jsonOffset; /**< Photo JSON buffer offset */
    uint32_t photoFormat; /**< Photo file format magic */
    uint32_t titlBuffer; /**< Photo Title buffer length */
    uint32_t titlOffset; /**< Photo Title buffer offset */
    uint32_t unnamedSum1; /**< 1st unnamed checksum for Red Dead Redemption 2 */
    uint32_t unnamedSum2; /**< 2nd unnamed checksum for Red Dead Redemption 2 */
} RagePhotoData;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // RAGEPHOTOTYPEDEFS_H
