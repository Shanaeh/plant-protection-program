/*
 * File: malloc.c
 * --------------
 * At the start, this is the simple "bump" allocator from lecture.
 * An allocation request is serviced by tacking on the requested
 * space to the end of the heap thus far. 
 * It does not recycle memory (free is a no-op) so when all the
 * space set aside for the heap is consumed, it will not be able
 * to service any further requests.
 *
 * This code is given here just to show the very simplest of
 * approaches to dynamic allocation. You are to replace this code
 * with your own heap allocator implementation.
 */

#include "malloc.h"
#include <stddef.h> // for NULL
#include "strings.h"
#include "printf.h"

extern int __bss_end__;

// Simple macro to round up x to multiple of n.
// The efficient but tricky bitwise approach it uses
// works only if n is a power of two -- why?
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

#define TOTAL_HEAP_SIZE 0x1000000 // 16 MB


/* Global variables
 *
 * `heap_start` tracks where heap segment begins in memory.
 * It is initialized to point to the address at end of data segment.
 * It uses symbol __bss_end__ from memmap to locate end.
 * `heap_used` tracks the number of bytes allocated thus far.
 * The next available memory begins at heap_start + heap_used.
 * `heap_max` is total number of bytes set aside for heap segment.
 */
static void *heap_start = NULL;
static int heap_used = 0, heap_max = TOTAL_HEAP_SIZE;

/* Struct: header
 * To package the payload_size and status data.
 */
struct header {
    size_t payload_size;
    int status; //0 if free, 1 if in use
};

//Function to round to nearest power of 2 (e.g. 32), use after round to 8.
size_t roundPower2(size_t nbytes)
{
    size_t value = 1;
    while(value <= nbytes) {
        value = value << 1; //bitwise operation that I only vaguely understand; power of 2 for the win
    }
    return value;
}

/* Function: buddySearch
 Pseudocode/inspiration taken from the wikipedia page on buddy allocation + stackOverflow!
 
 */
//void *buddySearch(size_t totalBlock) {
//    struct header* headerPtr = heap_start;
//    //search for a block of next power of 2 above totalBlock's size.
//    //for(i = 0; (1 << i) < totalBlock; i++); //increment so i is the power of 2 below totalBlock's power of 2.
//    while(1) {
//        //If you're lucky, then yay, return.
//        if(headerPtr->payload_size == totalBlock && headerPtr->status == 0) {
//            struct header* currHeader = headerPtr;
//            currHeader->payload_size = totalBlock - sizeof(struct header);
//            currHeader->status = 1;
//
//            char* tempAlloc = (char* )currHeader + sizeof(struct header);//address of headerPtr + 8 bytes start
//            alloc = (void *)tempAlloc;
//            heap_used += totalBlock;//assume the entire block is utilized
//            return alloc;
//        }
//        //Otherwise, move forward
//        size_t blockSize = headerPtr->payload_size;
//        headerPtr = headerPtr + headerIncrement + (blockSize/sizeof(struct header)); //check next header by traversing list, increments by 8 bytes at a time, so + the size of header
//
//        //End if < 16 bytes away from the end (not enough space for header/allocation w/ the 8-byte alignment)
//        if((char* )headerPtr > heapEnd) {
//            break;
//        }
//    }
//
//    int i;
//    for(i = 0; (1 << i) < totalBlock; i++); //increment so i is the power of 2 below totalBlock's power of 2.
//
//    else if(
//
//    else {
//        void* orig, buddy;
//        orig = buddySearch(1 << i+1); //find the closest block of power 2 greater than totalBlock.
//
//        if(orig != NULL) {
//            buddy =
//        }
//        return orig;
//
//    }
//}

/* Function: malloc
 * ----------------
 * Services a dynamic allocation request. Returns the
 * address of a block of at least `nybtes` contiguous bytes
 * or NULL if the request cannot be satisifed.
 */
