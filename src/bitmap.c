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

#include "bitmap.h"
#include "benchmark.h"
#include "util.h"
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

__attribute ((warn_unused_result)) FILE *
open_img (const char *path)
{
    return fopen (path, "rb");
}

__attribute ((warn_unused_result)) enum RETCODES
get_bitmap_metadata (FILE *bitmap, struct BITMAP_HEADER **outdata)
{
    struct BITMAP_HEADER *data = calloc (1, sizeof (*data));
    if (data == NULL)
        return ALLOC_FAILURE;

    const size_t count = sizeof (*data);

    if (fread (data, 1, count, bitmap) != count)
        return FREAD_FAILURE;

    if (fseek (bitmap, 0, SEEK_SET) != 0)
        return OTHER_READ_FAILURE;

    if (data->bf_id != 0x4d42)
        return INVALID_HEADER_FORMAT;

    *outdata = data;
    return SUCCESS;
}

__attribute ((warn_unused_result)) enum RETCODES
perform_metadata_checks (struct BITMAP_HEADER *metadata)
{
    fprintf (stdout, "DIB Header Size : %" PRId32 "\n", metadata->bi_size);
    fprintf (stdout, "Resolution:\nWidth: %" PRId32 "\nHeight: %" PRId32 "\n",
             metadata->bi_width, metadata->bi_height);
    if (metadata->bi_compression != BI_RGB)
        return INVALID_COMPRESSION_FORMAT;
    return SUCCESS;
}

__attribute ((warn_unused_result)) i32
row_size (double img_width, enum PIXEL_TYPE bitcnt)
{
    return ceil ((img_width * bitcnt) / 32.0) * 4;
}

#define fread_img_data_generator(BITCNT, BYTECNT)                             \
    img.pixel##BITCNT##b = calloc (img.width * abs_32b (metadata->bi_height), \
                                   sizeof (*img.pixel##BITCNT##b));           \
    if (img.pixel##BITCNT##b == NULL)                                         \
        return ALLOC_FAILURE;                                                 \
    for (int32_t i = 0; i < img.height; i++)                                  \
        {                                                                     \
            if (fread (img.pixel##BITCNT##b, 1, img.width * BYTECNT, bitmap)  \
                != img.width * BYTECNT)                                       \
                return FREAD_FAILURE;                                         \
                                                                              \
            int32_t off = row_size (img.width, BITCNT) - img.width;           \
            if (off > 0)                                                      \
                if (fseek (bitmap, off, SEEK_CUR) != 0)                       \
                    return OTHER_READ_FAILURE;                                \
        }

__attribute ((warn_unused_result)) enum RETCODES
load_pixel_data (struct Image *outimg, struct BITMAP_HEADER *metadata,
                 FILE *bitmap)
{
    struct Image img = { .width = metadata->bi_width,
                         .height = metadata->bi_height,
                         .ptype = metadata->bi_bitcount };

    if (fseek (bitmap, metadata->bf_pixels_offset, SEEK_SET) != 0)
        return OTHER_READ_FAILURE;

    switch (metadata->bi_bitcount)
        {
        case BITS_32:
            fread_img_data_generator (32, 4);
            break;

        case BITS_24:
            fread_img_data_generator (24, 3);
            break;

        case BITS_16:
            fread_img_data_generator (16, 2);
            break;

        case BITS_8:
            fread_img_data_generator (8, 1);
        }

    *outimg = img;
    return SUCCESS;
}

int
main (int argc, char *argv[])
{
    if (argc != 2)
        {
            fprintf (stderr,
                     "%s: Invalid Usage\n"
                     "Usage: %s file.bmp\n",
                     argv[0], argv[0]);
            exit (EX_USAGE);
        }

    FILE *bitmap = open_img (argv[1]);
    if (bitmap == NULL)
        handle_code (OTHER_READ_FAILURE, "open_img()", bitmap, true);

    struct BITMAP_HEADER *bitmap_metadata = NULL;
    enum RETCODES code = get_bitmap_metadata (bitmap, &bitmap_metadata);
    switch (code)
        {
        case SUCCESS:
            break;
        case OTHER_READ_FAILURE:
            handle_code (code, "get_bitmap_metadata(): fseek()", bitmap, true);
            break;
        case FREAD_FAILURE:
            handle_code (code, "get_bitmap_metadata()", bitmap, false);
            break;
        default:
            handle_code (code, "get_bitmap_metadata()", NULL, false);
        }

    code = perform_metadata_checks (bitmap_metadata);
    handle_code (code, "bitmap_metadata()", NULL, false);

    struct Image image;
    code = load_pixel_data (&image, bitmap_metadata, bitmap);
    switch (code)
        {
        case SUCCESS:
            break;
        case OTHER_READ_FAILURE:
            handle_code (code, "load_pixel_data(): fseek()", bitmap, true);
            break;
        case FREAD_FAILURE:
            handle_code (code, "load_pixel_data()", bitmap, false);
            break;
        default:
            handle_code (code, "load_pixel_data()", NULL, false);
        }

    fclose (bitmap);
    free (bitmap_metadata);
    return 0;
}