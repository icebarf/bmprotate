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
    fprintf (stdout, "Bits: %d\n", metadata->bi_bitcount);
    if (metadata->bi_compression != BI_RGB)
        return INVALID_COMPRESSION_FORMAT;
    return SUCCESS;
}

__attribute ((warn_unused_result)) i32
row_size (double img_width, enum PIXEL_TYPE bitcnt)
{

    double roof = ceil (img_width * (float)bitcnt / 32.0);
    return (i32)roof * 4;
}

#define fread_img_data_generator(BITCNT, BYTECNT)                             \
    {                                                                         \
        unsigned long size                                                    \
            = (unsigned long)(metadata->bi_width * metadata->bi_height);      \
        img.pixel##BITCNT##b = calloc (size, sizeof (*img.pixel##BITCNT##b)); \
        if (img.pixel##BITCNT##b == NULL)                                     \
            return ALLOC_FAILURE;                                             \
        unsigned long value = 0;                                              \
        if ((value = fread (img.pixel##BITCNT##b, BYTECNT, size, bitmap))     \
            != size)                                                          \
            {                                                                 \
                fprintf (stdout, "ftell(): %ld\nfread_ret = %ld\n",           \
                         ftell (bitmap), value);                              \
                return FREAD_FAILURE;                                         \
            }                                                                 \
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

void
write_back (struct BITMAP_HEADER *metadata, struct Image *img)
{
    FILE *out = fopen ("out.bmp", "wb");
    fwrite (metadata, 1, sizeof (struct BITMAP_HEADER), out);
    switch (img->ptype)
        {
        case BITS_8:
            fwrite (img->pixel24b, 1,
                    (unsigned long)(metadata->bi_height * metadata->bi_width),
                    out);
            break;
        case BITS_16:
            fwrite (img->pixel24b, 2,
                    (unsigned long)(metadata->bi_height * metadata->bi_width),
                    out);
            break;
        case BITS_24:
            fwrite (img->pixel24b, 3,
                    (unsigned long)(metadata->bi_height * metadata->bi_width),
                    out);
            break;
        case BITS_32:
            fwrite (img->pixel24b, 4,
                    (unsigned long)(metadata->bi_height * metadata->bi_width),
                    out);
            break;
        }
    fclose (out);
}

struct Image
rotate (struct Image img)
{
    struct Image out;
    out.height = img.width;
    out.width = img.height;
    out.ptype = img.ptype;
    switch (out.ptype)
        {
        case BITS_8:
            out.pixel8b = calloc ((unsigned long)(out.width * out.height), 1);
            break;
        case BITS_16:
            out.pixel16b = calloc ((unsigned long)(out.width * out.height), 2);
            break;
        case BITS_24:
            out.pixel24b = calloc ((unsigned long)(out.width * out.height), 3);
            break;
        case BITS_32:
            out.pixel32b = calloc ((unsigned long)(out.width * out.height), 4);
            break;
        }

    for (u32 i = 0; i < out.height; i++)
        for (u32 j = 0; j < out.width; j++)
            {
                switch (out.ptype)
                    {
                    case BITS_8:
                        out.pixel8b[(i * out.width) + j]
                            = img.pixel8b[(j * img.width) + i];
                        break;
                    case BITS_16:
                        out.pixel16b[(i * out.width) + j]
                            = img.pixel16b[(j * img.width) + i];
                        break;
                    case BITS_24:
                        out.pixel24b[(i * out.width) + j]
                            = img.pixel24b[(j * img.width) + i];
                        break;
                    case BITS_32:
                        out.pixel32b[(i * out.width) + j]
                            = img.pixel32b[(j * img.width) + i];
                        break;
                    }
            }

    return out;
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

    struct Image rotated = rotate (image);

    // metadata swapup
    bitmap_metadata->bi_width = rotated.width;
    bitmap_metadata->bi_height = rotated.height;
    i32 temp = bitmap_metadata->bi_ypixels_permeter;
    bitmap_metadata->bi_ypixels_permeter
        = bitmap_metadata->bi_xpixels_permeter;
    bitmap_metadata->bi_xpixels_permeter = temp;
    write_back (bitmap_metadata, &rotated);
    fclose (bitmap);
    free (bitmap_metadata);
    return 0;
}
