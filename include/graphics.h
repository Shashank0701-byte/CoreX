// Graphics Mode Support
// VESA/VGA graphics functions for drawing pixels and shapes

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "stdint.h"

// Graphics mode settings
#define GRAPHICS_WIDTH  320
#define GRAPHICS_HEIGHT 200
#define GRAPHICS_BPP    8       // 8-bit color (256 colors)

// VGA Mode 13h framebuffer
#define FRAMEBUFFER 0xA0000

// Color palette (VGA 256-color mode)
#define COLOR_BLACK         0x00
#define COLOR_BLUE          0x01
#define COLOR_GREEN         0x02
#define COLOR_CYAN          0x03
#define COLOR_RED           0x04
#define COLOR_MAGENTA       0x05
#define COLOR_BROWN         0x06
#define COLOR_LIGHT_GRAY    0x07
#define COLOR_DARK_GRAY     0x08
#define COLOR_LIGHT_BLUE    0x09
#define COLOR_LIGHT_GREEN   0x0A
#define COLOR_LIGHT_CYAN    0x0B
#define COLOR_LIGHT_RED     0x0C
#define COLOR_LIGHT_MAGENTA 0x0D
#define COLOR_YELLOW        0x0E
#define COLOR_WHITE         0x0F

// Graphics functions
void graphics_init();
void graphics_clear(uint8_t color);
void graphics_putpixel(int x, int y, uint8_t color);
uint8_t graphics_getpixel(int x, int y);
void graphics_draw_line(int x1, int y1, int x2, int y2, uint8_t color);
void graphics_draw_rect(int x, int y, int width, int height, uint8_t color);
void graphics_fill_rect(int x, int y, int width, int height, uint8_t color);
void graphics_draw_char(int x, int y, char c, uint8_t color);
void graphics_draw_string(int x, int y, const char* str, uint8_t color);

// Mode switching
void graphics_set_mode_13h();
void graphics_set_text_mode();

#endif // GRAPHICS_H
