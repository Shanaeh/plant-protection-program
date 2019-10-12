#include "gpio.h"
#include "gpioextra.h"
#include "keyboard.h"
#include "ps2.h"
#include "printf.h"

const unsigned int CLK  = GPIO_PIN23;
const unsigned int DATA = GPIO_PIN24;
static keyboard_modifiers_t currModifiers = 0x0;
static int scrollLockOn = 0;
static int numLockOn = 0;
static int capsLockOn = 0;

void wait_for_falling_clock_edge() {
    while (gpio_read(CLK) == 0) {}
    while (gpio_read(CLK) == 1) {}
}

void keyboard_init(void) 
{
    gpio_set_input(CLK); 
    gpio_set_pullup(CLK); 
 
    gpio_set_input(DATA); 
    gpio_set_pullup(DATA); 
}

/*
 * `keyboard_read_scancode`: Bottom level keyboard interface.
 *
 * Read (blocking) a single scan code byte from the PS/2 keyboard.
 * Bits are read on the falling edge of the clock.
 *
 * Tries to read 11 bits: 1 start bit, 8 data bits, 1 parity bit, and 1 stop bit
 *
 * Restarts the scan code if:
 *   (lab5) The start bit is incorrect
 *   (assign5) or if parity or stop bit is incorrect
 *
 * Returns the 8 data bits of a well-formed PS/2 scan code.
 * Will not return until it reads a valid scan code.
 */
unsigned char keyboard_read_scancode(void)
{
    unsigned char byte;
    //Do not leave loop unless usable byte; will restart as necessary.
    while(true) {
        byte = 0x00000000;
        int numOfOnes = 0; //to check parity
        wait_for_falling_clock_edge(); //go from 0 to 1
        
        //wait for start bit
        unsigned char startBit = gpio_read(DATA);
        if (startBit == 0) {
            //reading from right to left (the first bit sent is least significant)
            for(int i=0; i<8; i++) {
                wait_for_falling_clock_edge();
                unsigned char dataBit = gpio_read(DATA);
                if(dataBit == 1) numOfOnes++;
                    
                dataBit = dataBit << i; //just shift by the counter so can OR it
                byte = byte | dataBit;
            }
        }
        //parity bit checks for even/odd after 8-bit transmission, adjust to make it odd (if odd # of bits, parity bit will be 0, if even # of bits, will be 1)
        wait_for_falling_clock_edge();
        unsigned char parityBit = gpio_read(DATA);
        numOfOnes += parityBit;
        int parityCheck = numOfOnes % 2; //should be odd, so = 1.
        //if(parityCheck != 1) continue;
        
        wait_for_falling_clock_edge();
        unsigned char stopBit = gpio_read(DATA);
        
        //if byte is valid with parity and stopBit, break loop; otherwise restart.
        if(parityCheck == 1 && stopBit == 1) break;
    }
    return byte;
}
//TO IMPLEMENT: keyboard time-out in scancode (check falling clock edge for a restart if too much time in between)
//Keep track of start time, and check in while loop to make sure not stuck too long --> restart

/*
 * `keyboard_read_sequence`: Low level keyboard interface.
 *
 * Reads the sequence of scan codes corresponding to next key action.
 * A key action is either a key down or key up. A sequence can
 * 1, 2, or 3 bytes, as follows:
 *    1 byte:  simple key down
 *    2 bytes: simple key up or extended key down
 *    3 bytes: extended key up
 * The function stores the scan code bytes into the `seq` array and
 * returns the number of bytes written to the array. This function
 * calls `keyboard_read_scancode` to read each scan code.
 */
int keyboard_read_sequence(unsigned char seq[])
{
    int elemIndex = 0;
    while(true) {
        seq[elemIndex] = keyboard_read_scancode();
        
        if(seq[elemIndex] != PS2_CODE_RELEASE && seq[elemIndex] != PS2_CODE_EXTEND) {
            elemIndex++;
            break;
        }
        elemIndex++;
    }
    return elemIndex; //technically now the # of elements because incremented before returning!
}

