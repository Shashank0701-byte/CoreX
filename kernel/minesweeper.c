#include "shell.h"
#include "keyboard.h"
#include "timer.h"

extern void clear_screen();
extern void print_colored(const char* str, uint8_t color);
extern void put_char_at(int x, int y, char c, uint8_t color);
extern uint32_t timer_get_ticks();

// Simple pseudo-random number generator
static uint32_t rand_seed = 9999;
static uint32_t ms_rand() {
    rand_seed = (1103515245 * rand_seed + 12345) & 0x7FFFFFFF;
    return rand_seed;
}

#define BOARD_W 20
#define BOARD_H 15
#define NUM_MINES 30

typedef struct {
    uint8_t is_mine;
    uint8_t is_revealed;
    uint8_t is_flagged;
    uint8_t neighbor_mines;
} Cell;

static Cell board[BOARD_H][BOARD_W];
static int cursor_x = BOARD_W / 2;
static int cursor_y = BOARD_H / 2;
static int ms_game_over = 0;
static int ms_win = 0;
static int first_click = 1;

static void place_mines(int safe_x, int safe_y) {
    int mines_placed = 0;
    while (mines_placed < NUM_MINES) {
        int x = ms_rand() % BOARD_W;
        int y = ms_rand() % BOARD_H;
        
        // Don't place mine on the first click or immediately adjacent
        int dx = x - safe_x;
        int dy = y - safe_y;
        if (dx*dx <= 1 && dy*dy <= 1) continue;
        
        if (!board[y][x].is_mine) {
            board[y][x].is_mine = 1;
            mines_placed++;
        }
    }
    
    // Calculate neighbors
    for (int y = 0; y < BOARD_H; y++) {
        for (int x = 0; x < BOARD_W; x++) {
            if (board[y][x].is_mine) continue;
            int count = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < BOARD_W && ny >= 0 && ny < BOARD_H) {
                        if (board[ny][nx].is_mine) count++;
                    }
                }
            }
            board[y][x].neighbor_mines = count;
        }
    }
}

// Iterative flood fill for revealing cells to avoid stack overflow in kernel
static void reveal_cell(int start_x, int start_y) {
    if (board[start_y][start_x].is_flagged || board[start_y][start_x].is_revealed) return;
    
    if (board[start_y][start_x].is_mine) {
        ms_game_over = 1;
        board[start_y][start_x].is_revealed = 1;
        return;
    }

    // Stack for iterative DFS
    int stack_x[BOARD_W * BOARD_H];
    int stack_y[BOARD_W * BOARD_H];
    int stack_ptr = 0;
    
    stack_x[stack_ptr] = start_x;
    stack_y[stack_ptr] = start_y;
    stack_ptr++;
    
    while (stack_ptr > 0) {
        stack_ptr--;
        int x = stack_x[stack_ptr];
        int y = stack_y[stack_ptr];
        
        if (board[y][x].is_revealed || board[y][x].is_flagged) continue;
        
        board[y][x].is_revealed = 1;
        
        if (board[y][x].neighbor_mines == 0) {
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < BOARD_W && ny >= 0 && ny < BOARD_H) {
                        if (!board[ny][nx].is_revealed && !board[ny][nx].is_flagged) {
                            stack_x[stack_ptr] = nx;
                            stack_y[stack_ptr] = ny;
                            stack_ptr++;
                        }
                    }
                }
            }
        }
    }
}

static void check_win() {
    int unrevealed_safe = 0;
    for (int y = 0; y < BOARD_H; y++) {
        for (int x = 0; x < BOARD_W; x++) {
            if (!board[y][x].is_mine && !board[y][x].is_revealed) {
                unrevealed_safe++;
            }
        }
    }
    if (unrevealed_safe == 0) {
        ms_win = 1;
        ms_game_over = 1;
    }
}

static void ms_print_at(int px, int py, const char* str, uint8_t c_color) {
    int i = 0;
    while (str[i]) {
        put_char_at(px + i, py, str[i], c_color);
        i++;
    }
}

