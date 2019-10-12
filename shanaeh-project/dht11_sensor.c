#include "printf.h"
#include "uart.h"
#include "gpio.h"
#include "timer.h"
#include "spi.h"
#include "dht11_sensor.h"
#include "ftoa.h"

enum {
    DHT_PIN = GPIO_PIN21,
    LOW = 0,
    HIGH = 1,
};

void dht11_init() {
    
}

//Sensor description on how to use it (hence 18 and 40 weird constants): (https://www.mouser.com/ds/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf)
//dht11_data[0] and [1] are humidity (tens and tenths), dht11_data[2] and [3] is Celsius (tens and tenths), and dht11_data[4] is the checksum for making sure data is interpreted correctly.
unsigned int dht11_read(int dht11_data[5]) {
    int counter = 0; //to measure time intervals
    int bit_number = 0;
    int prev_state = HIGH; //1
    dht11_data[0] = dht11_data[1] = dht11_data[2] = dht11_data[3] = dht11_data[4] = 0; //for reading the temperature
    
    /*pull down 18 ms (milli)*/
    gpio_set_output(DHT_PIN); //configs for output
    gpio_write(DHT_PIN, LOW); //turns it low
    timer_delay_ms(18);
    
    /*pull up 40 us (micro)*/
    gpio_write(DHT_PIN, HIGH); //turns it high
    timer_delay_us(40);
    
    /*set to read data*/
    gpio_set_input(DHT_PIN); //configs for input
    
    
    /*something something about detecting changes/data like keyboard?*/
    for(int i=0; i<85; i++) {
        counter = 0;
        
        //A 0 or 1 bit depends on length of counter (microseconds passing)
        while(gpio_read(DHT_PIN) == prev_state) {
            counter++;
            timer_delay_us(1);
            if(counter == 255) break; //failsafe
        }
        prev_state = gpio_read(DHT_PIN);
        if(counter == 255) break; //failsafe
        
        //skip first 3 transitions b/c specifications...
        if((i >= 4) && (i%2 == 0)) { //skip every other pin
            
            dht11_data[bit_number / 8] <<= 1; //find the right 8-bit category
            if(counter > 16) {
                dht11_data[bit_number / 8] |= 1;
            }
            bit_number++;
        }
    }
    
    //Checksum to make sure bits interpreted correctly (last part of array)
    if ((bit_number >= 40) && (dht11_data[4] == ((dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]) & 0xFF))) {
        
    //NOTE: This printf logic is commented out since it'll go on the lcd display, but I'm leaving the code here so I can use this file on its own if so desired.
        //float celsiusDecimal = dht11_data[3]/100.0;
        //float f = (dht11_data[2] + celsiusDecimal) * 9.0 / 5.0 + 32;
        //char fahrenheit[8];
        //ftoa(f, fahrenheit, 2);
        
        //printf("Humidity = %d.%d | Temperature = %d.%d *C (%s *F)\n", dht11_data[0], dht11_data[1], dht11_data[2], dht11_data[3], fahrenheit);
        return 0;
    } else {
        //printf("Invalid data, skip\n");
        return 1;
    }
}
