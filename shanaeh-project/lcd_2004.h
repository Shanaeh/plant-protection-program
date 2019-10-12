#ifndef LCD_2004_H
#define LCD_2004_H

enum {
//GPIO to LCD mapping! Check lcd 2004 module (16 pins, 12 attached to pi)
    LCD_RS = 27,
    LCD_E  = 22,
    LCD_RW = 16,
    LED_ON = 17,
    
    //all in a row, L to R
    LCD_D0 = 26,
    LCD_D1 = 20,
    LCD_D2 = 13,
    LCD_D3 = 6,
    
    //all in a row, L to R
    LCD_D4 = 25,
    LCD_D5 = 24,
    LCD_D6 = 23,
    LCD_D7 = 18,
    
//Device constants
    LCD_WIDTH = 20, //Maximum characters per line
    E_DELAY = 500, //0.0005 second, or 500 microseconds
    CHAR_MODE = 1, //register 1, send to module to get into display mode
    INSTRUCT_MODE = 0, //register 0, send to module to get into instruct mode
    
//LCD line addresses
    LCD_LINE_1 = 0x00, //LCD RAM address for the 1st line, decimal 0
    LCD_LINE_2 = 0x28, //LCD RAM address for the 2nd line, decimal 40; also if decimal 60 (0x3C)
    LCD_LINE_3 = 0x14, //LCD RAM address for the 3rd line, decimal 20
    LCD_LINE_4 = 0x54, //LCD RAM address for the 4th line. decimal 84 (yes, it's odd)
};

//initializes the gpio pins (from the enum) and sets the display ready for char input (check .c for specifics, like cursor on and blink on)
void lcd_2004_init(void);


//Low-level, toggles the enable (only here for testing) necessary after each instruction/data transmission.
void toggle_enable(void);


//Writes 1 byte of information (char or instruction mode) with in-built toggle.
void lcd_write_byte(unsigned int bits, unsigned int mode);


//Writes 1 line of char data; will keep going through the display. Need to set the display address before it if you have a specific lcd location in mind.
void lcd_write_line(char* line);


//Sets address on lcd display (grid of 80 chars) for the line to start; use the LCD_LINE constants to start at first char of the line.
void lcd_set_display_address(unsigned int line_address);


//Clears the lcd screen and resets the cursor to 0x0.
void lcd_clear_screen();

#endif
