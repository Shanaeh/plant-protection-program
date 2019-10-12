/*
 * Low-level frameuffer routines for controlling a bare metal
 * Raspberry Pi's graphics. Presents a useful abstraction of hardware
 * that a graphics library can then use to provide useful drawing
 * primitives.
 *
 * In assignment 6 you are given a single-buffered implementation
 * of this module and extend it to provide double-buffering.
 *
 * Author: Philip Levis <pal@cs.stanford.edu>
 * Date: Mar 23 2016
 */

#include "mailbox.h"
#include "fb.h"
#include "printf.h"

// This prevents the GPU and CPU from caching mailbox messages
#define GPU_NOCACHE 0x40000000

typedef struct {
    unsigned int width;       // width of the display
    unsigned int height;      // height of the display
    unsigned int virtual_width;  // width of the virtual framebuffer
    unsigned int virtual_height; // height of the virtual framebuffer
    unsigned int pitch;       // number of bytes per row
    unsigned int depth;       // number of bits per pixel
    unsigned int x_offset;    // x of the upper left corner of the virtual fb
    unsigned int y_offset;    // y of the upper left corner of the virtual fb
    unsigned int framebuffer; // pointer to the start of the framebuffer
    unsigned int size;        // number of bytes in the framebuffer
} fb_config_t;

// fb is volatile because the GPU will write to it
static volatile fb_config_t fb __attribute__ ((aligned(16)));

/*
 * Initialize the framebuffer.
 *
 * @param width  the requested width in pixels of the framebuffer
 * @param height the requested height in pixels of the framebuffer
 * @param depth  the requested depth in bytes of each pixel
 * @param mode   whether the framebuffer should be
 *                      single buffered (FB_SINGLEBUFFER)
 *                      or double buffered (FB_DOUBLEBUFFER)
 */
void fb_init(unsigned int width, unsigned int height, unsigned int depth, unsigned int mode)
{
    
    //If single-buffering.
    fb.width = width;
    fb.virtual_width = width;
    fb.height = height;
    fb.virtual_height = height;
    fb.depth = depth * 8; // convert number of bytes to number of bits
    fb.x_offset = 0;
    fb.y_offset = 0;
    
    if(mode == FB_DOUBLEBUFFER) { //change all the required values
        fb.virtual_height = 2*height;
        fb.y_offset = height;
    }
    
    // the manual requires we to set these value to 0
    // the GPU will return new values
    fb.pitch = 0;
    fb.framebuffer = 0;
    fb.size = 0;
    
    //Actually initializes all of these values via this framebuffer.
    mailbox_write(MAILBOX_FRAMEBUFFER, (unsigned)&fb + GPU_NOCACHE);
    (void) mailbox_read(MAILBOX_FRAMEBUFFER);
}

/*
 * Swap the front and back buffers. The draw buffer is moved to the
 * front (displayed) and the front buffer is moved to the back
 * (becomes the draw buffer).
 *
 * If not in double buffering mode, this function has no effect.
 */
void fb_swap_buffer(void)
{
    if(fb.virtual_height == 2*fb.height) { //means in double buffering mode
        if(fb.y_offset == fb.height) {
            fb.y_offset = 0;
        } else if(fb.y_offset == 0) {
            fb.y_offset = fb.height;
        }
    }
    mailbox_write(MAILBOX_FRAMEBUFFER, (unsigned)&fb + GPU_NOCACHE);
    (void) mailbox_read(MAILBOX_FRAMEBUFFER);
}

/*
 * Get the start address of the portion of the framebuffer currently
 * being drawn into. This address is the start of an array of size
 * pitch*height bytes.  If in single buffering mode, the address
 * returned will not change as there is only one buffer in use.
 * In double buffering mode, the returned address differs based on
 * which buffer is currently being used as the draw buffer.
 *
 * @return    the address of the current draw buffer
 */
unsigned char* fb_get_draw_buffer(void)
{
    unsigned char* drawPtr = (unsigned char*) fb.framebuffer; //if normal buf or part 1 of double buffer
    
    if(fb.virtual_height == 2*fb.height && fb.y_offset == fb.height) { //means in double buffering mode
        int index = fb.y_offset*(fb.pitch/4); //fb.pitch = # of pixels in row * bytes per pixel, and each pixel requires 4 bytes... by dividing by 4 we can increment drawPtr by literal # of pixels
        drawPtr += index; //need to check why divided by depth
        //printf("index %d", index);
    }
    return drawPtr;
}

/*
 * Get the current width in pixels of the framebuffer.
 *
 * @return    the width in pixels
 */
unsigned int fb_get_width(void)
{
    return fb.width;
}

/*
 * Get the current height in pixels of the framebuffer.
 *
 * @return    the height in pixels
 */
unsigned int fb_get_height(void)
{
    return fb.height;
}

/*
 * Get the current depth in bytes of a single pixel.
 *
 * @return    the depth in bytes
 */
unsigned int fb_get_depth(void)
{
    return fb.depth;
}

/*
 * Get the current pitch in bytes of a single row of pixels in the framebuffer.
 * The pitch is nominally the width (pixels per row) multiplied by the depth
 * (in bytes per pixel). However, the pitch may be greater than that if the
 * GPU elects to add padding to the end of the row.
 *
 * @return    the pitch in bytes
 */
unsigned int fb_get_pitch(void)
{
    return fb.pitch;
}

