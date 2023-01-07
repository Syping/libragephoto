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

#include "RagePhotoC.h"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s [photo] [jpegout]\n", argv[0]);
        return 0;
    }

    // Initialise RagePhoto
    ragephoto_t ragephoto_in = ragephoto_open();

    // Load Photo
    const int loaded = ragephoto_loadfile(ragephoto_in, argv[1]);

    if (loaded != 1) {
        if (ragephoto_error(ragephoto_in) == 0) {
            printf("Failed to open file: %s\n", argv[1]);
            return 1;
        }
        else if (ragephoto_getphotosize(ragephoto_in) <= 0) {
            printf("Failed to load photo\n");
            return 1;
        }
    }

    // Write jpeg
#ifdef _WIN32
    FILE *file = NULL;
    fopen_s(&file, argv[2], "wb");
#else
    FILE *file = fopen(argv[2], "wb");
#endif
    if (!file) {
        printf("Failed to write file: %s\n", argv[2]);
        return 1;
    }
    const size_t size = fwrite(ragephoto_getphotojpeg(ragephoto_in), sizeof(char), ragephoto_getphotosize(ragephoto_in), file);
    fclose(file);

    if (size != ragephoto_getphotosize(ragephoto_in)) {
        printf("Failed to write file: %s\n", argv[2]);
        return 1;
    }

    if (ragephoto_getphotoformat(ragephoto_in) == ragephoto_format_gta5())
        printf("GTA V Photo successfully exported\n");
    else
        printf("RDR 2 Photo successfully exported\n");

    ragephoto_close(ragephoto_in);

    return 0;
}
