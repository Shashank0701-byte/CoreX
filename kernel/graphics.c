// Graphics Mode Implementation
// VGA Mode 13h (320x200, 256 colors)

#include "graphics.h"

// Framebuffer pointer
static uint8_t* framebuffer = (uint8_t*)FRAMEBUFFER;

// Minimal 8x8 font (only essential characters: 0-9, A-Z, space)
// Reduced from 96 to 37 characters to save space
static const uint8_t font_8x8[37][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // Space (0)
    {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00}, // 0 (1)
    {0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00}, // 1 (2)
    {0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00}, // 2 (3)
    {0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00}, // 3 (4)
    {0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00}, // 4 (5)
    {0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00}, // 5 (6)
    {0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00}, // 6 (7)
    {0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00}, // 7 (8)
    {0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00}, // 8 (9)
    {0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00}, // 9 (10)
    {0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00}, // A (11)
    {0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00}, // B (12)
    {0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00}, // C (13)
    {0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00}, // D (14)
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00}, // E (15)
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00}, // F (16)
    {0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00}, // G (17)
    {0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00}, // H (18)
    {0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // I (19)
    {0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00}, // J (20)
    {0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00}, // K (21)
    {0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00}, // L (22)
    {0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00}, // M (23)
    {0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00}, // N (24)
    {0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00}, // O (25)
    {0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00}, // P (26)
    {0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00}, // Q (27)
    {0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00}, // R (28)
    {0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00}, // S (29)
    {0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // T (30)
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00}, // U (31)
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00}, // V (32)
    {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00}, // W (33)
    {0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00}, // X (34)
    {0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00}, // Y (35)
    {0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00}, // Z (36)
};

// Initialize graphics mode
void graphics_init() {
    // Graphics mode will be set on demand
}

// Port I/O functions
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Set VGA Mode 13h (320x200, 256 colors) by programming VGA registers
void graphics_set_mode_13h() {
    // VGA register ports
    #define VGA_MISC_WRITE      0x3C2
    #define VGA_SEQ_INDEX       0x3C4
    #define VGA_SEQ_DATA        0x3C5
    #define VGA_CRTC_INDEX      0x3D4
    #define VGA_CRTC_DATA       0x3D5
    #define VGA_GC_INDEX        0x3CE
    #define VGA_GC_DATA         0x3CF
    #define VGA_AC_INDEX        0x3C0
    #define VGA_AC_WRITE        0x3C0
    #define VGA_AC_READ         0x3C1
    #define VGA_INSTAT_READ     0x3DA
    
    // Miscellaneous register
    outb(VGA_MISC_WRITE, 0x63);
    
    // Sequencer registers
    outb(VGA_SEQ_INDEX, 0x00); outb(VGA_SEQ_DATA, 0x03);
    outb(VGA_SEQ_INDEX, 0x01); outb(VGA_SEQ_DATA, 0x01);
    outb(VGA_SEQ_INDEX, 0x02); outb(VGA_SEQ_DATA, 0x0F);
    outb(VGA_SEQ_INDEX, 0x03); outb(VGA_SEQ_DATA, 0x00);
    outb(VGA_SEQ_INDEX, 0x04); outb(VGA_SEQ_DATA, 0x0E);
    
    // Unlock CRTC registers
    outb(VGA_CRTC_INDEX, 0x03); outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) | 0x80);
    outb(VGA_CRTC_INDEX, 0x11); outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) & ~0x80);
    
    // CRTC registers
    uint8_t crtc_regs[] = {
        0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
        0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
        0xFF
    };
    for (int i = 0; i < 25; i++) {
        outb(VGA_CRTC_INDEX, i);
        outb(VGA_CRTC_DATA, crtc_regs[i]);
    }
    
    // Graphics Controller registers
    outb(VGA_GC_INDEX, 0x00); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x01); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x02); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x03); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x04); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x05); outb(VGA_GC_DATA, 0x40);
    outb(VGA_GC_INDEX, 0x06); outb(VGA_GC_DATA, 0x05);
    outb(VGA_GC_INDEX, 0x07); outb(VGA_GC_DATA, 0x0F);
    outb(VGA_GC_INDEX, 0x08); outb(VGA_GC_DATA, 0xFF);
    
    // Attribute Controller registers
    inb(VGA_INSTAT_READ); // Reset flip-flop
    for (int i = 0; i < 16; i++) {
        outb(VGA_AC_INDEX, i);
        outb(VGA_AC_WRITE, i);
    }
    outb(VGA_AC_INDEX, 0x10); outb(VGA_AC_WRITE, 0x41);
    outb(VGA_AC_INDEX, 0x11); outb(VGA_AC_WRITE, 0x00);
    outb(VGA_AC_INDEX, 0x12); outb(VGA_AC_WRITE, 0x0F);
    outb(VGA_AC_INDEX, 0x13); outb(VGA_AC_WRITE, 0x00);
    outb(VGA_AC_INDEX, 0x14); outb(VGA_AC_WRITE, 0x00);
    
    // Enable display
    outb(VGA_AC_INDEX, 0x20);
}

