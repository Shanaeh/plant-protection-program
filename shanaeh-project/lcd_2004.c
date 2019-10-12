#include "printf.h"
#include "uart.h"
#include "gpio.h"
#include "timer.h"
#include "ftoa.h"
#include "strings.h"
#include "lcd_2004.h"

void toggle_enable() {
    timer_delay_us(E_DELAY);
    gpio_write(LCD_E, 1);
    timer_delay_us(E_DELAY);
    gpio_write(LCD_E, 0);
    timer_delay_us(E_DELAY);
}

void lcd_write_byte(unsigned int bits, unsigned int mode) { //bits = a hex code
    
    //mode = either char display or instruction register
    gpio_write(LCD_RS, mode);
    
    //Higher bits (endian) -- check chars on pg 17 of manual if char mode
    //D7 - D4
    //If there was a bit on this slot, then it'll be the same after &-ing it
    if((bits&0x80) == 0x80) gpio_write(LCD_D7, 1);
    else gpio_write(LCD_D7, 0); //0x80 = 1000 0000
    
    if((bits&0x40) == 0x40) gpio_write(LCD_D6, 1);
    else gpio_write(LCD_D6, 0); //0x40 = 0100 0000
    
    if((bits&0x20) == 0x20) gpio_write(LCD_D5, 1);
    else gpio_write(LCD_D5, 0); //0x20 = 0010 0000
    
    if((bits&0x10) == 0x10) gpio_write(LCD_D4, 1);
    else gpio_write(LCD_D4, 0); //0x10 = 0001 0000 and so on!
    
    
    //Lower bits (endian)
    //D3 - D0
    if((bits&0x08) == 0x08) gpio_write(LCD_D3, 1);
    else gpio_write(LCD_D3, 0); //0x08 = 0000 1000
    
    if((bits&0x04) == 0x04) gpio_write(LCD_D2, 1);
    else gpio_write(LCD_D2, 0); //0x04 = 0000 0100
    
    if((bits&0x02) == 0x02) gpio_write(LCD_D1, 1);
    else gpio_write(LCD_D1, 0); //0x02 = 0000 0010
    
    if((bits&0x01) == 0x01) gpio_write(LCD_D0, 1);
    else gpio_write(LCD_D0, 0); //0x01 = 0000 0001
    
    toggle_enable();
}

//User's responsibility not to overload line length (the display will keep outputting and overwriting former memory)
void lcd_write_line(char* line) {
    int line_len = strlen(line);
    for(int i=0; i<line_len; i++) {
        
        lcd_write_byte(*line, CHAR_MODE);
        line++;
    }
}

//Taking in the constant lcd_line_# (address) and OR-ing with 0x80. Layer of abstraction (for my sanity).
void lcd_set_display_address(unsigned int line_address) {
    //0x80 = 1000 0000, or 1 for D7 (which is a command to set the line address for the char display).
    line_address = 0x80 | line_address;
    lcd_write_byte(line_address, INSTRUCT_MODE);
}

void lcd_clear_screen() {
    lcd_write_byte(1, INSTRUCT_MODE);
}

//Initializes the gpio pins, mappings in the .h enum.
void lcd_gpio_init() {
    gpio_set_output(LCD_E);
    gpio_set_output(LCD_RS);
    gpio_set_output(LCD_RW);
    
    gpio_set_output(LCD_D0);
    gpio_set_output(LCD_D1);
    gpio_set_output(LCD_D2);
    gpio_set_output(LCD_D3);
    
    gpio_set_output(LCD_D4);
    gpio_set_output(LCD_D5);
    gpio_set_output(LCD_D6);
    gpio_set_output(LCD_D7);
    
    gpio_set_output(LED_ON);
}

//page 45 of the manual for specific instructions (many, many hours of drawing diagrams and coding in binary).
void lcd_display_init() {
    timer_delay_ms(45); //more than 40 ms
    
    //Function set: 8 bit, 2 lines, 5x7 char display
    //RS=0; D7-D0 = 0011 1000 (0x38)
    lcd_write_byte(0x38, INSTRUCT_MODE);
    
    //Function set: display on, cursor on, cursor blinking
    //RS=0; D7-D0 = 0000 1111 (0x0F)
    lcd_write_byte(0x0F, INSTRUCT_MODE);
    
    //Function set: entry mode, increment cursor position, no display shift (chars stay on same screen coordinates)
    //RS=0; D7-D0 = 0000 0110 (0x06)
    lcd_write_byte(0x06, INSTRUCT_MODE);
}

void lcd_2004_init(void) {
    lcd_gpio_init();
    lcd_display_init();
}

