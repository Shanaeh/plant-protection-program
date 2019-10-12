#include "printf.h"
#include "uart.h"
#include "gpio.h"
#include "timer.h"
#include "spi.h"
#include "strings.h"
#include "mcp3008.h"
#include "ftoa.h"
#include "dht11_sensor.h"
#include "water_sensor.h"
#include "soil_sensor.h"
#include "lcd_2004.h"

enum {
    LED_PIN = 12,
    LOW = 0,
    HIGH = 1,
    READ_SENSOR_DELAY = 6, //can be changed, currently set to every 6 seconds for demo purposes
    MINIMUM_MOISTURE = 20, //could change depending on plant type
};

//Could vary depending on plant.
unsigned int interpret_soil_sensor() {
    char moisture_buf[5]; //4 for __._ and null terminator.
    unsigned int toReturn = 0;

    float soil_moisture = soil_read(); //raw float moisture value
    ftoa(soil_moisture, moisture_buf, 1);
    
    //To check proper moisture for the plant.
    unsigned int soil_moisture_compare = MINIMUM_MOISTURE;
    if((unsigned int)soil_moisture < soil_moisture_compare) {
        toReturn = 1;
    }
    char soil_buf[LCD_WIDTH+1] = ""; //not outputting the null onto screen, so +1.
    snprintf(soil_buf, LCD_WIDTH+1, "Soil moisture: %s%c", moisture_buf, '%');
    
    lcd_set_display_address(LCD_LINE_3); //to start at beginning
    //lcd_write_line("Soil moist: ");
    lcd_write_line(soil_buf);
    
    return toReturn; //change to default 0 unless passes checkpoint
}

//Checks for a leak in the plant pot (or for general moisture). Returns 1 if leaking.
unsigned int interpret_water_sensor() {
    char* water_status = "";
    unsigned int toReturn = 0;
    unsigned int water_info = water_read();
    
    if(water_info < 90) {
        water_status = "normal";
    } else if(water_info >= 90) {
        toReturn = 1;
        water_status = "LEAK!!";
    }
    
    lcd_set_display_address(LCD_LINE_4);
    lcd_write_line("Water level: ");
    lcd_write_line(water_status);
    return toReturn; //change to default 0 unless passes checkpoint
}

//Just taking in the data and sending it to the lcd.
//dht11_data[0] and [1] are humidity (tens and tenths), dht11_data[2] and [3] is Celsius (tens and tenths), and dht11_data[4] is the checksum, checked in the .c file.
unsigned int interpret_dht11_sensor() {
    int dht11_data[5] = {0, 0, 0, 0, 0};
    unsigned int toReturn = dht11_read(dht11_data); //will be 1 if invalid data
    
    float celsiusDecimal = dht11_data[3]/100.0;
    float f = (dht11_data[2] + celsiusDecimal) * 9.0 / 5.0 + 32;
    char fahrenheit[8];
    ftoa(f, fahrenheit, 1);

    char temperature_buf[LCD_WIDTH+1] = ""; //not outputting the null onto screen, so +1.
    snprintf(temperature_buf, LCD_WIDTH+1, "Temperature: %s*F", fahrenheit);
    //Sends the temperature data to the lcd (using Fahrenheit over C)
    lcd_set_display_address(LCD_LINE_1);
    lcd_write_line(temperature_buf);

    char humidity_buf[LCD_WIDTH+1] = "";
    snprintf(humidity_buf, LCD_WIDTH+1, "Humidity: %d.%d%c", dht11_data[0], dht11_data[1], '%');
    lcd_set_display_address(LCD_LINE_2);
    lcd_write_line(humidity_buf);
    
    return toReturn; //if valid data, will return 0; else, return 1 and remind user to check if something is wrong.
}


//If read_interpret_sensors detects some issue, activate the led!
void activate_led(unsigned int status) {
    gpio_write(LED_PIN, status);
}

//I'm putting this here because different plants have different water needs.
void read_interpret_sensors() {
    unsigned int somethingWrong = 0;

    somethingWrong += interpret_dht11_sensor(); //e.g. if it can't get the temperature or humidity
    somethingWrong += interpret_soil_sensor(); //if moisture is too low
    somethingWrong += interpret_water_sensor(); //if there's a leak in the plant pot
  
    //if soil % is too low or water level is doing something weird
    if(somethingWrong != 0) {
        activate_led(1);
    } else {
        activate_led(0); //turns off the led
    }
}

void main(void) {
    uart_init();
    gpio_init();
    mcp3008_init();
    lcd_2004_init();
    gpio_set_output(LED_PIN);
    
    while(1) {
        lcd_clear_screen(); //resets screen
        
        read_interpret_sensors();
        timer_delay(READ_SENSOR_DELAY);
    }
}
