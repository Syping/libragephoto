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

#ifndef RAGEPHOTOC_H
#define RAGEPHOTOC_H

#include "RagePhotoLibrary.h"
#include "RagePhotoTypedefs.h"
#include <stddef.h>
#include <stdint.h>

#ifdef LIBRAGEPHOTO_C_NOAPI
#error "libragephoto was built without C API"
#endif // LIBRAGEPHOTO_C_NOAPI

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/** C API for RagePhoto.
* \file RagePhotoC.h
*/

/** RagePhoto C++ class typedef for C API. */
typedef void* ragephoto_t;

/** Opens a \p ragephoto_t instance. */
LIBRAGEPHOTO_C_PUBLIC ragephoto_t ragephoto_open();

/** Add a custom defined RagePhotoFormatParser.
* \param instance \p ragephoto_t instance
* \param rp_parser RagePhotoFormatParser parser to add
*/
LIBRAGEPHOTO_C_PUBLIC void ragephoto_addparser(ragephoto_t instance, RagePhotoFormatParser *rp_parser);

/** Resets the RagePhotoData object to default values.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_C_PUBLIC void ragephoto_clear(ragephoto_t instance);

/** Resets the RagePhotoData object to default values.
* \param rp_data RagePhotoData object
*/
LIBRAGEPHOTO_C_PUBLIC void ragephotodata_clear(RagePhotoData *rp_data);

/** Loads a Photo from a const char*.
* \param instance \p ragephoto_t instance
* \param data Photo data
* \param size Photo data size
*/
LIBRAGEPHOTO_C_PUBLIC bool ragephoto_load(ragephoto_t instance, const char *data, size_t size);

/** Loads a Photo from a const char*.
* \param rp_data RagePhotoData object
* \param rp_parser RagePhotoFormatParser parser array
* \param data Photo data
* \param size Photo data size
*/
LIBRAGEPHOTO_C_PUBLIC bool ragephotodata_load(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser, const char *data, size_t size);

/** Loads a Photo from a file.
* \param instance \p ragephoto_t instance
* \param filename File to load
*/
LIBRAGEPHOTO_C_PUBLIC bool ragephoto_loadfile(ragephoto_t instance, const char *filename);

/** Returns the last error occurred.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_C_PUBLIC int32_t ragephoto_error(ragephoto_t instance);

/** Returns the GTA V default Photo Buffer Size. */
LIBRAGEPHOTO_C_PUBLIC uint32_t ragephoto_defpbuf_gta5();

/** Returns the RDR 2 default Photo Buffer Size. */
LIBRAGEPHOTO_C_PUBLIC uint32_t ragephoto_defpbuf_rdr2();

/** Returns the GTA V Photo Format. */
LIBRAGEPHOTO_C_PUBLIC uint32_t ragephoto_format_gta5();

/** Returns the RDR 2 Photo Format. */
LIBRAGEPHOTO_C_PUBLIC uint32_t ragephoto_format_rdr2();

/** Returns the internal RagePhotoData object.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_C_PUBLIC RagePhotoData* ragephoto_getphotodata(ragephoto_t instance);

/** Returns the Photo description.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_C_PUBLIC const char* ragephoto_getphotodesc(ragephoto_t instance);

/** Returns the Photo Format (GTA V or RDR 2).
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_C_PUBLIC uint32_t ragephoto_getphotoformat(ragephoto_t instance);

/** Returns the Photo JPEG data.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_C_PUBLIC const char* ragephoto_getphotojpeg(ragephoto_t instance);

/** Returns the Photo JSON data.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_C_PUBLIC const char* ragephoto_getphotojson(ragephoto_t instance);

/** Returns the Photo header.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_C_PUBLIC const char* ragephoto_getphotoheader(ragephoto_t instance);

/** Returns the Photo JPEG sign.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_C_PUBLIC uint64_t ragephoto_getphotosign(ragephoto_t instance);

/** Returns the Photo JPEG sign.
* \param instance \p ragephoto_t instance
* \param photoFormat Photo Format (GTA V or RDR 2)
*/
LIBRAGEPHOTO_C_PUBLIC uint64_t ragephoto_getphotosignf(ragephoto_t instance, uint32_t photoFormat);

