#ifndef ICE_BITMAP_UTIL_H
#define ICE_BITMAP_UTIL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

enum RETCODES
{
    ALLOC_FAILURE,
    OTHER_READ_FAILURE,
    FREAD_FAILURE,
    FWRITE_FAILURE,
    INVALID_HEADER_FORMAT,
    INVALID_COMPRESSION_FORMAT,
    INVALID_PIXEL_FORMAT,
    SUCCESS,
};

int32_t abs_32b (int32_t x);
void handle_code (enum RETCODES code, const char *msg, FILE *failed_file,
                  bool check_errno);

#endif