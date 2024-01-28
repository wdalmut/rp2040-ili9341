#ifndef __PNG_HELP__
#define __PNG_HELP__

#include <stdio.h>
#include <png.h>

#include "ff.h"

typedef struct Png {
    png_structp png_ptr;
    png_infop info_ptr;

    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type, num_palette;
    size_t rowbytes;
    png_colorp palette;
    png_byte channels;
} Png_t;

Png_t *w_png_open(FIL *);

png_bytep w_png_read_row(Png_t *);
void w_png_free_row(Png_t *, png_bytep);

void w_png_close(Png_t *);

#endif