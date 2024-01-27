#ifndef  _ILI9431_H_
#define _ILI9431_H_

#define TFT_WIDTH   240
#define TFT_HEIGHT   320

#include "pico/stdlib.h"
#include "hardware/pio.h"

void ili9341_init();
void ili9431_init_config();
void ili9341_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
uint16_t ili9341_read_pixel(uint16_t x, uint16_t y);
void ili9341_set_address_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ili9341_memory_write_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ili9341_cmd(uint8_t cmd, uint32_t count, uint8_t dir, uint8_t *param);
uint16_t ili9341_color_565RGB(uint8_t R, uint8_t G, uint8_t B);
void ili9341_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void ili9341_invert_display(bool invert);
void ili9341_draw_line(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color);
void ili9341_draw_fill_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void ili9341_draw_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void ili9431_pio_cmd_init(PIO pio, uint sm, uint out_base,  uint set_base, uint32_t freq);
void ili9341_vertical_scroll_def(uint16_t top, uint16_t bottom);
void ili9341_vertical_scrolling(uint16_t addr);
void ili9341_read_area(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t * pixels);
void ili9341_write_area(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t * pixels);
#endif