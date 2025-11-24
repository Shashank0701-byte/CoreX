// PS/2 Keyboard Driver Header
// Reads scancodes and converts to ASCII

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

// Keyboard ports
#define KEYBOARD_DATA_PORT      0x60
#define KEYBOARD_STATUS_PORT    0x64
#define KEYBOARD_COMMAND_PORT   0x64

// Special keys
#define KEY_ESCAPE      0x01
#define KEY_BACKSPACE   0x0E
#define KEY_TAB         0x0F
#define KEY_ENTER       0x1C
#define KEY_LCTRL       0x1D
#define KEY_LSHIFT      0x2A
#define KEY_RSHIFT      0x36
#define KEY_LALT        0x38
#define KEY_CAPSLOCK    0x3A
#define KEY_F1          0x3B
#define KEY_F2          0x3C
#define KEY_F3          0x3D
#define KEY_F4          0x3E
#define KEY_F5          0x3F
#define KEY_F6          0x40
#define KEY_F7          0x41
#define KEY_F8          0x42
#define KEY_F9          0x43
#define KEY_F10         0x44

// Initialize keyboard driver
void keyboard_init();

// Keyboard interrupt handler (called from IRQ1)
void keyboard_handler();

// Get last pressed key (ASCII)
char keyboard_getchar();

// Check if a key is available
int keyboard_available();

#endif // KEYBOARD_H
