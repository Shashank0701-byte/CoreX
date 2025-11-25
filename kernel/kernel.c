// CoreX Kernel - Main Entry Point
// Minimal C kernel with VGA text mode output

#include "idt.h"
#include "pic.h"
#include "timer.h"
#include "pmm.h"
#include "paging.h"
#include "keyboard.h"
#include "shell.h"
#include "scheduler.h"
#include "fs.h"

// VGA text mode constants
#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define WHITE_ON_BLACK 0x0F

// Global variables
static unsigned short* vga_buffer = (unsigned short*)VGA_MEMORY;
static unsigned int cursor_x = 0;
static unsigned int cursor_y = 0;

// Port I/O functions
static inline void outb(unsigned short port, unsigned char value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Function: update_cursor
// Updates the VGA hardware cursor position
void update_cursor() {
    unsigned short position = cursor_y * VGA_WIDTH + cursor_x;
    
    // Cursor LOW port to VGA INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position & 0xFF));
    // Cursor HIGH port to VGA INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((position >> 8) & 0xFF));
}

// Function: clear_screen
// Clears the VGA text mode screen
void clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (WHITE_ON_BLACK << 8) | ' ';
    }
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

// Function: putchar
// Prints a single character to the screen
void putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\b') {
        // Backspace
        if (cursor_x > 0) {
            cursor_x--;
            unsigned int offset = cursor_y * VGA_WIDTH + cursor_x;
            vga_buffer[offset] = (WHITE_ON_BLACK << 8) | ' ';
        }
    } else {
        unsigned int offset = cursor_y * VGA_WIDTH + cursor_x;
        vga_buffer[offset] = (WHITE_ON_BLACK << 8) | c;
        cursor_x++;
        
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }
    
    // Scroll if needed
    if (cursor_y >= VGA_HEIGHT) {
        cursor_y = VGA_HEIGHT - 1;
        // Simple scroll: move everything up one line
        for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
            vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
        }
        // Clear last line
        for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
            vga_buffer[i] = (WHITE_ON_BLACK << 8) | ' ';
        }
    }
    
    // Update hardware cursor - DISABLED FOR TESTING
    // update_cursor();
}

// Function: print
// Prints a null-terminated string to the screen
void print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        putchar(str[i]);
    }
}

// Function: print_hex
// Prints a hexadecimal number
void print_hex(unsigned int num) {
    char hex_chars[] = "0123456789ABCDEF";
    print("0x");
    
    for (int i = 28; i >= 0; i -= 4) {
        putchar(hex_chars[(num >> i) & 0xF]);
    }
}

// Kernel main entry point
void kmain() {
    // Clear the screen
    clear_screen();
    
    // Print welcome message
    print("CoreX OS v3.0\n\n");
    
    // Initialize IDT
    idt_init();
    
    // Initialize PIC
    pic_init();
    
    // Initialize keyboard
    keyboard_init();
    
    // Enable interrupts
    __asm__ __volatile__("sti");
    
    // Initialize and run shell
    shell_init();
    shell_run();
    
    // Should never reach here
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
