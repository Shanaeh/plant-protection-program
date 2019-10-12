#include "printf.h"
#include <stdarg.h>
#include "strings.h"
#include "uart.h"

#define MAX_OUTPUT_LEN 1024
/*
 buf = the space in the charArray, bufsize = the max size (so truncate string if > bufsize -1 so there's room for the null terminator), val is the integer value to be converted into a string, base is either hex or decimal, and min_width is # of spaces the string must take up to print (so pad the beginning with zeroes).
 buf does not include null terminator, but min_width does.
 */
int unsigned_to_base(char *buf, int bufsize, unsigned int val, int base, int min_width)
{
    //Checks string length of values only (no neg or null-terminating).
    volatile int strLen = 0;
    volatile unsigned int tempVal = val;
    while(1) {
        if(tempVal == 0) break;
        strLen++;
        tempVal /= base; //Use NEXT instead of STEP!!
        
    }
    //Every single check is necessary; figuring out which ones took hours!
    volatile int bufIndex = 0;
    int stopIndex = bufsize - 1; //largest possible index for null term
    if(strLen < stopIndex) stopIndex = strLen; //adjust to the string size
    if(strLen < min_width) stopIndex = min_width; //adjust for padding
    if(min_width > bufsize -1) stopIndex = bufsize - 1; //adjust for truncation
    
   //Pads with zeros.
    if(strLen < min_width) {
        for(bufIndex; (bufIndex < min_width - strLen) && bufIndex < stopIndex; bufIndex++) {
            buf[bufIndex] = '0';
        }
    }
    
    int padding = min_width - strLen; //for counting at the end.
    if(padding < 0) padding = 0;
    for(int i=0; bufIndex + i < stopIndex; i++) { //how much space to go = hard limit - stop value
        unsigned int tempVal = val;
        unsigned int divExponent = strLen - (i + 1);
        unsigned int divider = 1;
        
        for(int i=0; i<divExponent; i++) {//Improvised power function.
            divider *= base;
        }
        tempVal = (val/divider) % base; //to get the remainder of this place; Use NEXT instead of STEP!!
        //Enter if/else brackets for string notation!
        if(tempVal >=0 && tempVal<=9) {
            tempVal += '0';
        } else if((tempVal >=10 && tempVal<=15) && base == 16) {//only for hex
            tempVal += 'a' - 10;
        } else { //not a valid input
            break;
        }
        buf[bufIndex + i] = tempVal;
    }
    if(stopIndex <0) stopIndex = 0; //edge case
    buf[stopIndex] = '\0';//should be at the end of the string, check the +1
    
    return strLen + padding; //return # of digits you would've written if not cut off
}

//Calls unsigned to base and accounts for negative value.
int signed_to_base(char *buf, int bufsize, int val, int base, int min_width)
{
    int toReturn = 0;
    char* tempBuf = buf;
    if(val < 0) {
        buf[0] = '-';
        val *= -1; //make easier for unsigned conversion.
        tempBuf += 1; //roll the pointer so doesn't overwrite negative.
        bufsize--; //adjust max size.
        min_width--; //adjust min_width for calculations.
        toReturn++; //to adjust for negative in array.
    }
    toReturn += unsigned_to_base(tempBuf, bufsize, val, base, min_width);
    return toReturn;
}

int vsnprintf(char *buf, int bufsize, const char *format, va_list argList)
{
    int charsPrintedSoFar = 0;
    char tempBuf[MAX_OUTPUT_LEN];
    char *currBufVal = tempBuf; //just in case for the end.
    
    volatile int activeFormatCodeSearch = 0;
    char numArray[MAX_OUTPUT_LEN];
    while(*format) {
        volatile char currChar = *format;
        
        if(currChar == '%') activeFormatCodeSearch = 1;
        
        //If % precedes it, then check for format code.
        else if(activeFormatCodeSearch == 1) {
            int charsPrinted1Round = 1; //also doubles as currBufVal to increment
            
            //Character %c
            if(currChar == 'c') {
                volatile int tempChar = va_arg(argList, int);
                *currBufVal = (char) tempChar; //Set value, and then increment currBufVal
            }
            //String %s
            else if(currChar == 's') {
                char* tempString = va_arg(argList, char*);
                int strSize = strlen(tempString);
                memcpy(currBufVal, tempString, strSize);
                
                charsPrinted1Round = strSize;
            }
            //Int dec or hex, %d and %x
            else if(currChar == '0' || currChar == 'd' || currChar == 'x') {
                int min_width = 0;
                int base = 10;
                
                //Checks for min_width.
                if(currChar == '0') {
                    const char* toResumeFormatPtr;
                    min_width = strtonum(format, &toResumeFormatPtr);
                    
                    format = toResumeFormatPtr; //currently at the d/x
                    currChar = *format;
                }
                if(currChar == 'x') base = 16;
                
                volatile int tempInt = va_arg(argList, int);
                char* convertedNum = numArray;
                signed_to_base(convertedNum, MAX_OUTPUT_LEN, tempInt, base, min_width);
                int strSize = strlen(convertedNum);
                memcpy(currBufVal, convertedNum, strSize);
                
                charsPrinted1Round = strSize;
            }
            //Pointer %p.
            else if(currChar == 'p') {
                volatile int tempInt = va_arg(argList, int);
                char* convertedNum = numArray;
                signed_to_base(convertedNum, MAX_OUTPUT_LEN, tempInt, 16, 8);
                
                *currBufVal++ = '0';
                *currBufVal++ = 'x';
                memcpy(currBufVal, convertedNum, 8);
                
                charsPrinted1Round = 8;
            }
            else { //normal input string value of %, no biggie.
                *currBufVal++ = '%';
                *currBufVal = currChar;
                charsPrinted1Round = 2; //the % sign and the currChar
                currBufVal--; //just so currBufVal will still be correct for this else case.
            }
            //Housekeeping -- update values.
            charsPrintedSoFar += charsPrinted1Round;
            activeFormatCodeSearch = 0; //reset format code search
            currBufVal += charsPrinted1Round;
            
        }
        else { //normal input string value
            *currBufVal = currChar;
            currBufVal++;
            charsPrintedSoFar++;
        }
        format++;
    }
    va_end(argList);
    *currBufVal = '\0';
    
    //Copy over temp buf string to the original. Accounts for truncation.
    int stopIndex = MAX_OUTPUT_LEN - 1;
    if(bufsize - 1 < stopIndex) stopIndex = bufsize - 1;
    for(int i=0; i<stopIndex; i++) {
        buf[i] = tempBuf[i];
    }
    buf[stopIndex] = '\0'; //only for truncation case
    
    return charsPrintedSoFar;
}

//The snprintf implementation.
int snprintf(char *buf, int bufsize, const char *format, ...)
{
    va_list argList;
    va_start(argList, format);
    int toReturn = vsnprintf(buf, bufsize, format, argList);
    va_end(argList);
    return toReturn;
}

//The grand function of them all, printf! Woohoo!
int printf(const char *format, ...)
{
    char buf[MAX_OUTPUT_LEN];
    va_list argList;
    va_start(argList, format);
    int toReturn = vsnprintf(buf, MAX_OUTPUT_LEN, format, argList);
    va_end(argList);
    
    for(int i=0; i<strlen(buf); i++) {
        uart_putchar(buf[i]);
    }
    return toReturn;
}