static void draw_board() {
    int offset_x = (80 - (BOARD_W * 2)) / 2;
    int offset_y = (25 - BOARD_H) / 2;
    
    for (int y = 0; y < BOARD_H; y++) {
        for (int x = 0; x < BOARD_W; x++) {
            char c = 254; // Square block
            uint8_t color = 0x08; // Dark Gray (unrevealed)
            
            if (board[y][x].is_revealed) {
                if (board[y][x].is_mine) {
                    c = '*';
                    color = 0x0C; // Red mine
                } else if (board[y][x].neighbor_mines > 0) {
                    c = '0' + board[y][x].neighbor_mines;
                    // Colors based on number
                    uint8_t num_colors[] = {0, 0x09, 0x0A, 0x0C, 0x05, 0x04, 0x03, 0x00, 0x08};
                    color = num_colors[board[y][x].neighbor_mines];
                } else {
                    c = ' ';
                    color = 0x07;
                }
            } else if (board[y][x].is_flagged) {
                c = 'P'; // Flag symbol
                color = 0x0E; // Yellow flag
            }
            
            // Invert colors if cursor is here
            if (x == cursor_x && y == cursor_y) {
                color = (color & 0x0F) | 0x70; // Gray background
            }
            
            put_char_at(offset_x + x * 2, offset_y + y, c, color);
            put_char_at(offset_x + x * 2 + 1, offset_y + y, ' ', color); // spacing
        }
    }
    
    // Draw UI without scrolling
    ms_print_at(3, 21, "MINESWEEPER  |  ARROWS: Move  |  SPACE: Reveal  |  F: Flag  |  ESC: Quit", 0x0B);
    
    if (ms_game_over) {
        if (ms_win) ms_print_at(3, 23, "YOU WIN! ALL MINES CLEARED! Press ESC to return.", 0x0A);
        else ms_print_at(3, 23, "BOOM! YOU HIT A MINE! Press ESC to return.      ", 0x0C);
    }
}
void cmd_minesweeper() {
    rand_seed = timer_get_ticks();
    
    // Init game state
    for (int y = 0; y < BOARD_H; y++) {
        for (int x = 0; x < BOARD_W; x++) {
            board[y][x].is_mine = 0;
            board[y][x].is_revealed = 0;
            board[y][x].is_flagged = 0;
            board[y][x].neighbor_mines = 0;
        }
    }
    
    cursor_x = BOARD_W / 2;
    cursor_y = BOARD_H / 2;
    ms_game_over = 0;
    ms_win = 0;
    first_click = 1;
    
    clear_screen();
    
    while (1) {
        draw_board();
        
        while (!keyboard_available()) {
            __asm__ __volatile__("hlt");
        }
        
        char c = keyboard_getchar();
        if (c == 27) break; // ESC
        
        if (!ms_game_over) {
            // Movement
            if (c == 17 && cursor_y > 0) cursor_y--; // Up
            if (c == 18 && cursor_y < BOARD_H - 1) cursor_y++; // Down
            if (c == 20 && cursor_x > 0) cursor_x--; // Left
            if (c == 19 && cursor_x < BOARD_W - 1) cursor_x++; // Right
            
            // Flag
            if (c == 'f' || c == 'F') {
                if (!board[cursor_y][cursor_x].is_revealed) {
                    board[cursor_y][cursor_x].is_flagged = !board[cursor_y][cursor_x].is_flagged;
                }
            }
            
            // Reveal
            if (c == ' ') {
                if (!board[cursor_y][cursor_x].is_flagged) {
                    if (first_click) {
                        place_mines(cursor_x, cursor_y);
                        first_click = 0;
                    }
                    reveal_cell(cursor_x, cursor_y);
                    check_win();
                    
                    // If game over, reveal all mines
                    if (ms_game_over && !ms_win) {
                        for (int y = 0; y < BOARD_H; y++) {
                            for (int x = 0; x < BOARD_W; x++) {
                                if (board[y][x].is_mine) board[y][x].is_revealed = 1;
                            }
                        }
                    }
                }
            }
        }
    }
    
    clear_screen();
}