// Set text mode (Mode 3) by programming VGA registers
void graphics_set_text_mode() {
    // For simplicity, we'll just reset to a basic text mode
    // This is a simplified version - a full implementation would restore all registers
    
    outb(VGA_MISC_WRITE, 0x67);
    
    // Sequencer
    outb(VGA_SEQ_INDEX, 0x00); outb(VGA_SEQ_DATA, 0x03);
    outb(VGA_SEQ_INDEX, 0x01); outb(VGA_SEQ_DATA, 0x00);
    outb(VGA_SEQ_INDEX, 0x02); outb(VGA_SEQ_DATA, 0x03);
    outb(VGA_SEQ_INDEX, 0x03); outb(VGA_SEQ_DATA, 0x00);
    outb(VGA_SEQ_INDEX, 0x04); outb(VGA_SEQ_DATA, 0x02);
    
    // CRTC
    outb(VGA_CRTC_INDEX, 0x11); outb(VGA_CRTC_DATA, 0x0E);
    
    uint8_t crtc_text[] = {
        0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
        0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x50,
        0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
        0xFF
    };
    for (int i = 0; i < 25; i++) {
        outb(VGA_CRTC_INDEX, i);
        outb(VGA_CRTC_DATA, crtc_text[i]);
    }
    
    // Graphics Controller
    outb(VGA_GC_INDEX, 0x00); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x01); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x02); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x03); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x04); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x05); outb(VGA_GC_DATA, 0x10);
    outb(VGA_GC_INDEX, 0x06); outb(VGA_GC_DATA, 0x0E);
    outb(VGA_GC_INDEX, 0x07); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x08); outb(VGA_GC_DATA, 0xFF);
    
    // Attribute Controller
    inb(VGA_INSTAT_READ);
    for (int i = 0; i < 16; i++) {
        outb(VGA_AC_INDEX, i);
        outb(VGA_AC_WRITE, i);
    }
    outb(VGA_AC_INDEX, 0x10); outb(VGA_AC_WRITE, 0x0C);
    outb(VGA_AC_INDEX, 0x11); outb(VGA_AC_WRITE, 0x00);
    outb(VGA_AC_INDEX, 0x12); outb(VGA_AC_WRITE, 0x0F);
    outb(VGA_AC_INDEX, 0x13); outb(VGA_AC_WRITE, 0x08);
    outb(VGA_AC_INDEX, 0x14); outb(VGA_AC_WRITE, 0x00);
    
    outb(VGA_AC_INDEX, 0x20);
}

// Clear screen with color
void graphics_clear(uint8_t color) {
    for (int i = 0; i < GRAPHICS_WIDTH * GRAPHICS_HEIGHT; i++) {
        framebuffer[i] = color;
    }
}

// Plot a pixel
void graphics_putpixel(int x, int y, uint8_t color) {
    if (x >= 0 && x < GRAPHICS_WIDTH && y >= 0 && y < GRAPHICS_HEIGHT) {
        framebuffer[y * GRAPHICS_WIDTH + x] = color;
    }
}

// Get pixel color
uint8_t graphics_getpixel(int x, int y) {
    if (x >= 0 && x < GRAPHICS_WIDTH && y >= 0 && y < GRAPHICS_HEIGHT) {
        return framebuffer[y * GRAPHICS_WIDTH + x];
    }
    return 0;
}

// Draw a line (Bresenham's algorithm)
void graphics_draw_line(int x1, int y1, int x2, int y2, uint8_t color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        graphics_putpixel(x1, y1, color);
        
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Draw a rectangle outline
void graphics_draw_rect(int x, int y, int width, int height, uint8_t color) {
    // Top and bottom lines
    for (int i = 0; i < width; i++) {
        graphics_putpixel(x + i, y, color);
        graphics_putpixel(x + i, y + height - 1, color);
    }
    
    // Left and right lines
    for (int i = 0; i < height; i++) {
        graphics_putpixel(x, y + i, color);
        graphics_putpixel(x + width - 1, y + i, color);
    }
}

// Draw a filled rectangle
void graphics_fill_rect(int x, int y, int width, int height, uint8_t color) {
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            graphics_putpixel(x + i, y + j, color);
        }
    }
}

// Draw a character
void graphics_draw_char(int x, int y, char c, uint8_t color) {
    int index = 0;
    
    // Map character to font index
    if (c == ' ') {
        index = 0;
    } else if (c >= '0' && c <= '9') {
        index = 1 + (c - '0');  // 0-9 maps to indices 1-10
    } else if (c >= 'A' && c <= 'Z') {
        index = 11 + (c - 'A'); // A-Z maps to indices 11-36
    } else if (c >= 'a' && c <= 'z') {
        index = 11 + (c - 'a'); // a-z also maps to A-Z (uppercase)
    } else {
        index = 0; // Default to space for unsupported chars
    }
    
    for (int row = 0; row < 8; row++) {
        uint8_t line = font_8x8[index][row];
        for (int col = 0; col < 8; col++) {
            if (line & (1 << (7 - col))) {
                graphics_putpixel(x + col, y + row, color);
            }
        }
    }
}

// Draw a string
void graphics_draw_string(int x, int y, const char* str, uint8_t color) {
    int offset = 0;
    while (*str) {
        graphics_draw_char(x + offset, y, *str, color);
        offset += 8;
        str++;
    }
}
