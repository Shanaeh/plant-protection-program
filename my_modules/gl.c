#include "gl.h"
#include "printf.h"
#include "font.h"
#include "strings.h"
/*
 *
 * Define a type for color. We use BGRA colors, where each color
 * component R, B, G, or A is a single unsigned byte. The least
 * signficant byte is the B component, and A is most significant.
 *
typedef unsigned int color_t;

 * Define some common colors ...
 *
 * Note that colors are BGRA, where B is the first byte in memory
 * and the least significant byte in the unsigned word.
 *
#define GL_BLACK   0xFF000000
#define GL_WHITE   0xFFFFFFFF
#define GL_RED     0xFFFF0000
#define GL_GREEN   0xFF00FF00
#define GL_BLUE    0xFF0000FF
#define GL_CYAN    0xFF00FFFF
#define GL_MAGENTA 0xFFFF00FF
#define GL_YELLOW  0xFFFFFF00
#define GL_AMBER   0xFFFFBF00
 */

/*
 * Initialize the graphic library. This function will call fb_init in turn
 * to initialize the framebuffer. The framebuffer will be initialzed to
 * 4-byte depth (32 bits per pixel).
 *
 * @param width  the requested width in pixels of the framebuffer
 * @param height the requested height in pixels of the framebuffer
 * @param mode   whether the framebuffer should be
 *                  single buffered (GL_SINGLEBUFFER)
 *                  or double buffered (GL_DOUBLEBUFFER)
 */
void gl_init(unsigned int width, unsigned int height, unsigned int mode)
{
    fb_init(width, height, 4, mode);
}

/*
 * Swap the front and back buffers. The draw buffer is moved to the
 * front (displayed) and the front buffer is moved to the back
 * (becomes the draw buffer).
 *
 * If not in double-buffer mode, this function has no effect.
 */
void gl_swap_buffer(void)
{
    fb_swap_buffer();
}

/*
 * Get the current width in pixels of the framebuffer.
 *
 * @return    the width in pixels
 */
unsigned int gl_get_width(void)
{
    return fb_get_width();
}

/*
 * Get the current height in pixels of the framebuffer.
 *
 * @return    the height in pixels
 */
unsigned int gl_get_height(void)
{
    return fb_get_height();
}

/*
 * Returns a color composed of the specified red, green, and
 * blue components. The alpha component of the color will be
 * set to 0xff (fully opaque).
 *
 * @param r  the red component of the color
 * @param g  the green component of the color
 * @param b  the blue component of the color
 *
 * @return   the color as a single value of type color_t
 */
color_t gl_color(unsigned char r, unsigned char g, unsigned char b)
{
    //blue / green / red /alpha
    color_t color = b | (g << 8) | (r << 16) | (0xFF << 24);
    return color;
}


/*
 Helper: checks in bound.
 */
unsigned int inBounds(int x, int y) {
    unsigned int toReturn = 0;
    
    if(x < gl_get_width() && y < gl_get_height()) {
        toReturn = 1;
    }
    return toReturn;
}


/*
 * Clear all the pixels in the framebuffer to the given color.
 *
 * @param c  the color drawn into the framebuffer
 */
void gl_clear(color_t c)
{
    unsigned char* fb = fb_get_draw_buffer();
    unsigned (*fbGrid)[fb_get_pitch()/4] = (unsigned (*)[fb_get_pitch()/4]) fb;

     for(int y = 0; y < gl_get_height(); y++) { //going by pixels
        for(int x = 0; x <gl_get_width(); x++) {
             fbGrid[y][x] = c;
        }
    }
}

/*
 * Draw a single pixel at location x,y in color c.
 * If the location is outside the bounds of framebuffer, it is not drawn.
 *
 * @param x  the x location of the pixel
 * @param y  the y location of the pixel
 * @param c  the color of the pixel
 */
void gl_draw_pixel(int x, int y, color_t c)
{
    unsigned char* fb = fb_get_draw_buffer();
    unsigned (*fbGrid)[fb_get_pitch()/4] = (unsigned (*)[fb_get_pitch()/4]) fb;
    
    if(inBounds(x, y) == 1) fbGrid[y][x] = c;
}

