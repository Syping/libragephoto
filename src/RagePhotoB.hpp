/*****************************************************************************
* libragephoto RAGE Photo Parser
* Copyright (C) 2023 Syping
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

#ifndef RAGEPHOTOB_HPP
#define RAGEPHOTOB_HPP

#ifdef __cplusplus
#include "RagePhotoLibrary.h"
#ifdef LIBRAGEPHOTO_CXX_ONLY
#include "RagePhoto.hpp"
typedef RagePhoto RagePhotoB;
#elif defined LIBRAGEPHOTO_CXX_C
#ifdef LIBRAGEPHOTO_STATIC
#include "RagePhoto.hpp"
typedef RagePhoto RagePhotoB;
#else
#include "RagePhotoA.hpp"
typedef RagePhotoA RagePhotoB;
#endif // LIBRAGEPHOTO_STATIC
#elif defined LIBRAGEPHOTO_C_ONLY
#include "RagePhotoA.hpp"
typedef RagePhotoA RagePhotoB;
#else
#error "Could not determine best RagePhoto implementation, libragephoto installation might be corrupt!"
#endif // LIBRAGEPHOTO_CXX_ONLY
#endif // __cplusplus

#endif // RAGEPHOTOB_HPP
