// CoreX Kernel - Main Entry Point
// Minimal C kernel with VGA text mode output

#include "idt.h"

// VGA text mode constants
#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define WHITE_ON_BLACK 0x0F

// Global variables
static unsigned short* vga_buffer = (unsigned short*)VGA_MEMORY;
static unsigned int cursor_x = 0;
static unsigned int cursor_y = 0;

// Function: clear_screen
// Clears the VGA text mode screen
void clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (WHITE_ON_BLACK << 8) | ' ';
    }
    cursor_x = 0;
    cursor_y = 0;
}

// Function: putchar
// Prints a single character to the screen
void putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
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
    print("=================================\n");
    print("    CoreX Kernel v3.1 (C)\n");
    print("=================================\n\n");
    
    print("Running in 32-bit Protected Mode\n");
    print("GDT loaded successfully\n");
    print("A20 line enabled\n");
    print("VGA text mode initialized\n\n");
    
    // Initialize IDT
    print("Initializing IDT...\n");
    idt_init();
    print("\n");
    
    print("Kernel features:\n");
    print("  - C language kernel\n");
    print("  - VGA text output\n");
    print("  - Screen scrolling support\n");
    print("  - IDT with exception handlers\n\n");
    
    print("System initialized successfully!\n\n");
    
    // Test exception handling (uncomment to test)
    // print("Testing divide by zero exception...\n");
    // int x = 1 / 0;  // This will trigger exception 0
    
    // print("Testing invalid opcode exception...\n");
    // __asm__ __volatile__("ud2");  // This will trigger exception 6
    
    print("Kernel running. System stable.\n");
    
    // Halt the CPU
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
