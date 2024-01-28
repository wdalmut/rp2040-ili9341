#include "stdio.h"
#include "stdlib.h"
#include "ili9341.pio.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "string.h"
#include "registers.h"
#include "ili9341.h"


PIO ili9341_pio = pio1;
uint ili9341_sm=0;
uint in_out_base_pin=4;
uint set_base_pin=12;

void ili9341_cmd(uint8_t cmd, uint32_t count, uint8_t dir, uint8_t *param) {/*dir:write:0xFF, read:0x00*/
    if (dir != 0xFF && dir != 0x00) return;
    //pio_sm_restart(ili9341_pio,ili9341_sm);
    pio_sm_put_blocking(ili9341_pio, ili9341_sm, cmd); // command code
    pio_sm_put_blocking(ili9341_pio, ili9341_sm, count); // how many parameters
    if (count != 0) {
        pio_sm_put_blocking(ili9341_pio, ili9341_sm, dir); // write out or read in
        if (dir == 0xFF) {
            for (int i = 0; i < count; i++) {
                pio_sm_put_blocking(ili9341_pio, ili9341_sm, param[i]); // write out parameters
            }
        } else {
            for (int i = 0; i < count; i++) {
                param[i] = (uint8_t)(pio_sm_get_blocking(ili9341_pio, ili9341_sm)); // read in data
            }
        }
    }
}

/* ================  */
void pio_irq_read_data() {
     if (pio_interrupt_get(ili9341_pio, ili9341_sm)) {
        pio_interrupt_clear(ili9341_pio, ili9341_sm);
     }
}

void ili9431_pio_cmd_init(PIO pio, uint sm, uint in_out_base,  uint set_base, uint32_t freq) {
    uint offset=0;
    pio_sm_config c;
    offset = pio_add_program(pio, &ili9341_pio_cmd_program);
    c = ili9341_pio_cmd_program_get_default_config(offset);
    
    for (int i=0; i < 8; i++) pio_gpio_init(pio, in_out_base+i);
    for (int i=0; i < 4; i++) pio_gpio_init(pio, set_base+i);
    
    pio_sm_set_consecutive_pindirs(pio, sm, in_out_base, 8, true);
    pio_sm_set_consecutive_pindirs(pio, sm, set_base, 4, true);
    
    sm_config_set_in_pins(&c, in_out_base);
    sm_config_set_out_pins(&c, in_out_base, 8);
    sm_config_set_set_pins(&c, set_base, 4);
   
    sm_config_set_out_shift(&c, true, false, 8);
    sm_config_set_in_shift(&c, false, false, 8);

    float div = clock_get_hz(clk_sys)/freq;
    sm_config_set_clkdiv(&c, div);

    //uint pio_irq = pio_get_index(pio)? PIO1_IRQ_0:PIO0_IRQ_0;
    //pio_set_irq0_source_enabled(pio, pis_interrupt0, true);
    //irq_add_shared_handler(pio_irq, pio_irq_read_data, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
    //irq_set_enabled(pio_irq, true);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

/* ili3941 draw functions*/
uint16_t ili9341_color_565RGB(uint8_t R, uint8_t G, uint8_t B) {
    uint16_t c;
    c = (((uint16_t)B)>>3)<<11 | (((uint16_t)G)>>2) << 5 | ((uint16_t)R)>>3;
    return c;
}
void ili9341_memory_write_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint8_t addr[4];
    addr[0]=(uint8_t)(x1 >> 8);
    addr[1]= (uint8_t)(x1 & 0xff);
    addr[2]= (uint8_t)(x2 >> 8);
    addr[3]= (uint8_t)(x2 & 0xff);
    ili9341_cmd(ILI9341_COLADDRSET, 4,  0xFF, addr);

    addr[0]=(uint8_t)(y1 >> 8);
    addr[1]= (uint8_t)(y1 & 0xff);
    addr[2]= (uint8_t)(y2 >> 8);
    addr[3]= (uint8_t)(y2 & 0xff);
	ili9341_cmd(ILI9341_PAGEADDRSET, 4,  0xFF, addr );

    ili9341_cmd(ILI9341_MEMORYWRITE, 0, 0xFF, NULL);
}

void ili9341_set_address_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint8_t addr[4];
    addr[0]=(uint8_t)(x1 >> 8);
    addr[1]= (uint8_t)(x1 & 0xff);
    addr[2]= (uint8_t)(x2 >> 8);
    addr[3]= (uint8_t)(x2 & 0xff);
    ili9341_cmd(ILI9341_COLADDRSET, 4,  0xFF, addr);

    addr[0]=(uint8_t)(y1 >> 8);
    addr[1]= (uint8_t)(y1 & 0xff);
    addr[2]= (uint8_t)(y2 >> 8);
    addr[3]= (uint8_t)(y2 & 0xff);
	ili9341_cmd(ILI9341_PAGEADDRSET, 4,  0xFF, addr );
}

