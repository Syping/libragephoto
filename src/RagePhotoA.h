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
* disadvantages include worse performance, which should be pretty minimal, and not always include the newest features.
*/
class RagePhotoA
{
public:
    RagePhotoA() {
        instance = ragephoto_open();
    }
    ~RagePhotoA() {
        ragephoto_close(instance);
    }
    void clear() {
        ragephoto_clear(instance);
    }
    bool load(const char *data, size_t size) {
        return ragephoto_load(instance, data, size);
    }
    bool load(const std::string &data) {
        return ragephoto_load(instance, data.data(), data.size());
    }
    bool loadFile(const char *filename) {
        return ragephoto_loadfile(instance, filename);
    }
    uint8_t error() const {
        return ragephoto_error(instance);
    }
    uint32_t format() const {
        return ragephoto_getphotoformat(instance);
    }
    const std::string photo() const {
        return std::string(ragephoto_getphotojpeg(instance), ragephoto_getphotosize(instance));
    }
    const char *photoData() const {
        return ragephoto_getphotojpeg(instance);
    }
    uint32_t photoSize() const {
        return ragephoto_getphotosize(instance);
    }
    const char* description() const {
        return ragephoto_getphotodesc(instance);
    }
    const char* json() const {
        return ragephoto_getphotojson(instance);
    }
    const char* header() const {
        return ragephoto_getphotoheader(instance);
    }
    const char* title() const {
        return ragephoto_getphototitle(instance);
    }
    static const char* version() {
        return ragephoto_version();
    }
    bool save(char *data, uint32_t photoFormat) {
        return ragephoto_savef(instance, data, photoFormat);
    }
    bool save(char *data) {
        return ragephoto_save(instance, data);
    }
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
    const std::string save(bool *ok = nullptr) {
        return save(ragephoto_getphotoformat(instance), ok);
    }
    bool saveFile(const char *filename, uint32_t photoFormat) {
        return ragephoto_savefilef(instance, filename, photoFormat);
    }
    bool saveFile(const char *filename) {
        return ragephoto_savefile(instance, filename);
    }
    size_t saveSize(uint32_t photoFormat) {
        return ragephoto_getsavesizef(instance, photoFormat);
    }
    size_t saveSize() {
        return ragephoto_getsavesize(instance);
    }
    void setBufferDefault() {
        ragephoto_setbufferdefault(instance);
    }
    void setBufferOffsets() {
        ragephoto_setbufferoffsets(instance);
    }
    void setDescription(const char *description, uint32_t bufferSize = 0) {
        ragephoto_setphotodesc(instance, description, bufferSize);
    }
    void setFormat(uint32_t photoFormat) {
        ragephoto_setphotoformat(instance, photoFormat);
    }
    void setJson(const char *json, uint32_t bufferSize = 0) {
        ragephoto_setphotojson(instance, json, bufferSize);
    }
    void setHeader(const char *header, uint32_t headerSum) {
        ragephoto_setphotoheader(instance, header, headerSum);
    }
    bool setPhoto(const char *data, uint32_t size, uint32_t bufferSize = 0) {
        return ragephoto_setphotojpeg(instance, data, size, bufferSize);
    }
    bool setPhoto(const std::string &data, uint32_t bufferSize = 0) {
        return ragephoto_setphotojpeg(instance, data.data(), static_cast<uint32_t>(data.size()), bufferSize);
    }
    void setTitle(const char *title, uint32_t bufferSize = 0) {
        ragephoto_setphototitle(instance, title, bufferSize);
    }

private:
    ragephoto_t instance;
};
#endif // __cplusplus

#endif // RAGEPHOTOA_H
