// PS/2 Keyboard Driver Implementation
// Scancode to ASCII conversion with shift support

#include "keyboard.h"
#include "pic.h"

// External print functions
extern void print(const char* str);
extern void putchar(char c);

// Keyboard state
static int shift_pressed = 0;
static int caps_lock = 0;
static int ctrl_pressed = 0;
static int alt_pressed = 0;

// Circular buffer for keyboard input
#define KEYBOARD_BUFFER_SIZE 256
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static int buffer_read = 0;
static int buffer_write = 0;

// I/O port operations
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// US QWERTY scancode to ASCII table (without shift)
static const char scancode_to_ascii[] = {
    0,   0,   '1', '2', '3', '4', '5', '6',     // 0x00-0x07
    '7', '8', '9', '0', '-', '=', '\b', '\t',   // 0x08-0x0F
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',     // 0x10-0x17
    'o', 'p', '[', ']', '\n', 0,   'a', 's',    // 0x18-0x1F
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     // 0x20-0x27
    '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',   // 0x28-0x2F
    'b', 'n', 'm', ',', '.', '/', 0,   '*',     // 0x30-0x37
    0,   ' ', 0,   0,   0,   0,   0,   0,       // 0x38-0x3F
    0,   0,   0,   0,   0,   0,   0,   '7',     // 0x40-0x47
    '8', '9', '-', '4', '5', '6', '+', '1',     // 0x48-0x4F
    '2', '3', '0', '.'                          // 0x50-0x53
};

// US QWERTY scancode to ASCII table (with shift)
static const char scancode_to_ascii_shift[] = {
    0,   0,   '!', '@', '#', '$', '%', '^',     // 0x00-0x07
    '&', '*', '(', ')', '_', '+', '\b', '\t',   // 0x08-0x0F
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',     // 0x10-0x17
    'O', 'P', '{', '}', '\n', 0,   'A', 'S',    // 0x18-0x1F
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',     // 0x20-0x27
    '"', '~', 0,   '|', 'Z', 'X', 'C', 'V',     // 0x28-0x2F
    'B', 'N', 'M', '<', '>', '?', 0,   '*',     // 0x30-0x37
    0,   ' ', 0,   0,   0,   0,   0,   0,       // 0x38-0x3F
    0,   0,   0,   0,   0,   0,   0,   '7',     // 0x40-0x47
    '8', '9', '-', '4', '5', '6', '+', '1',     // 0x48-0x4F
    '2', '3', '0', '.'                          // 0x50-0x53
};

// Initialize keyboard
void keyboard_init() {
    buffer_read = 0;
    buffer_write = 0;
    shift_pressed = 0;
    caps_lock = 0;
    ctrl_pressed = 0;
    alt_pressed = 0;
    
    print("Keyboard driver initialized\n");
}

// Add character to buffer
static void keyboard_buffer_add(char c) {
    int next = (buffer_write + 1) % KEYBOARD_BUFFER_SIZE;
    if (next != buffer_read) {
        keyboard_buffer[buffer_write] = c;
        buffer_write = next;
    }
}

// Keyboard interrupt handler - USE BUFFER
void keyboard_handler() {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Only handle key presses (not releases)
    if (!(scancode & 0x80)) {
        // Convert scancode to ASCII (simple version)
        if (scancode < sizeof(scancode_to_ascii)) {
            char ascii = scancode_to_ascii[scancode];
            if (ascii != 0) {
                // Add to buffer (NOT direct putchar!)
                keyboard_buffer_add(ascii);
            }
        }
    }
    
    // Send EOI to PIC
    pic_send_eoi(1);
}

// Get character from buffer (non-blocking)
char keyboard_getchar() {
    if (buffer_read == buffer_write) {
        return 0;  // No character available
    }
    
    char c = keyboard_buffer[buffer_read];
    buffer_read = (buffer_read + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

// Check if character is available
int keyboard_available() {
    return buffer_read != buffer_write;
}