//Sets the global variables for scroll, num, and caps lock.
//BUG: how to get the right global variable for the function -- going to OH tomorrow.
//void event_helper_lock(keyboard_action_t action, keyboard_modifiers_t newModifier)
//{
//
//    if(action == KEYBOARD_ACTION_DOWN) {
//        if(modLockOn == 0) {
//            currModifiers = currModifiers | newModifier;
//        } else if (modLockOn == 1){
//            currModifiers = currModifiers ^ newModifier;
//            modLockOn = 0;
//        }
//    } else if(action == KEYBOARD_ACTION_UP) {
//        modLockOn = 1;
//    }
//}

/*
 * `keyboard_read_event`: Mid level keyboard interface.
 *
 * The function reads (blocking) the next key action from the keyboard.
 * Returns a `key_event_t` struct that represents the key event.  The
 * struct includes the sequence of raw scan codes, the action (up or down),
 * and the state of the modifier flags in effect. If this event is a
 * a key down or up for a modifier key (CTRL, ALT, SHIFT, etc.), the
 * modifiers field in the event contains the state of the modifiers
 * after having incorporated this key action.
 * This function calls `keyboard_read_sequence`.
 */
/*
 typedef struct {
 unsigned char seq[3];     // sequence of raw scan code bytes
 int seq_len;              // number of bytes in sequence
 ps2_key_t key;            // entry taken from ps2_keys table (see ps2.h)
 keyboard_action_t action; // either KEYBOARD_ACTION_UP or KEYBOARD_ACTION_DOWN
 keyboard_modifiers_t modifiers;   // modifiers in effect, composed of above bit flags
 } key_event_t;
 */
key_event_t keyboard_read_event(void) 
{
    key_event_t event;
    event.seq_len = keyboard_read_sequence(event.seq); //stores data in event.seq and size of array in seq_len
    event.action = KEYBOARD_ACTION_DOWN;

    //Saves raw key value and converts to the ps2_key table.
    for(int i = 0; i < event.seq_len; i++) {
        //If f0 at any point in this sequence, key is being released up
        if(event.seq[i] == PS2_CODE_RELEASE) event.action = KEYBOARD_ACTION_UP;
        else if(event.seq[i] == PS2_CODE_EXTEND) continue; //If E0 at i=0, then for sure an extra key, but don't want to keep that, just need that sweet, sweet scan code (at least for basic), though technically would be overwritten even if you let it progress to the else case.
        
        //Still need to put in the key type! Will go to this condition at last index, AKA  event.seq[event.seq_len -1], saves as ps2_key_t struct
        else event.key = ps2_keys[event.seq[i]];
    }
    //Checks for modifier value (as per ps2.h file, the mod values are greater than 0x90)
    keyboard_modifiers_t newModifier = 0;
    if(event.key.ch >= 0x90) {
        if(event.key.ch == PS2_KEY_SHIFT) {
            newModifier = KEYBOARD_MOD_SHIFT;
        }
        else if(event.key.ch == PS2_KEY_ALT) {
            newModifier = KEYBOARD_MOD_ALT;
        }
        else if(event.key.ch == PS2_KEY_CTRL) {
            newModifier = KEYBOARD_MOD_CTRL;
        }
        else if(event.key.ch == PS2_KEY_SCROLL_LOCK) {
            newModifier = KEYBOARD_MOD_SCROLL_LOCK;
            
            if(event.action == KEYBOARD_ACTION_DOWN) {
                if(scrollLockOn == 0) {
                    currModifiers = currModifiers | newModifier;
                } else if (scrollLockOn == 1){
                    currModifiers = currModifiers ^ newModifier;
                    scrollLockOn = 0;
                }
            } else if(event.action == KEYBOARD_ACTION_UP) {
                scrollLockOn = 1;
            }
        }
        else if(event.key.ch == PS2_KEY_NUM_LOCK) {
            newModifier = KEYBOARD_MOD_NUM_LOCK;
            
            if(event.action == KEYBOARD_ACTION_DOWN) {
                if(numLockOn == 0) {
                    currModifiers = currModifiers | newModifier;
                } else if (numLockOn == 1){
                    currModifiers = currModifiers ^ newModifier;
                    numLockOn = 0;
                }
            } else if(event.action == KEYBOARD_ACTION_UP) {
                numLockOn = 1;
            }
        }
        else if(event.key.ch == PS2_KEY_CAPS_LOCK) {
            newModifier = KEYBOARD_MOD_CAPS_LOCK;
            //event_helper_lock(event.action, KEYBOARD_MOD_CAPS_LOCK);

            if(event.action == KEYBOARD_ACTION_DOWN) {
                if(capsLockOn == 0) {
                    currModifiers = currModifiers | newModifier;
                } else if (capsLockOn == 1){
                    currModifiers = currModifiers ^ newModifier;
                    capsLockOn = 0;
                }
            } else if(event.action == KEYBOARD_ACTION_UP) {
                capsLockOn = 1;
            }
        }

        //Modifier storing for shift, alt, and control.
        if(event.key.ch >= PS2_KEY_SHIFT && event.key.ch < PS2_KEY_CTRL) {
            if(event.action == KEYBOARD_ACTION_DOWN) {
                currModifiers = currModifiers | newModifier;
            } else if(event.action == KEYBOARD_ACTION_UP) {
                //need to check to make sure it's the 2nd keyboard action up for the keyboard locks
                currModifiers = currModifiers ^ newModifier; //XOR to remove newMod if released by bit flipping (if currModifiers contains the newModifier, XOR-ing it with the same modifier value will effectively 0 that bit slot).
            }
        }
    }
    event.modifiers = currModifiers;
    return event;
}