//EXTENSION: attempt at buddy memory allocation (commented out), but added a new improvement to coalescing!
void *malloc(size_t nbytes) 
{
    if(nbytes == 0) return NULL; //special case malloc(0);
    //Initializes the heap_start with 2 blocks
    if (!heap_start) {
        heap_start = &__bss_end__;//literally the address of the start, then header1
        
        //put header1 at start of heap
        struct header* h1Ptr = heap_start;
        h1Ptr->payload_size = heap_max- sizeof(struct header);
        h1Ptr->status = 0;
        
        heap_used = sizeof(struct header); //currently the size of the struct, 8 bytes
//        h1Ptr->payload_size = heap_max/2 - sizeof(struct header);
//        h1Ptr->status = 0;
//
//        struct header* h2Ptr = (struct header*) heap_start + (heap_max/2)/sizeof(struct header);
//        h2Ptr->payload_size = heap_max/2 - sizeof(struct header);
//        h2Ptr->status = 0;
//
//        heap_used = 2*sizeof(struct header);
    }
    struct header* headerPtr = heap_start; //to traverse, start at the beginning!
    size_t headerIncrement = 1; //headerPtr increments by 1 (to adjust for size of header, which is 8 bytes.
    
    char* heapEnd = (char* )heap_start + (heap_max - 2*sizeof(struct header)); //effectively using pointer of heap_start address + (heap_max - 16 bytes)

    //Check to make sure request for memory is valid.
    nbytes = roundup(nbytes, 8);
    if (heap_used + nbytes > heap_max) {
        return NULL;
    }
    
    //Need to fit it all in one block; subtract out header in payload later.
    void* alloc = NULL;
//    size_t totalBlock = roundPower2(nbytes + sizeof(struct header));
//    //First see if there's a block of this EXACT size
//    while(1) {
//        //If you're lucky, then yay, return.
//        if(headerPtr->payload_size == totalBlock && headerPtr->status == 0) {
//            struct header* currHeader = headerPtr;
//            currHeader->payload_size = totalBlock - sizeof(struct header);
//            currHeader->status = 1;
//
//            char* tempAlloc = (char* )currHeader + sizeof(struct header);//address of headerPtr + 8 bytes start
//            alloc = (void *)tempAlloc;
//            heap_used += totalBlock;//assume the entire block is utilized
//            return alloc;
//        }
//        //Otherwise, move forward
//        size_t blockSize = headerPtr->payload_size;
//        headerPtr = headerPtr + headerIncrement + (blockSize/sizeof(struct header)); //check next header by traversing list, increments by 8 bytes at a time, so + the size of header
//
//        //End if < 16 bytes away from the end (not enough space for header/allocation w/ the 8-byte alignment)
//        if((char* )headerPtr > heapEnd) {
//            break;
//        }
//    }
//    //If not, then we create our own block! Woohoo!
//    alloc = buddySearch(totalBlock);
  
    while(1) {
        if(headerPtr->payload_size > (nbytes + sizeof(struct header)) && headerPtr->status == 0) {
            break;
        }
        //Otherwise, move forward
        size_t blockSize = headerPtr->payload_size;
        headerPtr = headerPtr + headerIncrement + (blockSize/sizeof(struct header)); //check next header by traversing list, increments by 8 bytes at a time, so + the size of header
        
        //End if < 16 bytes away from the end (not enough space for header/allocation w/ the 8-byte alignment)
        if((char* )headerPtr > heapEnd) {
            return NULL;
        }
    }
    
    //If you've found a free header... Update this header. (Assume header made from previous malloc call.)
    struct header* currHeader = headerPtr;
    size_t currOrigPayload = currHeader->payload_size;
    
    //Update current header with values.
    currHeader->payload_size = nbytes;
    currHeader->status = 1;//used
    
    char* tempAlloc = (char* )currHeader + sizeof(struct header);//address of headerPtr + 8 bytes start
    alloc = (void *)tempAlloc;
    heap_used += nbytes;
    
    //Code to split from free block and check/make new header.
    struct header* nextHeader = headerPtr + headerIncrement + (nbytes/sizeof(struct header));
    
    //If at effective end OR not enough space (16 bytes remaining), don't bother with a header. These are technically the same, but it helps me distinguish between heap end vs. header end from freeing.
    if((char* )nextHeader > heapEnd || currOrigPayload - nbytes < 2*sizeof(struct header)) {
        //Housekeeping: if not enough space/past effective end, means only 8 bytes left (due to 8-byte alignment rounding) worth keeping.
        currHeader->payload_size += sizeof(struct header);
        heap_used += sizeof(struct header);
        return alloc;
    }
    //Otherwise, do create a header.
    heap_used += sizeof(struct header);
    nextHeader->payload_size = currOrigPayload - nbytes - sizeof(struct header);
    nextHeader->status = 0;//free
    
    return alloc;
}

/* Function: free
 * --------------
 * Deallocates the memory at address `ptr`.
 *
 * The `ptr` argument is expected to an address that was previously
 * return by malloc and has not yet been freed. If this
 * precondition is not satisified, the behavior is undefined.
 */

