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

#include "RagePhoto.h"
#include <fstream>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " [photo] [jpegout]" << std::endl;
        return 0;
    }

    // Initialise RagePhoto
    RagePhoto ragePhoto;

    // Load Photo
    const bool loaded = ragePhoto.loadFile(argv[1]);

    if (!loaded) {
        if (ragePhoto.error() == RagePhoto::Uninitialised) {
            std::cout << "Failed to open file: " << argv[1] << std::endl;
            return 1;
        }
        else if (ragePhoto.error() <= RagePhoto::PhotoReadError) {
            std::cout << "Failed to load photo" << std::endl;
            return 1;
        }
    }

    // Write jpeg
    std::ofstream ofs(argv[2], std::ios::out | std::ios::binary | std::ios::trunc);
    if (!ofs.is_open()) {
        std::cout << "Failed to write file: " << argv[2] << std::endl;
        return 1;
    }
    ofs << ragePhoto.photo();
    const bool ok = ofs.good();
    ofs.close();

    if (!ok) {
        std::cout << "Failed to write file: " << argv[2] << std::endl;
        return 1;
    }

    if (ragePhoto.format() == RagePhoto::GTA5)
        std::cout << "GTA V Photo successfully exported" << std::endl;
    else
        std::cout << "RDR 2 Photo successfully exported" << std::endl;

    return 0;
}