/*
 * `keyboard_read_next`: Top level keyboard interface.
 *
 * This function reads (blocking) the next key typed on the keyboard.
 * The character returned reflects the current keyboard modifier settings
 * for shift and caps lock.
 *
 * Characters returned that have value <= 0x7f '~' are printable ascii
 * characters. Character values >= 0x90 are returned for those keys that are
 * are not associated with an ascii character (e.g. arrow and function keys).
 * See the ps2_codes defined in ps2.h for constants used for those keys.
 * This function calls `keyboard_read_event`.
 */
unsigned char keyboard_read_next(void) 
{
    //Keep reading events until you need to; return when you press down (not when you press)!!
    //Don't print out modifiers (check via events), e.g. CTRL should not come out; if current modifier has shift capital should return Other char instead of char
    return '!';
}

////        if(event.key.ch >= PS2_KEY_SHIFT && event.key.ch < PS2_KEY_CTRL) {
//if(event.action == KEYBOARD_ACTION_DOWN) {
//    currModifiers = currModifiers | newModifier;
//} else if(event.action == KEYBOARD_ACTION_UP) {
//    //need to check to make sure it's the 2nd keyboard action up for the keyboard locks
//    currModifiers = currModifiers ^ newModifier; //XOR to remove newMod if released by bit flipping (if currModifiers contains the newModifier, XOR-ing it with the same modifier value will effectively 0 that bit slot).
//}
//} else if((event.key.ch == PS2_KEY_SCROLL_LOCK || event.key.ch == PS2_KEY_NUM_LOCK || event.key.ch == PS2_KEY_CAPS_LOCK) && event.action == KEYBOARD_ACTION_DOWN) {
//    keyboard_modifiers_t bitMask = currModifiers;
//    bitMask = currModifiers & newModifier;
//    if(bitMask != 0) { //ANDed, and if not 0, then the lock for this modifier is currently active, will be removed
//        currModifiers = currModifiers ^ newModifier;
//    }
//    else {
//        currModifiers = currModifiers | newModifier;
//    }
//}
