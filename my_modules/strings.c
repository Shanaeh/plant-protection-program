#include "strings.h"
#include "assert.h"
#include "printf.h"

/***
 Taking void address *s and converting to a string pointer to return.
 Used to fill (one) block of memory of size_t n, of address pointer s, with a value of c (e.g. 8 bit char at a time).
 Often used to initialize char arrays/structs w/ 1 value at once.
 */
void *memset(void *s, int c, size_t n)
{
    unsigned char *charArrayPtr = s;
    for(int i=0; i < n; i++) {
        charArrayPtr[i] = c;
    }
    return charArrayPtr;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    //Just to typecast and make it clear where's the start!
    
    unsigned char *dstArrayPtr = dst;
    unsigned const char *srcArrayPtr = src;
    
    for(int i=0; i<n; i++) {
        dstArrayPtr[i] = srcArrayPtr[i];
    }
    return dstArrayPtr;
}

int strlen(const char *s)
{
    /* Implementation a gift to you from lab3 */
    int i;
    for (i = 0; s[i] != '\0'; i++) ;
    return i;
}

/***
 Takes two strings and compares them, at the stopping value (where they differ), return and check if s1's value at that point is <, >, or =.
 Also, unsigned char and const char are the same thing, so yay!
 For lexicographical importance, e.g. to see which one comes up first in the dictionary.
 */
int strcmp(const char *s1, const char *s2)
{
    while(*s1 && (*s1 == *s2)) { //if (*s1 != '\0'), since terminates on 0
        s1++;
        s2++;
    }
    return *s1 - *s2; //at the place where the strings left off.
}

/***
 Combines dst with src (appends src to the end of dst).
 String, length, concatenation.
 
 */
int strlcat(char *dst, const char *src, int maxsize)
{
    int dstLen = strlen(dst);
    int srcLen = strlen(src);

    for(int i=0; i<maxsize - dstLen - 1; i++) {
        dst[dstLen + i] = src[i];
    }
    //Adds null terminator if maxsize transcended
    if(dstLen + srcLen > maxsize - 1) {
        dst[maxsize - 1] = '\0';
    }
    
    return dstLen + srcLen;
}

/***
 String to number.
 */
unsigned int strtonum(const char *str, const char **endptr)
{
    unsigned int convertedNum = 0;
    const char *currStrVal = str;
    //Hex base
    if(currStrVal[0] == '0' && currStrVal[1] == 'x') {
        currStrVal += 2;//to get past the 0x
        while(*currStrVal) {
            volatile unsigned int tempVal = 0;
            if(*currStrVal >='0' && *currStrVal<='9') {
                tempVal = *currStrVal - '0'; //need to subtract ASCII value to store int value.
            } else if(*currStrVal >='a' && *currStrVal<='f') {
                tempVal = *currStrVal - 'a' + 10;
            } else if(*currStrVal >='A' && *currStrVal<='F') {
                tempVal = *currStrVal - 'A' + 10;
            } else { //not a valid input
                break;
            }
            convertedNum *=16; //because incrementing with base 16.
            convertedNum += tempVal;
            currStrVal++;
        }
    }//Decimal base
     else {
        while(*currStrVal) {
            volatile unsigned int tempVal = 0;
            if(*currStrVal >='0' && *currStrVal<='9') {
                tempVal = *currStrVal - '0'; //need to subtract ASCII value to store int value.
            } else { //not a valid input
                break;
            }
            convertedNum *=10; //because incrementing new tens value.
            convertedNum += tempVal;
            currStrVal++;
        }
    }
    *endptr = currStrVal;
    return convertedNum;
}
