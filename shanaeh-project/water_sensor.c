#include "printf.h"
#include "uart.h"
#include "gpio.h"
#include "timer.h"
#include "spi.h"
#include "mcp3008.h"
#include "water_sensor.h"

void water_init(void) {
    
}

//Measures the water level to check for rain... mcp3008 from lecture code; however, I had an interesting 3.5 hours trying to create my own using spi (thanks Jennifer and Chris)!
unsigned int water_read(void) {
    unsigned int water_info = mcp3008_read(0); //raw data
    //printf("Water level: %d\n", water_info);
    return water_info;
}
