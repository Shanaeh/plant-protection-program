#include "gpio.h"
#include "assert.h"

enum { //Addresses
    FSEL0 = 0x20200000, //GPIO Func Sel register 0, pin 0-9 (for commands)
    SET0 = 0x2020001C, //GPIO Set register, pin 0-31 (0 or 1 for input/output)
    CLR0 = 0x20200028, //GPIO Clear register, pin 0-31 (flag this, won't do X)
    LEV0 = 0x20200034, //GPIO read Set/Clear for pin 0-31
};
//NOTE TO SELF: when incremening for unsigned int pointer, automatically increments by 1 each time (so 4 bytes, don't need to multiply by 4)

void gpio_init(void) {
}

void gpio_set_function(unsigned int pin, unsigned int function) {
    if(pin < GPIO_PIN_FIRST || pin > GPIO_PIN_LAST) return; //just leave :)
    if(function < GPIO_FUNC_INPUT || function > GPIO_FUNC_ALT3) return;
    
    unsigned int pinTens = pin/10; //get the 10s digit of an integer
    volatile unsigned int* gpioAddressPtr = (unsigned int *)FSEL0 + (pinTens);
    
    unsigned int shiftLeft = (pin % 10) * 3;
    unsigned int tempMask = 7 << shiftLeft;
    tempMask = ~tempMask; //Now 111111... except for 000 in the pin slot
    
    unsigned int tempRegister = *gpioAddressPtr & tempMask; //AND keeps original values w/ 000 for pin slot
    tempMask = function << shiftLeft; //Now puts the 3-bit function to this pin slot
    *gpioAddressPtr = tempRegister | tempMask; //ORR to keep the 1s in either register
    //Saves directly.
}

unsigned int gpio_get_function(unsigned int pin) {
    if(pin < GPIO_PIN_FIRST || pin > GPIO_PIN_LAST) return GPIO_INVALID_REQUEST; //just leave :)
    unsigned int pinTens = pin/10;
    volatile unsigned int* gpioAddressPtr = (unsigned int *)FSEL0 + pinTens;
    
    volatile unsigned int currRegister = *gpioAddressPtr; //get values at this register w/out changing actual
    
    unsigned int shiftLeft = (pin % 10) * 3;
    unsigned int tempMask = 7 << shiftLeft; //0000000... except for 111 in the pin slot
    currRegister = currRegister & tempMask; //should only have values in the pin slot remaining
    currRegister = currRegister >> shiftLeft; //moving the number to the end to check
    
    return currRegister;
}

void gpio_set_input(unsigned int pin) {
    gpio_set_function(pin, GPIO_FUNC_INPUT);
}

void gpio_set_output(unsigned int pin) {
    gpio_set_function(pin, GPIO_FUNC_OUTPUT);
}

void gpio_write(unsigned int pin, unsigned int value) {
    if(pin < GPIO_PIN_FIRST || pin > GPIO_PIN_LAST) return; //just leave :)
    
    volatile unsigned int* gpioAddressPtr;
    if(value == 1) {
        gpioAddressPtr = (unsigned int *)SET0;
        if(pin > GPIO_PIN31) {
            gpioAddressPtr += 1;
        }
    } else if (value == 0) {
        gpioAddressPtr = (unsigned int *)CLR0;
        if(pin > GPIO_PIN31) {
            gpioAddressPtr += 1;
        }
    } else {
        return;
    }
    
    unsigned int pinShift = pin % 32;
    *gpioAddressPtr = 1 << pinShift;
}

unsigned int gpio_read(unsigned int pin) {
    if(pin < GPIO_PIN_FIRST || pin > GPIO_PIN_LAST) return GPIO_INVALID_REQUEST;
    
    volatile unsigned int* gpioAddressPtr;
    if(pin < GPIO_PIN32) {
        gpioAddressPtr = (unsigned int *)LEV0;
    } else if(pin > GPIO_PIN31 && pin <= GPIO_PIN_LAST) {
        gpioAddressPtr += 1;
    } else {
        return GPIO_INVALID_REQUEST;
    }
    unsigned int currRegister = *gpioAddressPtr; //get values at this register w/out changing actual
    unsigned int pinShift = pin % 32;
    unsigned int tempMask = 1 << pinShift;
    
    currRegister = currRegister & tempMask; //should only have values in the pin slot remaining
    currRegister = currRegister >> pinShift; //moving the number to the end to check
    return currRegister;
}
