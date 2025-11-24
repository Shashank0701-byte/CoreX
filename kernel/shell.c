// Simple Shell Implementation
// Command parser and built-in commands

#include "shell.h"
#include "keyboard.h"
#include "pmm.h"

// External functions
extern void print(const char* str);
extern void putchar(char c);
extern void print_hex(unsigned int num);
extern void clear_screen();

// Shell state
static char input_buffer[SHELL_BUFFER_SIZE];
static int buffer_pos = 0;

// String utility functions
static int strlen(const char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

static int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

static int strncmp(const char* s1, const char* s2, int n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

static void strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

// Print shell prompt
static void print_prompt() {
    print(SHELL_PROMPT);
}

// Command: help
static void cmd_help() {
    print("\nAvailable commands:\n");
    print("  help      - Show this help message\n");
    print("  clear     - Clear the screen\n");
    print("  meminfo   - Display memory information\n");
    print("  echo      - Echo text to screen\n");
    print("  version   - Show OS version\n");
    print("\n");
}

// Command: clear
static void cmd_clear() {
    clear_screen();
}

// Command: meminfo
static void cmd_meminfo() {
    print("\nMemory Information:\n");
    print("  Total pages:  ");
    print_hex(pmm_get_total_pages());
    print("\n");
    
    print("  Used pages:   ");
    print_hex(pmm_get_used_pages());
    print("\n");
    
    print("  Free pages:   ");
    print_hex(pmm_get_free_pages());
    print("\n");
    
    print("  Page size:    4096 bytes (4 KB)\n");
    
    uint32_t total_mem = pmm_get_total_pages() * 4096;
    uint32_t free_mem = pmm_get_free_pages() * 4096;
    
    print("  Total memory: ");
    print_hex(total_mem);
    print(" bytes\n");
    
    print("  Free memory:  ");
    print_hex(free_mem);
    print(" bytes\n\n");
}

// Command: echo
static void cmd_echo(const char* args) {
    print("\n");
    print(args);
    print("\n\n");
}

// Command: version
static void cmd_version() {
    print("\nCoreX OS v3.2\n");
    print("A simple x86 operating system\n");
    print("Built from scratch in C and Assembly\n\n");
}

// Initialize shell
void shell_init() {
    buffer_pos = 0;
    input_buffer[0] = '\0';
    
    print("\n");
    print("========================================\n");
    print("  Welcome to CoreX OS Shell v1.0\n");
    print("========================================\n");
    print("\nType 'help' for available commands\n\n");
    
    print_prompt();
}

// Handle keyboard input
void shell_handle_input(char c) {
    if (c == '\n') {
        // Enter pressed - execute command
        print("\n");
        input_buffer[buffer_pos] = '\0';
        
        if (buffer_pos > 0) {
            shell_execute(input_buffer);
        }
        
        // Reset buffer
        buffer_pos = 0;
        input_buffer[0] = '\0';
        print_prompt();
        
    } else if (c == '\b') {
        // Backspace
        if (buffer_pos > 0) {
            buffer_pos--;
            input_buffer[buffer_pos] = '\0';
            putchar('\b');  // Visual backspace
        }
        
    } else {
        // Regular character
        if (buffer_pos < SHELL_BUFFER_SIZE - 1) {
            input_buffer[buffer_pos++] = c;
            input_buffer[buffer_pos] = '\0';
            putchar(c);  // Echo character
        }
    }
}

// Execute command
void shell_execute(const char* command) {
    // Skip leading spaces
    while (*command == ' ') command++;
    
    // Empty command
    if (*command == '\0') {
        return;
    }
    
    // Parse command and arguments
    if (strcmp(command, "help") == 0) {
        cmd_help();
        
    } else if (strcmp(command, "clear") == 0) {
        cmd_clear();
        print_prompt();
        
    } else if (strcmp(command, "meminfo") == 0) {
        cmd_meminfo();
        
    } else if (strcmp(command, "version") == 0) {
        cmd_version();
        
    } else if (strncmp(command, "echo ", 5) == 0) {
        // Echo command with arguments
        cmd_echo(command + 5);
        
    } else if (strcmp(command, "echo") == 0) {
        // Echo without arguments
        print("\n\n");
        
    } else {
        // Unknown command
        print("\nUnknown command: ");
        print(command);
        print("\nType 'help' for available commands\n\n");
    }
}

// Run shell (main loop)
void shell_run() {
    while (1) {
        // Check if keyboard input is available
        if (keyboard_available()) {
            char c = keyboard_getchar();
            if (c != 0) {
                shell_handle_input(c);
            }
        }
        
        // Halt CPU until next interrupt
        __asm__ __volatile__("hlt");
    }
}
