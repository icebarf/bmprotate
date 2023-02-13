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
typedef int32_t i32;
typedef int16_t i16;

struct __attribute ((packed)) BITMAPFILE_HEADER
{
    u16 bf_id;
    u32 bf_fsize;
    u32 bf_reserved;
    u32 bf_pixels_offset;
};

struct __attribute ((packed)) BITMAPINFO_HEADER
{
    u32 dib_size;
    i32 dib_width;
    i32 dib_height;
    u16 dib_colorpanes;
    u16 dib_bpp;
    u32 dib_compression;
    u32 dib_imgsize;
    u32 dib_imghorres;
    u32 dib_imgvertres;
    u32 dib_ncolors;
    u32 dib_impcolors;
};

enum DIB_COMPRESSION_METHOD
{
    BI_RGB
};

#endif