/*
 * Return the color of the pixel at location x,y. Returns 0 if the
 * location is outside the bounds of the framebuffer.
 *
 * @param x  the x location of the pixel
 * @param y  the y location of the pixel
 *
 * @return   the color at that location
 */
color_t gl_read_pixel(int x, int y)
{
    unsigned char* fb = fb_get_draw_buffer();
    unsigned (*fbGrid)[fb_get_pitch()/4] = (unsigned (*)[fb_get_pitch()/4]) fb;
    
    color_t color = 0;
    if(inBounds(x, y) == 1) color = fbGrid[y][x];
    return color;
}

/*
 * Draw a filled rectangle at location x,y with size w,h filled with color c.
 * All pixels in the rectangle that lie within the bounds of the
 * framebuffer are drawn. Any pixel that lies outside is clipped (i.e. not drawn).
 *
 * @param x  the x location of the upper left corner of the rectangle
 * @param y  the y location of the upper left corner of the rectangle
 * @param w  the width of the rectangle
 * @param h  the height of the rectangle
 * @param c  the color of the rectangle
 */
void gl_draw_rect(int x, int y, int w, int h, color_t c)
{
    unsigned char* fb = fb_get_draw_buffer();
    unsigned (*fbGrid)[fb_get_pitch()/4] = (unsigned (*)[fb_get_pitch()/4]) fb;

    for(int tempY = 0; tempY < h; tempY++) { //going by pixels
        for(int tempX = 0; tempX < w; tempX++) {
             if(inBounds(x, y) == 1) fbGrid[y + tempY][x + tempX] = c;
        }
    }
}

/*
 * Draw a single character at location x,y in color c.
 * Only those pixels of the character that lie within the bounds
 * of the framebuffer are drawn. Any pixel that lies outside is
 * clipped (i.e. not drawn).
 *
 * @param x   the x location of the upper left corner of the character glyph
 * @param y   the y location of the upper left corner of the character glyph
 * @param ch  the character to be drawn, e.g. 'a'. If this character has no glyph
 *            in the current font, nothing is drawn(refer to font_get_char())
 * @param c   the color of the character
 */
//Assume 32-bit image
void gl_draw_char(int x, int y, int ch, color_t c)
{
    unsigned char* fb = fb_get_draw_buffer();
    unsigned (*fbGrid)[fb_get_pitch()/4] = (unsigned (*)[fb_get_pitch()/4]) fb;
    
    unsigned char buf[font_get_size()]; //will become a bitmap
    font_get_char(ch, buf, font_get_size());
    
    for(int tempY = 0; tempY < font_get_height(); tempY++) { //going by pixels
        for(int tempX = 0; tempX < font_get_width(); tempX++) {
            int index = tempY*font_get_width() + tempX;
            if(inBounds(x, y) == 1 && buf[index] != 0) {
                fbGrid[y + tempY][x + tempX] = c;
            }
        }
    }
}

/*
 * Draw a string at location x,y in color c. The characters are drawn
 * left to right in a single line. Only the pixels of the characters
 * that lie within the bounds of the framebuffer are drawn. Any pixel
 * that lies outside is clipped (i.e. not drawn).
 *
 * @param x    the x location of the upper left corner of the first char of string
 * @param y    the y location of the upper left corner of the first char of string
 * @param str  the null-terminated string to be drawn
 * @param c    the color of the string
 */
void gl_draw_string(int x, int y, char* str, color_t c)
{
    //later, can adapt y so the string can roll over to the new line.
    while(*str != '\0' && inBounds(x, y) == 1) {
        unsigned char currChar = *str;
        gl_draw_char(x, y, currChar, c);
        x += gl_get_char_width();
        str++;
    }
}

/*
 * Get the height in pixels of a single character glyph.
 *
 * @return the character height in pixels
 */
unsigned int gl_get_char_height(void)
{
    return font_get_height();
}

/*
 * Get the width in pixels of a single character glyph.
 *
 * @return the character width in pixels
 */
unsigned int gl_get_char_width(void)
{
    return font_get_width();
}

