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

#ifndef RAGEPHOTOC_H
#define RAGEPHOTOC_H

#include "libragephoto_global.h"
#include <stdint.h>
#include <stdio.h>

#ifdef LIBRAGEPHOTO_C_NOAPI
#error "libragephoto was built without C API"
#endif // LIBRAGEPHOTO_C_NOAPI

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/** \file RagePhotoC.h */
/** RagePhoto C++ class typedef for C API. */
typedef void* ragephoto_t;

/** Opens a \p ragephoto_t instance. */
LIBRAGEPHOTO_EXPORT ragephoto_t ragephoto_open();

/** Resets the \p ragephoto_t instance to default values.
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
* \param bufferSize Description buffer size
*
* Default bufferSize: 256UL
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
*
* Default bufferSize: ragephoto_defpbuf_gta5() or ragephoto_defpbuf_rdr2()
*/
LIBRAGEPHOTO_EXPORT int ragephoto_setphotojpeg(ragephoto_t instance, const char *data, uint32_t size, uint32_t bufferSize);

/** Sets the Photo JSON data.
* \param instance \p ragephoto_t instance
* \param json JSON data
* \param bufferSize JSON data buffer size
*
* Default bufferSize: 3072UL
*/
LIBRAGEPHOTO_EXPORT void ragephoto_setphotojson(ragephoto_t instance, const char *json, uint32_t bufferSize);

/** Sets the Photo header. (EXPERT ONLY) */
LIBRAGEPHOTO_EXPORT void ragephoto_setphotoheader(ragephoto_t instance, const char *header, uint32_t headerSum);

/** Sets the Photo title.
* \param instance \p ragephoto_t instance
* \param title Title
* \param bufferSize Title buffer size
*
* Default bufferSize: 256UL
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
#endif // __cplusplus

#endif // RAGEPHOTOC_H