/* put color at point*/
void ili9341_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
  if ( x < 0 || x > TFT_WIDTH-1 || y < 0 || y > TFT_HEIGHT-1) return;
	ili9341_set_address_window(x,y,x,y);
    ili9341_cmd(ILI9341_MEMORYWRITE, 2, 0xFF, (uint8_t[2]){(uint8_t)(color >> 8), (uint8_t)color});
}
/* read a RGB565 pixel from tft device
read 4 bytes:
0: dummy
1: Red byte
2: Green byte
3: Blue byte
*/
uint16_t ili9341_read_pixel(uint16_t x, uint16_t y)
{
    if ( x < 0 || x > TFT_WIDTH-1 || y < 0 || y > TFT_HEIGHT-1) return 0;
	uint8_t buff[4];
 
    ili9341_set_address_window(x, y, x,y);
    ili9341_cmd(ILI9341_MEMORYREAD, 4, 0x00, buff);
    return ((((uint16_t)buff[1])>>3) << 11 | (((uint16_t)buff[2])>>2) << 5 | (((uint16_t)buff[3])>>3));
}
void ili9341_invert_display(bool invert) {
  if (invert) 
    ili9341_cmd(ILI9341_INVERTON, 0, 0xFF, NULL);
  else 
    ili9341_cmd(ILI9341_INVERTOFF, 0, 0xFF, NULL);
}
void ili9341_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
  if (x < 0) x=0;
  if (y < 0) y=0;
  if (x+width > TFT_WIDTH) width = TFT_WIDTH-x;
  if (y+height > TFT_HEIGHT) height = TFT_HEIGHT-y;
  ili9341_memory_write_window(x,y, x+width-1, y+height-1);
  for (int j = y; j < y+height; j++) 
    for (int i = x; i< x+width;i++)
      ili9341_cmd(ILI9341_NOP, 2, 0xFF, (uint8_t[2]){(uint8_t)(color >> 8), (uint8_t)(color&0xff)});
}

void ili9341_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	/* algorithm from https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm */
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;
    int e2;
    while(1) {
        //plot(x0, y0)
        ili9341_draw_pixel(x0, y0, color);
        if ( x0 == x1 && y0 == y1 ) break;
        e2 = 2 * error;
        if (e2 >= dy) {  
            if (x0 == x1) break;
            error = error + dy;
            x0 = x0 + sx;
        }
        if (e2 <= dx) { 
            if (y0 == y1) break;
            error = error + dx;
            y0 = y0 + sy;
        }
    }
}

void ili9341_draw_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
	int x;
	int y;
	int error;
	int old_error;

	x=0;
	y=-r;
	error=2-2*r;
	do{
		ili9341_draw_pixel(x0-x,  y0+y, color);
		ili9341_draw_pixel(x0-y,  y0-x, color);
		ili9341_draw_pixel(x0+x,  y0-y, color);
		ili9341_draw_pixel(x0+y,  y0+x, color);
		if ((old_error=error)<=x)	error+=++x*2+1;
		if (old_error>y || error>x) error+=++y*2+1;	 
	} while(y<0);
}

// Draw circle of filling
// x0:Central X coordinate
// y0:Central Y coordinate
// r:radius
// color:color
void ili9341_draw_fill_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
	int x;
	int y;
	int error;
	int old_error;
	int ChangeX;

	x=0;
	y=-r;
	error=2-2*r;
	ChangeX=1;
	do{
		if(ChangeX) {
			ili9341_draw_line(x0-x, y0-y,x0-x, y0+y, color);
			ili9341_draw_line(x0+x, y0-y, x0+x, y0+y, color);
		} // endif
		ChangeX=(old_error=error)<=x;
		if (ChangeX)			error+=++x*2+1;
		if (old_error>y || error>x) error+=++y*2+1;
	} while(y<=0);
} 

