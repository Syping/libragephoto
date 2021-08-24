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

#include "iconv.h"
#include "stdio.h"

int main(int argc, char *argv[])
{
    iconv_t instance = iconv_open("UTF-16LE", "UTF-8");
    if (instance == (iconv_t)-1)
        return 1;
    char src[] = "Test";
    char dst[256];
    size_t src_s = sizeof(src);
    size_t dst_s = sizeof(dst);
    char *isrc = src;
    char *idst = dst;
    iconv(instance, &isrc, &src_s, &idst, &dst_s);
    iconv_close(instance);
    printf("%s\n", dst);
    return 0;
}
