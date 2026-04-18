// Simple Shell Implementation
// Command parser and built-in commands

#include "shell.h"
#include "keyboard.h"
#include "pmm.h"
#include "fs.h"
#include "scheduler.h"
#include "graphics.h"
#include "timer.h"

// External functions
extern void print(const char* str);
extern void putchar(char c);
extern void print_hex(unsigned int num);
extern void print_dec(unsigned int num);
extern void clear_screen();

// Shell state
static char input_buffer[SHELL_BUFFER_SIZE];
static int buffer_pos = 0;

// I/O helper
static inline void outb(unsigned short port, unsigned char value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

// String utility functions
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

// Print shell prompt
static void print_prompt() {
    print(SHELL_PROMPT);
}

// ====== Built-in Commands ======

// Command: help
static void cmd_reboot(); // Forward declaration

static void cmd_help() {
    print("\nAvailable commands:\n");
    print("  help      - Show this help message\n");
    print("  clear     - Clear the screen\n");
    print("  version   - Show OS version\n");
    print("  uptime    - Show system uptime\n");
    print("  meminfo   - Display memory information\n");
    print("  echo      - Echo text to screen\n");
    print("  ls        - List files\n");
    print("  touch     - Create a file (touch <name> <content>)\n");
    print("  cat       - Read a file (cat <name>)\n");
    print("  rm        - Delete a file (rm <name>)\n");
    print("  tasks     - Show running tasks\n");
    print("  gfx       - Graphics mode demo\n");
    print("  reboot    - Reboot the system\n");
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

// Command: uptime
static void cmd_uptime() {
    uint32_t ticks = timer_get_ticks();
    uint32_t total_seconds = ticks / 100;
    uint32_t minutes = total_seconds / 60;
    uint32_t seconds = total_seconds % 60;
    
    print("\nUptime: ");
    print_dec(minutes);
    print("m ");
    print_dec(seconds);
    print("s (");
    print_dec(ticks);
    print(" ticks)\n\n");
}

// Command: ls (list files)
static void cmd_ls() {
    fs_list();
    print("\n");
}

// Command: touch (create file)
static void cmd_touch(const char* args) {
    char filename[32];
    int i = 0;
    const char* ptr = args;
    
    // Parse filename (first word)
    while (*ptr && *ptr != ' ' && i < 31) {
        filename[i++] = *ptr++;
    }
    filename[i] = '\0';
    
    if (i == 0) {
        print("\nUsage: touch <filename> [content]\n\n");
        return;
    }
    
    // Skip space to get content
    if (*ptr == ' ') ptr++;
    
    if (fs_create(filename, ptr) == 0) {
        print("\nCreated: ");
        print(filename);
        print("\n\n");
    }
}

// Command: cat (read file)
static void cmd_cat(const char* args) {
    char buffer[1024];
    
    if (*args == '\0') {
        print("\nUsage: cat <filename>\n\n");
        return;
    }
    
    if (fs_read(args, buffer, sizeof(buffer)) >= 0) {
        print("\n");
        print(buffer);
        print("\n\n");
    }
}

// Command: rm (delete file)
static void cmd_rm(const char* args) {
    if (*args == '\0') {
        print("\nUsage: rm <filename>\n\n");
        return;
    }
    
    if (fs_delete(args) == 0) {
        print("\nDeleted: ");
        print(args);
        print("\n\n");
    }
}

// Command: tasks
static void cmd_tasks() {
    scheduler_print_tasks();
}

// Command: gfx (graphics demo)
static void cmd_gfx() {
    // Switch to VGA Mode 13h (320x200, 256 colors)
    graphics_set_mode_13h();
    graphics_clear(COLOR_BLACK);
    
    // Draw border
    graphics_draw_rect(0, 0, 319, 199, COLOR_WHITE);
    
    // Title
    graphics_draw_string(96, 10, "COREX OS", COLOR_LIGHT_CYAN);
    graphics_draw_string(72, 25, "GRAPHICS DEMO", COLOR_YELLOW);
    
    // Divider line
    graphics_draw_line(20, 40, 300, 40, COLOR_DARK_GRAY);
    
    // Colored rectangles
    graphics_fill_rect(30, 55, 50, 35, COLOR_RED);
    graphics_fill_rect(95, 55, 50, 35, COLOR_GREEN);
    graphics_fill_rect(160, 55, 50, 35, COLOR_BLUE);
    graphics_fill_rect(225, 55, 50, 35, COLOR_YELLOW);
    
    // Labels
    graphics_draw_string(45, 95, "RED", COLOR_LIGHT_RED);
    graphics_draw_string(105, 95, "GREEN", COLOR_LIGHT_GREEN);
    graphics_draw_string(170, 95, "BLUE", COLOR_LIGHT_BLUE);
    graphics_draw_string(235, 95, "YELLOW", COLOR_YELLOW);
    
    // Diagonal lines demo
    graphics_draw_line(30, 115, 290, 115, COLOR_WHITE);
    graphics_draw_line(30, 120, 290, 145, COLOR_LIGHT_MAGENTA);
    graphics_draw_line(30, 145, 290, 120, COLOR_LIGHT_CYAN);
    
    // Footer
    graphics_draw_string(48, 175, "PRESS ANY KEY TO REBOOT", COLOR_LIGHT_GRAY);
    
    // Wait for keypress
    while (!keyboard_available()) {
        __asm__ __volatile__("hlt");
    }
    keyboard_getchar();  // Consume the key
    
    // Note: Returning to text mode requires reloading the VGA fonts into Plane 2,
    // which were destroyed by Mode 13h. To keep this simple, we just reboot safely.
    cmd_reboot();
}

// Command: reboot
static void cmd_reboot() {
    print("\nRebooting...\n");
    // Pulse keyboard controller reset line
    outb(0x64, 0xFE);
    // If that fails, halt
    while(1) {
        __asm__ __volatile__("cli; hlt");
    }
}

// ====== Shell Core ======

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
    
    // Parse command
    if (strcmp(command, "help") == 0) {
        cmd_help();
        
    } else if (strcmp(command, "clear") == 0) {
        cmd_clear();
        
    } else if (strcmp(command, "meminfo") == 0) {
        cmd_meminfo();
        
    } else if (strcmp(command, "version") == 0) {
        cmd_version();
        
    } else if (strcmp(command, "uptime") == 0) {
        cmd_uptime();
        
    } else if (strcmp(command, "ls") == 0) {
        cmd_ls();
        
    } else if (strcmp(command, "tasks") == 0) {
        cmd_tasks();
        
    } else if (strcmp(command, "gfx") == 0) {
        cmd_gfx();
        
    } else if (strcmp(command, "reboot") == 0) {
        cmd_reboot();
        
    } else if (strncmp(command, "echo ", 5) == 0) {
        cmd_echo(command + 5);
        
    } else if (strcmp(command, "echo") == 0) {
        print("\n\n");
        
    } else if (strncmp(command, "touch ", 6) == 0) {
        cmd_touch(command + 6);
        
    } else if (strncmp(command, "cat ", 4) == 0) {
        cmd_cat(command + 4);
        
    } else if (strncmp(command, "rm ", 3) == 0) {
        cmd_rm(command + 3);
        
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
        // Get character from keyboard buffer
        char c = keyboard_getchar();
        if (c != 0) {
            // Echo and handle the character
            shell_handle_input(c);
        }
        
        // Wait for next interrupt
        __asm__ __volatile__("hlt");
    }
}