//"Extension": frees searching from the very beginning (but does add a lot of overhead to free).
//Just wanted to see if there was a way to clear all the extra spaces in my heap testing.
void free(void *ptr) 
{
    if(ptr == NULL) return; //free(NULL)
    
    char* temp = (char*) ptr - sizeof(struct header); //subtract by 8 bytes
    struct header* currHeader = (struct header*) temp;
    currHeader->status = 0;
    heap_used -= currHeader->payload_size; //Header still exists, only the block space is now free
    
    
     //Coalesce through entire heap!
    struct header* nextHeader = currHeader;
    char* heapEnd = (char* )heap_start + (heap_max - 2*sizeof(struct header)); //effectively using pointer of heap_start address + (heap_max - 16 bytes)
    size_t headerIncrement = 1; //headerPtr increments by 1 (to adjust for size of header, which is 8 bytes.
    
    struct header* prevHeader = heap_start;
    int prevHeaderFree = prevHeader->status; //0 free, 1 used
    
    while(1) {
        nextHeader = prevHeader + headerIncrement + (prevHeader->payload_size)/sizeof(struct header);
        
        //Keep going until the end...
        if((char* )nextHeader > heapEnd) {
            break;
        }
        else if(nextHeader->status == 0 && prevHeaderFree == 0) {
            //Combine them, update currHeader size (assume this block as already been freed, not coalesced)
            size_t nextBlockSize = nextHeader->payload_size;
            
            prevHeader->payload_size += nextBlockSize + sizeof(struct header); //update payload + header overhead
            heap_used -= sizeof(struct header); //now we're removing header overhead! Assume the payload_size has already been subtracted from heap_used via a previous free!
        }
        else if(nextHeader->status == 1) {//if used, skip it (since nextHeader will become prevHeader).
            prevHeaderFree = 1;
            prevHeader = nextHeader;
        }
        else if(nextHeader->status == 0) {//if free, mark it down.
            prevHeaderFree = 0;
            prevHeader = nextHeader;
        }
    }
}

/* Function: realloc
 * -----------------
 * Resizes the memory allocated for `ptr` to `new_size`. If the
 * requested change in size cannot be accommodated in-place,
 * realloc() creates a new allocation, copies as much of the data
 * pointed to by `ptr` as will fit to the new allocation, frees
 * the previous allocation, and returns a pointer to the new memory.
 *
 * The `ptr` argument is expected to an address that was
 * previously return by malloc and has not yet been freed.
 * If this precondition is not satisified, the behavior is undefined.
 */
void *realloc (void *old_ptr, size_t new_size)
{
    if(old_ptr == NULL && new_size>0) {
        void* temp = malloc(new_size); //realloc(NULL, size)
        return temp;
    }
    if(old_ptr != NULL && new_size<=0) { //realloc(ptr, 0)
        free(old_ptr);
        return NULL;
    }

    //Getting currHeader pointer and calculating needed space.
    char* temp = (char* )old_ptr - sizeof(struct header); //subtract by 8 bytes to get to header
    struct header* currHeader = (struct header* ) temp;
    
    size_t spaceToExpand = roundup(new_size, 8) - currHeader->payload_size;

    //Helper variables from malloc.
    size_t headerIncrement = 1; //headerPtr increments by 1 (to adjust for size of header, which is 8 bytes.
    char* heapEnd = (char* )heap_start + (heap_max - 2*sizeof(struct header)); //effectively using pointer of heap_start address + (heap_max - 16 bytes)
    
    //Finding the nextHeader. Check for heapEnd, not being used, and available space.
    struct header* nextHeader = currHeader + headerIncrement + (currHeader->payload_size)/sizeof(struct header);
    if((char *) nextHeader < heapEnd && nextHeader->status != 1 && spaceToExpand < nextHeader->payload_size) {
        //Literally creating a newHeader so that nextHeader can be overwritten.
        char* tempH = (char* )nextHeader + spaceToExpand;
        struct header* newHeader = (struct header* )tempH;
        newHeader->payload_size = nextHeader->payload_size - spaceToExpand; //don't subtract header overhead
        newHeader->status = 0;
        
        //Update the current header
        currHeader->payload_size += spaceToExpand;
        heap_used += spaceToExpand;

        return old_ptr; //Space with nextHeader will be overwritten.
    }
    //Else, just allocate as is.
    void *new_ptr = malloc(new_size);
    if (!new_ptr) return NULL;
    
    memcpy(new_ptr, old_ptr, new_size);
    free(old_ptr);
    return new_ptr;
}

void heap_dump () {
    struct header* headerPtr = heap_start;
    size_t headerIncrement = 1; //headerPtr increments by 1 (to adjust for size of header, which is 8 bytes.
    char* heapEnd = (char* )heap_start + (heap_max - 2*sizeof(struct header)); //effectively using pointer of heap_start address + (heap_max - 16 bytes)
    int currBlock = 1;
    
    //Will break search when at the end of the heap.
    while(1) {
        size_t blockSize = headerPtr->payload_size;
        int blockStatus = headerPtr->status;
        
        printf("Block %d of header address %p: payload size of %d bytes, status of %d \n", currBlock, headerPtr, blockSize, blockStatus);

        headerPtr = headerPtr + headerIncrement + (blockSize/sizeof(struct header)); //check next header by traversing list, increments by 4 bytes at a time, so + the size of header
        
        //End if at 15 bytes away from the end or more (not enough space for header/allocation w/ the 8-byte alignment)
        if((char* )headerPtr > heapEnd) {
            break;
        }
        currBlock++;
    }
}
