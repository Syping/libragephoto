/*****************************************************************************
* libragephoto RAGE Photo Parser
* Copyright (C) 2023-2024 Syping
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

#ifndef RAGEPHOTO_HPP
#define RAGEPHOTO_HPP

#ifdef __cplusplus
#include "RagePhotoLibrary.h"
#ifdef LIBRAGEPHOTO_CXX_ONLY
#include "ragephoto_cxx.hpp"
typedef ragephoto::photo RagePhoto;
#elif defined LIBRAGEPHOTO_CXX_C
#ifdef LIBRAGEPHOTO_STATIC
#include "ragephoto_cxx.hpp"
typedef ragephoto::photo RagePhoto;
#else
#ifdef LIBRAGEPHOTO_PREFER_NATIVE
#include "ragephoto_cxx.hpp"
typedef ragephoto::photo RagePhoto;
#else
#include "ragephoto_c.hpp"
typedef ragephoto::c_wrapper::photo RagePhoto;
#endif // LIBRAGEPHOTO_PREFER_NATIVE
#endif // LIBRAGEPHOTO_STATIC
#elif defined LIBRAGEPHOTO_C_ONLY
#include "ragephoto_c.hpp"
typedef ragephoto::c_wrapper::photo RagePhoto;
#else
#error "Could not determine best RagePhoto implementation, libragephoto installation might be corrupt!"
#endif // LIBRAGEPHOTO_CXX_ONLY
#endif // __cplusplus

#endif // RAGEPHOTO_HPP
