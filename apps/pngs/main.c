#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hardware/dma.h"
#include "hardware/pio.h"
#include "ili9341.h"
#include "string.h"

#include "f_util.h"
#include "ff.h"
#include "pico/stdlib.h"
#include "rtc.h"

#include "hw_config.h"

#include "png-help.h"

int main() {
    stdio_init_all();

    ili9341_init();

    pio_sm_set_clkdiv(pio1, 0, 125000000/70000000);
    pio_sm_clkdiv_restart(pio1, 0);
    
    ili9341_fill_rect(0,0, TFT_WIDTH, TFT_HEIGHT, 0x0000);

    sleep_ms(3000);

    FATFS fs;
    FIL fil;

    FRESULT fr = f_mount(&fs, "/", 1);
    if (FR_OK != fr) panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);

    fr = f_open(&fil, "/display/240320.png", FA_READ);
    if (FR_OK != fr && FR_EXIST != fr) {
        panic("f_open(%s) error: %s (%d)\n", "/display/240320.png", FRESULT_str(fr), fr);
        return 1;
    }

    Png_t *p = w_png_open(&fil);

    uint16_t at_col = 0, at_row = 0;
    uint16_t maxCol = p->width > TFT_WIDTH ? TFT_WIDTH : p->width;
    uint16_t maxRow = p->height > TFT_HEIGHT ? TFT_HEIGHT : p->height;

    for (int i=0, row = at_row; i < maxRow; i++, row++) {
        png_bytep row_pointers = w_png_read_row(p);

        for (int j=0, col = 0; j < maxCol; col+=p->channels, j++) {
            png_byte red, green, blue;

            if (p->channels == 4) {
                png_bytep pixel = &row_pointers[col] + 3;
                if (*pixel == 0) {
                    continue;
                }
            }

            if ((p->color_type == PNG_COLOR_TYPE_PALETTE) && (p->palette != NULL)) {
                red = p->palette[row_pointers[col]].red;
                green = p->palette[row_pointers[col]].green;
                blue = p->palette[row_pointers[col]].blue;
            } else {
                // if the image is paletted but we don't have a palette, display as grayscale using palette index.
                png_bytep pixel = &row_pointers[col];
                red = *(pixel++);
                green = *(pixel++);
                blue = *(pixel++);
            }
            
            ili9341_draw_pixel(j + at_col, row, ili9341_color_565RGB(red, green, blue));
        }

        w_png_free_row(p, row_pointers);
    }

    while(true) {
        sleep_ms(1000);
    }

    f_unmount("/");
    return 0;
}