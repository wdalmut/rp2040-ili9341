#include <stdio.h>
#include "stdlib.h"
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "ili9341.h"
#include "string.h"

void demo() {
    ili9341_fill_rect(0,0, TFT_WIDTH, TFT_HEIGHT, 0x0000);
    for (int j = 0; j < 320; j+=5) ili9341_draw_line(0, 0, 239, j, 0xffff);
    for (int i = 0; i < 240; i+=5) ili9341_draw_line(0, 0, i, 319, 0xffff);
    for (int i = 10;i < 150; i+=10) {
        ili9341_fill_rect(i, i, 100, 100, ili9341_color_565RGB(0x60+i, 0x70+i*2, 0x30+i));
    }
    // ili9341_draw_bitmap(50,0, &pico_img);
    ili9341_invert_display(true);
    ili9341_invert_display(false);
}
int main()
{
    stdio_init_all();

    sleep_ms(3000);
    
    ili9341_init();

    uint32_t freqs[4] = {1000000,2500000,  50000000, 70000000};
    for (int i = 0 ; i < 4; i++) {
        float div =125000000/freqs[i]; 
        //
        pio_sm_set_clkdiv(pio1, 0, div);
        pio_sm_clkdiv_restart(pio1, 0);
        
        ili9341_fill_rect(0,0, TFT_WIDTH, TFT_HEIGHT, 0x0000);
        absolute_time_t t1 = get_absolute_time();
        demo();
        absolute_time_t t2 = get_absolute_time();
        printf("Freq=%lu\n",freqs[i]);
        printf("%lld ms", absolute_time_diff_us(t1,t2)/1000);
        sleep_ms(5000);
    }

    while (true)
    {
        sleep_ms(1000);
    }
}