/** Returns the Photo JPEG sign.
* \param rp_data RagePhotoData object
*/
LIBRAGEPHOTO_C_PUBLIC uint64_t ragephotodata_getphotosign(RagePhotoData *rp_data);

/** Returns the Photo JPEG sign.
* \param rp_data RagePhotoData object
* \param photoFormat Photo Format (GTA V or RDR 2)
*/
LIBRAGEPHOTO_C_PUBLIC uint64_t ragephotodata_getphotosignf(RagePhotoData *rp_data, uint32_t photoFormat);

/** Returns the Photo JPEG data size.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_C_PUBLIC uint32_t ragephoto_getphotosize(ragephoto_t instance);

/** Returns the Photo title.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_C_PUBLIC const char* ragephoto_getphototitle(ragephoto_t instance);

/** Returns the Photo save file size.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_C_PUBLIC size_t ragephoto_getsavesize(ragephoto_t instance);

/** Returns the Photo save file size.
* \param rp_data RagePhotoData object
* \param rp_parser RagePhotoFormatParser parser array
*/
LIBRAGEPHOTO_C_PUBLIC size_t ragephotodata_getsavesize(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser);

/** Returns the Photo save file size.
* \param instance \p ragephoto_t instance
* \param photoFormat Photo Format (GTA V or RDR 2)
*/
LIBRAGEPHOTO_C_PUBLIC size_t ragephoto_getsavesizef(ragephoto_t instance, uint32_t photoFormat);

/** Returns the Photo save file size.
* \param rp_data RagePhotoData object
* \param rp_parser RagePhotoFormatParser parser array
* \param photoFormat Photo Format (GTA V or RDR 2)
*/
LIBRAGEPHOTO_C_PUBLIC size_t ragephotodata_getsavesizef(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser, uint32_t photoFormat);

/** Saves a Photo to a char*.
* \param instance \p ragephoto_t instance
* \param data Photo data
*/
LIBRAGEPHOTO_C_PUBLIC bool ragephoto_save(ragephoto_t instance, char *data);

/** Saves a Photo to a char*.
* \param rp_data RagePhotoData object
* \param rp_parser RagePhotoFormatParser parser array
* \param data Photo data
*/
LIBRAGEPHOTO_C_PUBLIC bool ragephotodata_save(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser, char *data);

/** Saves a Photo to a char*.
* \param instance \p ragephoto_t instance
* \param data Photo data
* \param photoFormat Photo Format (GTA V or RDR 2)
*/
LIBRAGEPHOTO_C_PUBLIC bool ragephoto_savef(ragephoto_t instance, char *data, uint32_t photoFormat);

/** Saves a Photo to a char*.
* \param rp_data RagePhotoData object
* \param rp_parser RagePhotoFormatParser parser array
* \param data Photo data
* \param photoFormat Photo Format (GTA V or RDR 2)
*/
LIBRAGEPHOTO_C_PUBLIC bool ragephotodata_savef(RagePhotoData *rp_data, RagePhotoFormatParser *rp_parser, char *data, uint32_t photoFormat);

/** Saves a Photo to a file.
* \param instance \p ragephoto_t instance
* \param filename File to save
*/
LIBRAGEPHOTO_C_PUBLIC bool ragephoto_savefile(ragephoto_t instance, const char *filename);

/** Saves a Photo to a file.
* \param instance \p ragephoto_t instance
* \param filename File to save
* \param photoFormat Photo Format (GTA V or RDR 2)
*/
LIBRAGEPHOTO_C_PUBLIC bool ragephoto_savefilef(ragephoto_t instance, const char *filename, uint32_t photoFormat);

