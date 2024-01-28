#include <stdio.h>
#include <stdlib.h>

#include "png-help.h"

#include <png.h>
#include "ff.h"

static void custom_read_data(png_structrp, png_bytep, size_t);

static void error(png_structp, const char *);

static void custom_read_data(png_structrp png_ptr, png_bytep data, size_t length) {
    UINT bytesRead;
    FIL *file = (FIL *)png_get_io_ptr(png_ptr);
    f_read(file, data, length, &bytesRead);
}

static void error(png_structp png_ptr, const char *message)
{
    printf("Error from libpng: %s\n", message);
}

Png_t *w_png_open(FIL *file)
{
    Png_t *png = malloc(sizeof(Png_t));

    printf("Creating read structure...\n");
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, error, NULL);

    if (png_ptr == NULL) {
        printf("png_create_read_struct error\n");
        free(png);
        return NULL;
    }

    printf("Allocating memory for image information...\n");
    png_infop info_ptr = png_create_info_struct(png_ptr);

    if (info_ptr == NULL) {
        printf("png_create_info_struct error\n");
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        free(png);
        return NULL;
    }

    png_set_palette_to_rgb(png_ptr);

    png->png_ptr = png_ptr;
    png->info_ptr = info_ptr;

    printf("Setting up the custom read function...\n");
    png_set_read_fn(png->png_ptr, file, custom_read_data);

    printf("Setting up LongJump...\n");

    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
        printf("LongJump set...\n");
    } else {
        printf("We got a LongJump, destroying read struct...\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(png);
        return NULL;
    }

    // The call to png_read_info() gives us all of the information from the
    // PNG file before the first IDAT (image data chunk). REQUIRED.
    printf("Reading info...\n");
    png_read_info(png_ptr, info_ptr);

    printf("Parsing image info...\n");
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
    printf("PNG info: width: %d, height: %d, bit_depth: %d, color_type: %d\n", width, height, bit_depth, color_type);
    
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        printf("Palette found...\n");
        png_get_PLTE(png_ptr, info_ptr, &(png->palette), &(png->num_palette));
        printf("num_palette: %d\n", png->num_palette);
    }

    png->channels = png_get_channels(png_ptr, info_ptr);
    printf("channels: %d\n", png->channels);

    png->bit_depth = bit_depth;
    png->color_type = color_type;
    png->width = width;
    png->height = height;
    png->rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    return png;
}

png_bytep w_png_read_row(Png_t *png)
{
    png_bytep row_pointers = (png_bytep)png_malloc(png->png_ptr, png->rowbytes);;
    png_read_rows(png->png_ptr, &row_pointers, NULL, 1);

    return row_pointers;
}

void w_png_free_row(Png_t *png, png_bytep row_pointers)
{
    png_free(png->png_ptr, row_pointers);
}

void w_png_close(Png_t *png)
{
    printf("Done! Destroying read struct...\n");
    png_destroy_read_struct(&(png->png_ptr), &(png->info_ptr), NULL);

    free(png);
}
