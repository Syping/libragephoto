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

#include "RagePhoto.h"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " [snapmatic] [jpegout]" << std::endl;
        return 0;
    }

    // Initialise RagePhoto
    RagePhoto ragePhoto;

    // Read file
    FILE *file = fopen(argv[1], "rb");
    if (!file)
        return -1;
    const int fseek_end_value = fseek(file, 0, SEEK_END);
    if (fseek_end_value == -1) {
        fclose(file);
        return -1;
    }
    size_t file_size = ftell(file);
    if (file_size == -1) {
        fclose(file);
        return -1;
    }
    const int fseek_set_value = fseek(file, 0, SEEK_SET);
    if (fseek_set_value == -1) {
        fclose(file);
        return -1;
    }
    char *data = static_cast<char*>(malloc(file_size));
    const size_t file_rsize = fread(data, 1, file_size, file);
    if (file_size != file_rsize) {
        fclose(file);
        return -1;
    }

    ragePhoto.load(data, file_size);
    fclose(file);

    // Write jpeg
    file = fopen(argv[2], "wb");
    if (!file)
        return -1;
    fwrite(ragePhoto.photoData(), sizeof(char), ragePhoto.photoSize(), file);
    fclose(file);

    return 0;
}