/** Sets all cross-format Buffer to default size.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_C_PUBLIC void ragephoto_setbufferdefault(ragephoto_t instance);

/** Sets all cross-format Buffer to default size.
* \param rp_data RagePhotoData object
*/
LIBRAGEPHOTO_C_PUBLIC void ragephotodata_setbufferdefault(RagePhotoData *rp_data);

/** Moves all Buffer offsets to correct position.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_C_PUBLIC void ragephoto_setbufferoffsets(ragephoto_t instance);

/** Moves all Buffer offsets to correct position.
* \param rp_data RagePhotoData object
*/
LIBRAGEPHOTO_C_PUBLIC void ragephotodata_setbufferoffsets(RagePhotoData *rp_data);

/** Sets the internal RagePhotoData object.
* \param instance \p ragephoto_t instance
* \param rp_data RagePhotoData object being set
*/
LIBRAGEPHOTO_C_PUBLIC bool ragephoto_setphotodata(ragephoto_t instance, RagePhotoData *rp_data);

/** Copies RagePhotoData object to internal RagePhotoData object.
* \param instance \p ragephoto_t instance
* \param rp_data RagePhotoData object being copied
*/
LIBRAGEPHOTO_C_PUBLIC bool ragephoto_setphotodatac(ragephoto_t instance, RagePhotoData *rp_data);

/** Sets the Photo description.
* \param instance \p ragephoto_t instance
* \param description Description
* \param bufferSize Description buffer size
*
* Default bufferSize: 256UL
*/
LIBRAGEPHOTO_C_PUBLIC void ragephoto_setphotodesc(ragephoto_t instance, const char *description, uint32_t bufferSize);

/** Sets the Photo Format (GTA V or RDR 2).
* \param instance \p ragephoto_t instance
* \param photoFormat Photo Format (GTA V or RDR 2)
*/
LIBRAGEPHOTO_C_PUBLIC void ragephoto_setphotoformat(ragephoto_t instance, uint32_t photoFormat);

/** Sets the Photo JPEG data.
* \param instance \p ragephoto_t instance
* \param data JPEG data
* \param size JPEG data size
* \param bufferSize JPEG buffer size
*
* Default bufferSize: ragephoto_defpbuf_gta5() or ragephoto_defpbuf_rdr2()
*/
LIBRAGEPHOTO_C_PUBLIC bool ragephoto_setphotojpeg(ragephoto_t instance, const char *data, uint32_t size, uint32_t bufferSize);

/** Sets the Photo JSON data.
* \param instance \p ragephoto_t instance
* \param json JSON data
* \param bufferSize JSON data buffer size
*
* Default bufferSize: 3072UL
*/
LIBRAGEPHOTO_C_PUBLIC void ragephoto_setphotojson(ragephoto_t instance, const char *json, uint32_t bufferSize);

/** Sets the Photo header. */
LIBRAGEPHOTO_C_PUBLIC void ragephoto_setphotoheader(ragephoto_t instance, const char *header, uint32_t headerSum);

/** Sets the Photo header. (RDR 2) */
LIBRAGEPHOTO_C_PUBLIC void ragephoto_setphotoheader2(ragephoto_t instance, const char *header, uint32_t headerSum, uint32_t headerSum2);

/** Sets the Photo title.
* \param instance \p ragephoto_t instance
* \param title Title
* \param bufferSize Title buffer size
*
* Default bufferSize: 256UL
*/
LIBRAGEPHOTO_C_PUBLIC void ragephoto_setphototitle(ragephoto_t instance, const char *title, uint32_t bufferSize);

/** Closes a \p ragephoto_t instance.
* \param instance \p ragephoto_t instance
*/
LIBRAGEPHOTO_C_PUBLIC void ragephoto_close(ragephoto_t instance);

/** Returns the library version. */
LIBRAGEPHOTO_C_PUBLIC const char* ragephoto_version();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // RAGEPHOTOC_H
