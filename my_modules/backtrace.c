#include "backtrace.h"
#include "printf.h"
//NOTE: The comments are for my personal understanding (apologies if they're lengthy) -- I want to be able to return to this weeks from now (circa final project) and understand what I did! Thanks!

/* Type: frame_t
 * -------------
 * This struct stores the information for a caller function who has a
 * frame on the stack.
 *
 * The `name` fields points to the string name of the function. That name is in
 * the text section with the instructions.  If the function name is not
 * available, the string will be "???".
 *
 * The `resume_addr` field is taken from saved lr in the callee. The saved lr
 * (sometimes referred to as "return address") is the address of the
 * instruction in the caller's sequence of instructions where control will
 * resume after the callee returns.
 *
 * The `resume_offset` is the number of bytes from the start of the function to
 * the `resume_addr`. This offset represents how far control has advanced into
 * the caller function before it invoked the callee.
 
typedef struct {
    char *name;
    uintptr_t resume_addr;
    int resume_offset;
} frame_t;
 */


/* Function: backtrace
 * -------------------
 * backtrace() gathers a backtrace for the calling program and writes the
 * call stack information into the array pointed to by `f`.
 *
 * A backtrace is the sequence of currently active function calls. Each element
 * in the array `f` is of type `frame_t` (struct described above). Each struct
 * represents a caller who has a frame on the stack.
 *
 * The `max_frames` argument specifies the maximum number of frames that can be
 * stored in the array f.  If the backtrace contains more than `max_frames`,
 * then the information for only the `max_frames` most recent function calls
 * are stored in the array `f`.
 *
 * The return value of the function is the number of frames written to `f`.
 */
int backtrace(frame_t f[], int max_frames)
{
    /*Gets the current frame pointer (the anchor in r12 from wchich all offsets relative to it stay constant.*/
    void *first_fp;
    __asm__("mov %0, fp" : "=r" (first_fp));
    unsigned int* curr_fp = (unsigned int*) first_fp;
    
    int numOfFrames = 0; //if curr_fp == 0, then you're at _cstart :)
    for(int i=0; i<max_frames && (curr_fp != 0); i++) {
  
        unsigned int* saved_fp = curr_fp - 3; //pointer to the address in fp, which takes you to the saved pc, which is 12 bytes away
        
        //AKA as the sp (for the moment)
        unsigned int* saved_pc = (unsigned int*) *saved_fp; //we are dereferencing the saved_fp to get us to the saved pc of the previous function, and then casting it so we can go directly to that address
        
        if(saved_pc == 0) break; //if the frame pointer of the next function == 0, that means we're at _cstart! Don't save any more data!
        
        unsigned int* saved_lr = curr_fp - 1;
        f[i].resume_addr = (uintptr_t) *saved_lr; //the address directly below (-4 bytes; this address holds the saved "lr" to resume w/ the callee's address of next line)
        
        
        //AKA the previous function's fp, which holds its saved pc that we dereference to get to the resume
        unsigned int* prev_func_resume = (unsigned int*) *saved_pc; //we're getting its saved_pc address so we get a pointer to the previous function's resume
        
        unsigned int* prev_func_start = prev_func_resume - 3; //gets us to where the function actually starts (12 byte offset accounts for prologue)
        
        f[i].resume_offset = f[i].resume_addr - (uintptr_t) prev_func_start; //offset between resume address and adress of caller's first instruction/the start of the function
        
        
        //Go to the name of the "previous"/caller function (not the one we are currently in, which is backtrace(), but the caller).
        unsigned int* name_marker = prev_func_start - 1; //pointer to the potential address right before that denotes if there's a name, and the strlen
        
        unsigned int name_markerMask = *name_marker & 0xff000000; //check the first 2 digits
        if(name_markerMask == 0xff000000) {
            int length = *name_marker - 0xff000000;
            char* name = (char*) name_marker - length; //going by actual bytes now, because char* is 1 byte each
            f[i].name = name;
        } else {
            f[i].name = "???"; //for wild cases
        }
        
        curr_fp = (unsigned int*) *saved_fp; //to update curr_fp pointer to the curr_fp of the previous function.
        numOfFrames++;
    }
    return numOfFrames;
}

/* Function: print_frames
 * ----------------------
 * Given an array of frames that has previously been filled in by a call to
 * backtrace(), `print_frames` prints the backtrace information, one line per
 * frame, using this format:
 *
 *     #0 0x85f8 at malloc+132
 *     #1 0x8868 at strndup+28
 *     #2 0x8784 at main+24
 */
void print_frames (frame_t f[], int n)
{
    for (int i = 0; i < n; i++)
        printf("#%d 0x%x at %s+%d\n", i, f[i].resume_addr, f[i].name, f[i].resume_offset);
}

/* Function: print_backtrace
 * -------------------------
 * Convenience function that calls `backtrace` and `print_frames` to display
 * stack frames of currently executing program.
 */
void print_backtrace (void)
{
    int max = 50;
    frame_t arr[max];

    int n = backtrace(arr, max);
    print_frames(arr+1, n-1);   // print frames starting at this function's caller
}
