/*****************************************************************************
* libragephoto RAGE Photo Parser
* Copyright (C) 2023-2025 Syping
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

#ifndef RAGEPHOTOLIBRARY_H
#define RAGEPHOTOLIBRARY_H

#include "RagePhotoConfig.h"

/* RAGEPHOTO LIBRARY BINDING BEGIN */
#ifdef _WIN32
    #ifndef LIBRAGEPHOTO_STATIC
        #ifdef LIBRAGEPHOTO_LIBRARY
            #define LIBRAGEPHOTO_C_PUBLIC __declspec(dllexport)
            #define LIBRAGEPHOTO_CXX_PUBLIC __declspec(dllexport)
        #else
            #define LIBRAGEPHOTO_C_PUBLIC __declspec(dllimport)
            #define LIBRAGEPHOTO_CXX_PUBLIC __declspec(dllimport)
        #endif // LIBRAGEPHOTO_LIBRARY
    #else
        #define LIBRAGEPHOTO_C_PUBLIC
        #define LIBRAGEPHOTO_CXX_PUBLIC
    #endif // LIBRAGEPHOTO_STATIC
#else
    #ifdef __EMSCRIPTEN__
        #include <emscripten/emscripten.h>
        #define LIBRAGEPHOTO_C_PUBLIC EMSCRIPTEN_KEEPALIVE
        #define LIBRAGEPHOTO_CXX_PUBLIC
    #else
        #ifdef __GNUC__
            #ifndef LIBRAGEPHOTO_STATIC
                #define LIBRAGEPHOTO_C_PUBLIC __attribute__((visibility("default")))
                #define LIBRAGEPHOTO_CXX_PUBLIC __attribute__((visibility("default")))
            #else
                #define LIBRAGEPHOTO_C_PUBLIC
                #define LIBRAGEPHOTO_CXX_PUBLIC
            #endif // LIBRAGEPHOTO_STATIC
        #else
            #define LIBRAGEPHOTO_C_PUBLIC
            #define LIBRAGEPHOTO_CXX_PUBLIC
        #endif // __GNUC__
    #endif // __EMSCRIPTEN__
#endif // _WIN32
/* RAGEPHOTO LIBRARY BINDING END */

/* RAGEPHOTO LIBRARY ENDIAN START */
#if defined(_WIN32)
    #define LIBRAGEPHOTO_LITTLE_ENDIAN
#elif defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define LIBRAGEPHOTO_LITTLE_ENDIAN
#elif defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && __BYTE_ORDER == __LITTLE_ENDIAN
    #define LIBRAGEPHOTO_LITTLE_ENDIAN
#elif defined(_BYTE_ORDER) && defined(_LITTLE_ENDIAN) && _BYTE_ORDER == _LITTLE_ENDIAN
    #define LIBRAGEPHOTO_LITTLE_ENDIAN
#elif defined(__APPLE__)
    #include <machine/endian.h>
    #if defined(__DARWIN_BYTE_ORDER) && defined(__DARWIN_LITTLE_ENDIAN) && __DARWIN_BYTE_ORDER == __DARWIN_LITTLE_ENDIAN
        #define LIBRAGEPHOTO_LITTLE_ENDIAN
    #endif // __DARWIN_BYTE_ORDER
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    #include <sys/endian.h>
    #if defined(_BYTE_ORDER) && defined(_LITTLE_ENDIAN) && _BYTE_ORDER == _LITTLE_ENDIAN
        #define LIBRAGEPHOTO_LITTLE_ENDIAN
    #endif // _BYTE_ORDER
#elif defined(__linux__)
    #include <endian.h>
    #if defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && __BYTE_ORDER == __LITTLE_ENDIAN
        #define LIBRAGEPHOTO_LITTLE_ENDIAN
    #endif // __BYTE_ORDER
#endif
/* RAGEPHOTO LIBRARY ENDIAN END */

/* ENABLE C API FOR LIBRAGEPHOTO WASM LIBRARY BEGIN */
#ifdef LIBRAGEPHOTO_WASM
    #ifdef LIBRAGEPHOTO_CXX_ONLY
        #undef LIBRAGEPHOTO_CXX_ONLY
        #define LIBRAGEPHOTO_CXX_C
    #endif // LIBRAGEPHOTO_CXX_ONLY
#endif // LIBRAGEPHOTO_WASM
/* ENABLE C API FOR LIBRAGEPHOTO WASM LIBRARY END */

#endif // RAGEPHOTOLIBRARY_H
