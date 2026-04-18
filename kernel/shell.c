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
extern void print_colored(const char* str, unsigned char color);
extern void put_char_at(int x, int y, char c, unsigned char color);
extern uint32_t timer_get_ticks();

// Shell state
static char input_buffer[SHELL_BUFFER_SIZE];
static int buffer_pos = 0;

// Command history
#define HISTORY_MAX 8
static char history[HISTORY_MAX][SHELL_BUFFER_SIZE];
static int history_count = 0;
static int history_index = 0;

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

static void strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

static int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

// Print shell prompt
static void print_prompt() {
    print_colored(SHELL_PROMPT, 0x0A); // Light Green
}

// ====== Built-in Commands ======

// Command: help
static void cmd_reboot(); // Forward declaration

static void cmd_help() {
    print("\nAvailable commands:\n");
    print("  help      - Show this help message\n");
    print("  clear     - Clear the screen\n");
    print("  version   - Show OS version\n");
    print("  sysinfo   - Show system information\n");
    print("  uptime    - Show system uptime\n");
    print("  meminfo   - Display memory information\n");
    print("  calc      - Simple calculator (calc <num> <op> <num>)\n");
    print("  echo      - Echo text to screen\n");
    print("  ls        - List files\n");
    print("  touch     - Create a file (touch <name> <content>)\n");
    print("  cat       - Read a file (cat <name>)\n");
    print("  rm        - Delete a file (rm <name>)\n");
    print("  edit      - Simple Text Editor (edit <name>)\n");
    print("  snake     - Play the Snake Game!\n");
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

// Command: sysinfo
static void cmd_sysinfo() {
    print("\n");
    print_colored("--- System Information ---\n", 0x0B);
    print("OS Version:    CoreX OS v3.2\n");
    print("CPU Mode:      32-bit Protected Mode\n");
    print("Memory:        ");
    print_dec(pmm_get_total_pages() * 4);
    print(" KB Total, ");
    print_dec(pmm_get_free_pages() * 4);
    print(" KB Free\n");
    print("Timer Freq:    100 Hz\n");
    print("Scheduler:     Cooperative (Round-Robin)\n");
    print("Filesystem:    In-Memory Array\n");
    print("--------------------------\n\n");
}

// Command: calc
static void cmd_calc(const char* args) {
    int num1 = 0, num2 = 0;
    char op = 0;
    const char* ptr = args;
    
    // Skip spaces
    while (*ptr == ' ') ptr++;
    
    // Parse first number
    while (*ptr >= '0' && *ptr <= '9') {
        num1 = num1 * 10 + (*ptr - '0');
        ptr++;
    }
    
    // Skip spaces
    while (*ptr == ' ') ptr++;
    
    // Get operator
    if (*ptr == '+' || *ptr == '-' || *ptr == '*') {
        op = *ptr++;
    } else {
        print_colored("\nError: Invalid operator. Use +, -, or *\n\n", 0x0C);
        return;
    }
    
    // Skip spaces
    while (*ptr == ' ') ptr++;
    
    // Parse second number
    if (!(*ptr >= '0' && *ptr <= '9')) {
        print_colored("\nError: Missing second number\n\n", 0x0C);
        return;
    }
    
    while (*ptr >= '0' && *ptr <= '9') {
        num2 = num2 * 10 + (*ptr - '0');
        ptr++;
    }
    
    print("\nResult: ");
    if (op == '+') {
        print_dec(num1 + num2);
    } else if (op == '-') {
        // Handle negative result slightly gracefully by preventing it for now
        if (num1 < num2) {
            print("-");
            print_dec(num2 - num1);
        } else {
            print_dec(num1 - num2);
        }
    } else if (op == '*') {
        print_dec(num1 * num2);
    }
    print("\n\n");
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

// Command: edit (text editor)
static void cmd_edit(const char* args) {
    char buffer[1024];
    int len = 0;
    
    if (*args == '\0') {
        print("\nUsage: edit <filename>\n\n");
        return;
    }
    
    // Try to load existing file
    if (fs_read(args, buffer, sizeof(buffer) - 1) >= 0) {
        while (buffer[len] != '\0' && len < 1023) {
            len++;
        }
    } else {
        buffer[0] = '\0';
    }
    
    while (1) {
        clear_screen();
        
        // Draw header
        print_colored("--- CoreX Editor --- File: ", 0x0B);
        print_colored(args, 0x0E);
        print_colored(" --- [Press ESC to Save & Exit] ---\n\n", 0x0B);
        
        // Print file content
        print(buffer);
        
        // Print cursor block
        print_colored("_", 0x0A);
        
        // Wait for key
        while (!keyboard_available()) {
            __asm__ __volatile__("hlt");
        }
        
        char c = keyboard_getchar();
        
        if (c == 27) { // ESC key
            break;
        } else if (c == '\b') {
            if (len > 0) {
                len--;
                buffer[len] = '\0';
            }
        } else if (c >= 32 || c == '\n') { // Printable char or newline
            if (len < 1022) {
                buffer[len] = c;
                len++;
                buffer[len] = '\0';
            }
        }
    }
    
    // Save file
    fs_delete(args); // Delete if it already exists
    
    clear_screen();
    if (fs_create(args, buffer) == 0) {
        print("File saved successfully.\n\n");
    } else {
        print_colored("Error: Failed to save file.\n\n", 0x0C);
    }
}

// Command: snake (Snake Game)
static void cmd_snake() {
    int snake_x[200], snake_y[200];
    int snake_len = 4;
    int dir_x = 1, dir_y = 0; // Start moving right
    int food_x = 40, food_y = 12;
    int game_over = 0;
    int score = 0;
    
    // Initial snake
    for (int i = 0; i < snake_len; i++) {
        snake_x[i] = 40 - i;
        snake_y[i] = 12;
    }
    
    clear_screen();
    uint32_t last_tick = timer_get_ticks();
    
    while (!game_over) {
        // Render step
        clear_screen();
        
        // Draw borders
        for (int x = 0; x < 80; x++) {
            put_char_at(x, 0, '#', 0x07);
            put_char_at(x, 24, '#', 0x07);
        }
        for (int y = 0; y < 25; y++) {
            put_char_at(0, y, '#', 0x07);
            put_char_at(79, y, '#', 0x07);
        }
        
        // Draw food (Red 'O')
        put_char_at(food_x, food_y, 'O', 0x0C);
        
        // Draw snake
        put_char_at(snake_x[0], snake_y[0], '@', 0x0A); // Head (Green)
        for (int i = 1; i < snake_len; i++) {
            put_char_at(snake_x[i], snake_y[i], '*', 0x02); // Body (Dark Green)
        }
        
        // Draw score
        print_colored(" Score: ", 0x0E);
        print_dec(score);
        print(" ");
        
        // Update logic (wait for tick to control speed)
        while (timer_get_ticks() - last_tick < 8) {
            // Process input while waiting
            if (keyboard_available()) {
                char c = keyboard_getchar();
                if (c == 27) { game_over = 1; break; } // ESC
                else if (c == 17 && dir_y == 0) { dir_x = 0; dir_y = -1; } // Up
                else if (c == 18 && dir_y == 0) { dir_x = 0; dir_y = 1; }  // Down
                else if (c == 19 && dir_x == 0) { dir_x = 1; dir_y = 0; }  // Right
                else if (c == 20 && dir_x == 0) { dir_x = -1; dir_y = 0; } // Left
            }
        }
        last_tick = timer_get_ticks();
        if (game_over) break;
        
        // Calculate next head position
        int next_x = snake_x[0] + dir_x;
        int next_y = snake_y[0] + dir_y;
        
        // Check wall collision
        if (next_x <= 0 || next_x >= 79 || next_y <= 0 || next_y >= 24) {
            game_over = 1;
        }
        
        // Check self collision
        for (int i = 0; i < snake_len; i++) {
            if (snake_x[i] == next_x && snake_y[i] == next_y) {
                game_over = 1;
            }
        }
        
        if (game_over) break;
        
        // Move snake
        for (int i = snake_len - 1; i > 0; i--) {
            snake_x[i] = snake_x[i-1];
            snake_y[i] = snake_y[i-1];
        }
        snake_x[0] = next_x;
        snake_y[0] = next_y;
        
        // Check food collision
        if (next_x == food_x && next_y == food_y) {
            score++;
            if (snake_len < 199) snake_len++;
            
            // Basic pseudo-random food placement
            food_x = 1 + (timer_get_ticks() + score * 7) % 78;
            food_y = 1 + (timer_get_ticks() + score * 11) % 23;
        }
    }
    
    clear_screen();
    print_colored("\n--- GAME OVER ---\n", 0x0C);
    print("Final Score: ");
    print_dec(score);
    print("\n\n");
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
    print_colored("========================================\n", 0x0B);
    print_colored("  Welcome to CoreX OS Shell v1.0\n", 0x0E); // Yellow
    print_colored("========================================\n", 0x0B);
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
            // Save to history if different from last command
            if (history_count == 0 || strcmp(input_buffer, history[history_count - 1]) != 0) {
                if (history_count < HISTORY_MAX) {
                    strcpy(history[history_count], input_buffer);
                    history_count++;
                } else {
                    for (int i = 0; i < HISTORY_MAX - 1; i++) {
                        strcpy(history[i], history[i+1]);
                    }
                    strcpy(history[HISTORY_MAX - 1], input_buffer);
                }
            }
            history_index = history_count;
            
            shell_execute(input_buffer);
        }
        
        // Reset buffer
        buffer_pos = 0;
        input_buffer[0] = '\0';
        print_prompt();
        
    } else if (c == 17) {
        // Up arrow - previous command
        if (history_count > 0 && history_index > 0) {
            history_index--;
            // Clear current input from screen
            for (int i = 0; i < buffer_pos; i++) {
                putchar('\b');
            }
            // Load history
            strcpy(input_buffer, history[history_index]);
            buffer_pos = strlen(input_buffer);
            print(input_buffer);
        }
    } else if (c == 18) {
        // Down arrow - next command
        if (history_count > 0 && history_index < history_count) {
            history_index++;
            // Clear current input from screen
            for (int i = 0; i < buffer_pos; i++) {
                putchar('\b');
            }
            if (history_index == history_count) {
                // Return to empty prompt
                input_buffer[0] = '\0';
                buffer_pos = 0;
            } else {
                // Load history
                strcpy(input_buffer, history[history_index]);
                buffer_pos = strlen(input_buffer);
                print(input_buffer);
            }
        }
    } else if (c == '\b') {
        // Backspace
        if (buffer_pos > 0) {
            buffer_pos--;
            input_buffer[buffer_pos] = '\0';
            putchar('\b');  // Visual backspace
        }
        
    } else if (c >= 32 && c <= 126) {
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
        
    } else if (strcmp(command, "sysinfo") == 0) {
        cmd_sysinfo();
        
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
        
    } else if (strncmp(command, "calc ", 5) == 0) {
        cmd_calc(command + 5);
        
    } else if (strncmp(command, "touch ", 6) == 0) {
        cmd_touch(command + 6);
        
    } else if (strncmp(command, "cat ", 4) == 0) {
        cmd_cat(command + 4);
        
    } else if (strncmp(command, "rm ", 3) == 0) {
        cmd_rm(command + 3);
        
    } else if (strncmp(command, "edit ", 5) == 0) {
        cmd_edit(command + 5);
        
    } else if (strcmp(command, "snake") == 0) {
        cmd_snake();
        
    } else {
        // Unknown command
        print_colored("\nUnknown command: ", 0x0C); // Light Red
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