void ili9341_vertical_scroll_def(uint16_t top, uint16_t bottom) {
    uint16_t scroll=TFT_HEIGHT - top - bottom;
    ili9341_cmd(ILI9341_VSCROLLDEF, 6, 0xFF, (uint8_t[6]){(uint8_t)(top>>8),(uint8_t)(top&0xff),
    (uint8_t)(scroll>>8),(uint8_t)(scroll&0xff),(uint8_t)(bottom>>8),(uint8_t)(bottom&0xff)});
}
void ili9341_vertical_scrolling(uint16_t addr) {
    ili9341_cmd(ILI9341_VSCROLLADDR, 2, 0xFF, (uint8_t[2]){(uint8_t)(addr>>8),(uint8_t)(addr&0xff),});
}

void ili9341_read_area(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t * pixels) {
    if (x < 0 || y < 0 || x >= TFT_WIDTH || y >= TFT_HEIGHT) return;
    if (x+width > TFT_WIDTH) width = TFT_WIDTH - x;
    if (y+height > TFT_HEIGHT) height = TFT_HEIGHT - y;
    uint32_t total_pixels = width*height;
    uint8_t tmpbuf[4];
    ili9341_set_address_window(x,y,x+width-1, y+height-1);
    ili9341_cmd(ILI9341_MEMORYREAD, 1, 0x00, tmpbuf);
    for (int i=0; i < total_pixels; i++) {
        ili9341_cmd(ILI9341_NOP, 3, 0x00, tmpbuf);
        pixels[i*2] = (tmpbuf[0]& 0xf8) | (tmpbuf[1] >> 5);
        pixels[i*2+1] = ((tmpbuf[1] << 3) & 0xe0) | (tmpbuf[2] >> 3);
    }
    

}
void ili9341_write_area(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t * pixels) {
    if (x < 0 || y < 0 || x >= TFT_WIDTH || y >= TFT_HEIGHT) return;
    if (x+width > TFT_WIDTH) width = TFT_WIDTH - x;
    if (y+height > TFT_HEIGHT) height = TFT_HEIGHT - y;
    uint32_t total_bytes = width*height*2;
    ili9341_set_address_window(x,y,x+width-1, y+height-1);
    ili9341_cmd(ILI9341_MEMORYWRITE, total_bytes, 0xFF, pixels);

}

void ili9431_init_config() {
	ili9341_cmd(ILI9341_SOFTRESET, 0, 0xFF, NULL);
    sleep_ms(150);
    ili9341_cmd(ILI9341_DISPLAYOFF, 0, 0xFF, NULL);
    sleep_ms(150);
    ili9341_cmd(ILI9341_PIXELFORMAT, 1, 0xFF, (uint8_t[1]){0x55}); //0x55
    ili9341_cmd(ILI9341_POWERCONTROL1, 1, 0xFF, (uint8_t[1]){0x05}); // 0x05 :3.3V
    ili9341_cmd(ILI9341_POWERCONTROL2, 1, 0xFF, (uint8_t[1]){0x10});
    ili9341_cmd(ILI9341_VCOMCONTROL1, 2, 0xFF, (uint8_t[2]){0x3E, 0x28});
    ili9341_cmd(ILI9341_VCOMCONTROL2, 1, 0xFF, (uint8_t[1]){0x86});
    ili9341_cmd(ILI9341_MADCTL, 1, 0xFF, (uint8_t[1]){0x00}); //MY,MX,MV,ML,BRG,MH,0,0(40)
    ili9341_cmd(ILI9341_FRAMECONTROL, 2, 0xFF, (uint8_t[2]){0x00, 0x1B}); // Default 70Hz
    ili9341_cmd(ILI9341_DISPLAYFUNC, 4, 0xFF, (uint8_t[4]){0x0A, 0x82, 0x27, 0x04}); //0a,a2,27,04
    ili9341_cmd(ILI9341_GAMMASET, 1, 0xFF, (uint8_t[1]){0x01});
  
  	//ili9341_cmd(ILI9341_PGAMCOR, 15, 0xFF, (uint8_t[15]){ 0x0f, 0x31, 0x2b, 0x0c, 0x0e, 0x08, 0x4e, 0xf1, 0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00 });
    //ili9341_cmd(ILI9341_NGAMCOR,  15,0xFF, (uint8_t[15]){ 0x00, 0x0e, 0x14, 0x03, 0x11, 0x07, 0x31, 0xc1, 0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0f }); 
    
    ili9341_cmd(ILI9341_SLEEPOUT, 0, 0xFF, NULL);
    sleep_ms(150);
    ili9341_cmd(ILI9341_DISPLAYON, 0, 0xFF, NULL);
    sleep_ms(500);
    
}
void ili9341_init() {
    
    ili9431_pio_cmd_init(ili9341_pio, ili9341_sm, in_out_base_pin, set_base_pin, 35000000); 
	ili9431_init_config();
    
}