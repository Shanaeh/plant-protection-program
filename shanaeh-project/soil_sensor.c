#include "printf.h"
#include "uart.h"
#include "gpio.h"
#include "timer.h"
#include "spi.h"
#include "mcp3008.h"
#include "soil_sensor.h"
#include "ftoa.h"

void soil_init(void) {
    
}

//Reads soil sensor, analog: lower than 300 when wet, baseline around 600-850, 1023 (above 900?) when nothing is touching it, as if dry
//Use datasheet to find moisture percentage. Commented out code is if you want to use soil_sensor separately.
float soil_read() {
    //char moisture_buf[10];
    
    unsigned int soil_analog = mcp3008_read(1); //raw data, channel 1
    float moisture_percentage = 100 - ((soil_analog/1023.00) * 100);
    //ftoa(moisture_percentage, moisture_buf, 2);
    
    //printf("Soil sensor: %s\n", moisture_buf);
    return moisture_percentage;
}

