#include "shell.h"
#include "keyboard.h"
#include "timer.h"

extern void clear_screen();
extern void print_colored(const char* str, unsigned char color);
extern void put_char_at(int x, int y, char c, unsigned char color);
extern void print_dec(unsigned int num);
extern void print(const char* str);
extern uint32_t timer_get_ticks();

static const unsigned short tetrominoes[7][4] = {
    // I
    {0x0F00, 0x2222, 0x00F0, 0x4444},
    // J
    {0x44C0, 0x8E00, 0x6440, 0x0E20},
    // L
    {0x4460, 0x0E80, 0xC440, 0x2E00},
    // O
    {0xCC00, 0xCC00, 0xCC00, 0xCC00},
    // S
    {0x06C0, 0x8C40, 0x06C0, 0x8C40},
    // T
    {0x0E40, 0x4C40, 0x4E00, 0x4640},
    // Z
    {0x0C60, 0x4C80, 0x0C60, 0x4C80}
};

static const unsigned char tetris_colors[7] = {
    0x0B, // Light Cyan (I)
    0x09, // Light Blue (J)
    0x06, // Brown/Orange (L)
    0x0E, // Yellow (O)
    0x0A, // Light Green (S)
    0x0D, // Light Magenta (T)
    0x0C  // Light Red (Z)
};

static int check_collision(int grid[20][10], int shape, int rot, int px, int py) {
    unsigned short bits = tetrominoes[shape][rot];
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (bits & (1 << (15 - (y * 4 + x)))) {
                int nx = px + x;
                int ny = py + y;
                if (nx < 0 || nx >= 10 || ny >= 20) return 1;
                if (ny >= 0 && grid[ny][nx]) return 1;
            }
        }
    }
    return 0;
}

static void lock_piece(int grid[20][10], int shape, int rot, int px, int py) {
    unsigned short bits = tetrominoes[shape][rot];
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (bits & (1 << (15 - (y * 4 + x)))) {
                if (py + y >= 0 && py + y < 20) {
                    grid[py + y][px + x] = tetris_colors[shape];
                }
            }
        }
    }
}

void cmd_tetris() {
    int grid[20][10];
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 10; x++) {
            grid[y][x] = 0;
        }
    }
    
    int score = 0;
    int game_over = 0;
    
    int current_shape = (timer_get_ticks() % 7);
    int current_rot = 0;
    int current_x = 3;
    int current_y = -2;
    
    uint32_t last_tick = timer_get_ticks();
    uint32_t fall_delay = 50; // 50 ticks = 0.5s initially
    
    clear_screen();
    
    while (!game_over) {
        // Render step
        clear_screen();
        
        // Draw board
        for (int y = 0; y < 20; y++) {
            put_char_at(29, y + 2, '<', 0x07);
            put_char_at(50, y + 2, '>', 0x07);
            for (int x = 0; x < 10; x++) {
                if (grid[y][x]) {
                    put_char_at(30 + x*2, y + 2, '[', grid[y][x]);
                    put_char_at(31 + x*2, y + 2, ']', grid[y][x]);
                } else {
                    put_char_at(30 + x*2, y + 2, '.', 0x08);
                    put_char_at(31 + x*2, y + 2, ' ', 0x08);
                }
            }
        }
        for (int x = 0; x < 22; x++) {
            put_char_at(29 + x, 22, '=', 0x07);
        }
        
        // Draw current piece
        unsigned short bits = tetrominoes[current_shape][current_rot];
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (bits & (1 << (15 - (y * 4 + x)))) {
                    if (current_y + y >= 0 && current_y + y < 20) {
                        put_char_at(30 + (current_x + x)*2, current_y + y + 2, '[', tetris_colors[current_shape]);
                        put_char_at(31 + (current_x + x)*2, current_y + y + 2, ']', tetris_colors[current_shape]);
                    }
                }
            }
        }
        
        // Draw UI
        print_colored(" TETRIS ", 0x0E);
        print("\n\n Score: ");
        print_dec(score);
        
        // Input and timing
        while (timer_get_ticks() - last_tick < fall_delay) {
            if (keyboard_available()) {
                char c = keyboard_getchar();
                if (c == 27) { game_over = 1; break; } // ESC
                else if (c == 17) { // Up (Rotate)
                    int next_rot = (current_rot + 1) % 4;
                    if (!check_collision(grid, current_shape, next_rot, current_x, current_y)) {
                        current_rot = next_rot;
                    }
                }
                else if (c == 20) { // Left
                    if (!check_collision(grid, current_shape, current_rot, current_x - 1, current_y)) {
                        current_x--;
                    }
                }
                else if (c == 19) { // Right
                    if (!check_collision(grid, current_shape, current_rot, current_x + 1, current_y)) {
                        current_x++;
                    }
                }
                else if (c == 18) { // Down (Soft Drop)
                    if (!check_collision(grid, current_shape, current_rot, current_x, current_y + 1)) {
                        current_y++;
                    }
                }
                else if (c == ' ') { // Space (Hard Drop)
                    while (!check_collision(grid, current_shape, current_rot, current_x, current_y + 1)) {
                        current_y++;
                    }
                    last_tick = 0; // Force immediate lock
                }
            }
        }
        
        if (game_over) break;
        
        // Gravity
        if (timer_get_ticks() - last_tick >= fall_delay) {
            if (!check_collision(grid, current_shape, current_rot, current_x, current_y + 1)) {
                current_y++;
            } else {
                // Lock piece
                lock_piece(grid, current_shape, current_rot, current_x, current_y);
                
                // Clear lines
                int lines_cleared = 0;
                for (int y = 19; y >= 0; y--) {
                    int full = 1;
                    for (int x = 0; x < 10; x++) {
                        if (!grid[y][x]) full = 0;
                    }
                    if (full) {
                        lines_cleared++;
                        for (int yy = y; yy > 0; yy--) {
                            for (int x = 0; x < 10; x++) {
                                grid[yy][x] = grid[yy-1][x];
                            }
                        }
                        for (int x = 0; x < 10; x++) {
                            grid[0][x] = 0;
                        }
                        y++; // Re-check this line
                    }
                }
                
                if (lines_cleared == 1) score += 100;
                else if (lines_cleared == 2) score += 300;
                else if (lines_cleared == 3) score += 500;
                else if (lines_cleared == 4) score += 800;
                
                // Speed up slightly
                if (fall_delay > 10) fall_delay = 50 - (score / 1000);
                
                // Spawn next
                current_shape = (timer_get_ticks() + score) % 7;
                current_rot = 0;
                current_x = 3;
                current_y = -2;
                
                if (check_collision(grid, current_shape, current_rot, current_x, current_y + 1)) {
                    game_over = 1;
                }
            }
            last_tick = timer_get_ticks();
        }
    }
    
    clear_screen();
    print_colored("\n--- GAME OVER ---\n", 0x0C);
    print("Final Score: ");
    print_dec(score);
    print("\n\n");
}
