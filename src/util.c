#include "util.h"
#include <stdlib.h>
#include <sysexits.h>

int32_t
abs_32b (int32_t x)
{
    return x < 0 ? -x : x;
}

void
handle_code (enum RETCODES code, const char *msg, FILE *failed_file,
             bool check_errno)
{
    switch (code)
        {
        case SUCCESS:
            return;
        case ALLOC_FAILURE:
            fprintf (stderr, "%s: Out of memory\n", msg);
            exit (EX_OSERR);

        case INVALID_HEADER_FORMAT:
            fprintf (stderr, "%s: Invalid bitmap image header format\n", msg);
            exit (EX_DATAERR);

        case INVALID_COMPRESSION_FORMAT:
            fprintf (stderr,
                     "%s: Compressed image data is detected -- Unsupported as "
                     "of now\n",
                     msg);
            exit (EX_DATAERR);

        case INVALID_PIXEL_FORMAT:
            fprintf (stderr,
                     "%s: Pixel format using bits other than 8, 16, 24, 32 "
                     "detected -- Unsupported as of now\n",
                     msg);
            exit (EX_DATAERR);

        case OTHER_READ_FAILURE:
            if (check_errno)
                perror (msg);
            else
                fprintf (stderr, "%s: Reading Failure\n", msg);
            exit (EX_DATAERR);

        case FREAD_FAILURE:
            if (feof (failed_file))
                {
                    fprintf (stderr,
                             "%s: fread(): Unexpected end of file while "
                             "reading metadata\n",
                             msg);
                }
            else if (ferror (failed_file))
                {
                    fprintf (stderr,
                             "%s: fread(): Unexpected error found "
                             "in stream while reading metadata\n",
                             msg);
                }
            exit (EX_DATAERR);

        default:
            fprintf (stderr, "%s: Failure", msg);
            exit (EX_TEMPFAIL);
        }
}