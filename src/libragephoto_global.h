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

#ifndef LIBRAGEPHOTO_GLOBAL_H
#define LIBRAGEPHOTO_GLOBAL_H

#ifdef _WIN32
#ifndef LIBRAGEPHOTO_STATIC
#ifdef LIBRAGEPHOTO_LIBRARY
#define LIBRAGEPHOTO_EXPORT __declspec(dllexport)
#else
#define LIBRAGEPHOTO_EXPORT __declspec(dllimport)
#endif
#else
#define LIBRAGEPHOTO_EXPORT
#endif
#else
#define LIBRAGEPHOTO_EXPORT
#endif

#endif // LIBRAGEPHOTO_GLOBAL_H
