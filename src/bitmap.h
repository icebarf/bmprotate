//  This file is part of bmprotate
//  bmprotate is free software: you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by the
//  Free Software Foundation, either version 3 of the License,
//  or (at your option) any later version.
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//  You should have received a copy of the GNU General Public License
//  along with this program. If not, see <https://www.gnu.org/licenses/>.

#ifndef ICE_BITMAP_HEADER_H
#define ICE_BITMAP_HEADER_H

#include <stdint.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int32_t i32;
typedef int16_t i16;

struct __attribute ((packed)) BITMAP_HEADER
{
    u16 bf_id;
    u32 bf_fsize;
    u32 bf_reserved;
    u32 bf_pixels_offset;

    u32 bi_size;
    i32 bi_width;
    i32 bi_height;
    u16 bi_panes;
    u16 bi_bitcount;
    u32 bi_compression;
    u32 bi_sizeimage;
    u32 bi_xpixels_permeter;
    u32 bi_ypixels_permeter;
    u32 bi_clrused;
    u32 bi_clrimportant;
};

struct Pixel_32bits
{
    u8 alpha;
    u8 blue;
    u8 green;
    u8 red;
};

struct Pixel_24bits
{
    u8 blue;
    u8 green;
    u8 red;
};

struct Pixel_16bits
{
    u8 upperword;
    u8 lowerword;
};

struct Pixel_8bits
{
    u8 pixel;
};

enum PIXEL_TYPE
{
    BITS_8 = 8,
    BITS_16 = 16,
    BITS_24 = 24,
    BITS_32 = 32,
};

struct Image
{
    i32 width;
    i32 height;
    enum PIXEL_TYPE ptype;
    union
    {
        struct Pixel_8bits *pixel8b;
        struct Pixel_16bits *pixel16b;
        struct Pixel_24bits *pixel24b;
        struct Pixel_32bits *pixel32b;
    };
};

enum DIB_COMPRESSION_METHOD
{
    BI_RGB
};

#endif