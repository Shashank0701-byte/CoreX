#include "shell.h"
#include "keyboard.h"
#include "timer.h"
#include "graphics.h"

extern void clear_screen();
extern uint32_t timer_get_ticks();

// Simple pseudo-random number generator
static uint32_t rand_seed = 12345;
static uint32_t rand() {
    rand_seed = (1103515245 * rand_seed + 12345) & 0x7FFFFFFF;
    return rand_seed;
}

#define BRICK_ROWS 6
#define BRICK_COLS 10
#define BRICK_WIDTH 30
#define BRICK_HEIGHT 10
#define BRICK_PADDING 2
#define BRICK_OFFSET_X 10
#define BRICK_OFFSET_Y 20

void cmd_breakout() {
    // Switch to VGA Graphics Mode 13h (320x200, 256 colors)
    graphics_set_mode_13h();
    
    while (1) {
        rand_seed = timer_get_ticks();
        
        int paddle_x = 140;
        int paddle_y = 185;
        int paddle_width = 40;
        int paddle_height = 5;
        
        int ball_x = 160;
        int ball_y = 175;
        int ball_dx = 1; 
        int ball_dy = -1;
        int ball_size = 4;
        
        int bricks[BRICK_ROWS][BRICK_COLS];
        int bricks_remaining = 0;
        
        // Setup bricks
        for (int r = 0; r < BRICK_ROWS; r++) {
            for (int c = 0; c < BRICK_COLS; c++) {
                bricks[r][c] = 1;
                bricks_remaining++;
            }
        }
        
        uint8_t row_colors[] = {40, 42, 44, 46, 48, 50}; // Classic VGA color indices
        
        int game_over = 0;
        int win = 0;
        int quit_game = 0;
        uint32_t last_tick = timer_get_ticks();
        
        // Full screen background
        graphics_clear(0); // Black
        
        // Draw initial static elements
        graphics_draw_string(5, 5, "BREAKOUT", 15); // White text
        
        // Draw all bricks initially
        for (int r = 0; r < BRICK_ROWS; r++) {
            for (int c = 0; c < BRICK_COLS; c++) {
                int bx = BRICK_OFFSET_X + c * (BRICK_WIDTH + BRICK_PADDING);
                int by = BRICK_OFFSET_Y + r * (BRICK_HEIGHT + BRICK_PADDING);
                graphics_fill_rect(bx, by, BRICK_WIDTH, BRICK_HEIGHT, row_colors[r]);
            }
        }
        
        while (!game_over && !win) {
            // Draw ball and paddle
            graphics_fill_rect(paddle_x, paddle_y, paddle_width, paddle_height, 15); // White paddle
            graphics_fill_rect(ball_x, ball_y, ball_size, ball_size, 14); // Yellow ball
            
            // Input handling (wait loop to control speed)
            while (timer_get_ticks() - last_tick < 1) { 
                if (keyboard_available()) {
                    char c = keyboard_getchar();
                    if (c == 27) { game_over = 1; quit_game = 1; break; } // ESC
                    
                    // Left Arrow or A
                    if ((c == 20 || c == 'a' || c == 'A') && paddle_x > 0) {
                        graphics_fill_rect(paddle_x, paddle_y, paddle_width, paddle_height, 0);
                        paddle_x -= 10;
                        if (paddle_x < 0) paddle_x = 0;
                        graphics_fill_rect(paddle_x, paddle_y, paddle_width, paddle_height, 15);
                    }
                    // Right Arrow or D
                    if ((c == 19 || c == 'd' || c == 'D') && paddle_x < 320 - paddle_width) {
                        graphics_fill_rect(paddle_x, paddle_y, paddle_width, paddle_height, 0);
                        paddle_x += 10;
                        if (paddle_x > 320 - paddle_width) paddle_x = 320 - paddle_width;
                        graphics_fill_rect(paddle_x, paddle_y, paddle_width, paddle_height, 15);
                    }
                }
            }
            last_tick = timer_get_ticks();
            
            if (quit_game) break;
            
            // Clear previous ball and paddle positions before moving
            graphics_fill_rect(paddle_x, paddle_y, paddle_width, paddle_height, 0); // Black
            graphics_fill_rect(ball_x, ball_y, ball_size, ball_size, 0); // Black
            
            // Ball movement (reduced speed to 1 pixel per tick instead of 2)
            ball_x += ball_dx;
            ball_y += ball_dy;
            
            // Wall collisions
            if (ball_x <= 0) {
                ball_x = 0; ball_dx = -ball_dx;
            } else if (ball_x >= 320 - ball_size) {
                ball_x = 320 - ball_size; ball_dx = -ball_dx;
            }
            
            if (ball_y <= 0) {
                ball_y = 0; ball_dy = -ball_dy;
            } else if (ball_y >= 200) {
                // Missed!
                game_over = 1;
            }
            
            // Paddle collision
            if (ball_dy > 0 && ball_y + ball_size >= paddle_y && ball_y <= paddle_y + paddle_height) {
                if (ball_x + ball_size >= paddle_x && ball_x <= paddle_x + paddle_width) {
                    ball_dy = -ball_dy;
                    ball_y = paddle_y - ball_size;
                    
                    // Add some English (change X direction based on where it hit)
                    int hit_pos = (ball_x + ball_size/2) - (paddle_x + paddle_width/2);
                    if (hit_pos < -5) ball_dx = -1;
                    else if (hit_pos > 5) ball_dx = 1;
                }
            }
            
            // Brick collision
            int hit_brick = 0;
            for (int r = 0; r < BRICK_ROWS && !hit_brick; r++) {
                for (int c = 0; c < BRICK_COLS && !hit_brick; c++) {
                    if (bricks[r][c]) {
                        int bx = BRICK_OFFSET_X + c * (BRICK_WIDTH + BRICK_PADDING);
                        int by = BRICK_OFFSET_Y + r * (BRICK_HEIGHT + BRICK_PADDING);
                        
                        if (ball_x + ball_size >= bx && ball_x <= bx + BRICK_WIDTH &&
                            ball_y + ball_size >= by && ball_y <= by + BRICK_HEIGHT) {
                            
                            // Destroy brick
                            bricks[r][c] = 0;
                            bricks_remaining--;
                            hit_brick = 1;
                            
                            // Erase brick visually
                            graphics_fill_rect(bx, by, BRICK_WIDTH, BRICK_HEIGHT, 0); // Black
                            
                            // Bounce ball
                            int overlap_left = (ball_x + ball_size) - bx;
                            int overlap_right = (bx + BRICK_WIDTH) - ball_x;
                            int overlap_top = (ball_y + ball_size) - by;
                            int overlap_bottom = (by + BRICK_HEIGHT) - ball_y;
                            
                            int min_overlap = overlap_left;
                            if (overlap_right < min_overlap) min_overlap = overlap_right;
                            if (overlap_top < min_overlap) min_overlap = overlap_top;
                            if (overlap_bottom < min_overlap) min_overlap = overlap_bottom;
                            
                            if (min_overlap == overlap_left || min_overlap == overlap_right) {
                                ball_dx = -ball_dx;
                            } else {
                                ball_dy = -ball_dy;
                            }
                        }
                    }
                }
            }
            
            if (bricks_remaining <= 0) {
                win = 1;
            }
        }
        
        if (quit_game) break; // Jump straight to reboot
        
        // End sequence
        if (win) {
            graphics_draw_string(110, 100, "YOU WIN!", 10); // Light Green
        } else {
            graphics_draw_string(110, 100, "GAME OVER", 12); // Light Red
        }
        graphics_draw_string(70, 120, "SPACE: RESTART GAME", 15); // White
        graphics_draw_string(70, 135, "  ESC: REBOOT SYSTEM", 15); // White
        
        int do_restart = 0;
        while (1) {
            if (keyboard_available()) {
                char c = keyboard_getchar();
                if (c == ' ') { do_restart = 1; break; } // SPACE
                if (c == 27) { break; } // ESC
            }
            __asm__ __volatile__("hlt");
        }
        
        if (!do_restart) break; // Exit main loop and reboot
    }
    
    // Reboot system
    __asm__ __volatile__("outb %0, %1" : : "a"((uint8_t)0xFE), "Nd"((uint16_t)0x64));
    
    while(1) {
        __asm__ __volatile__("hlt");
    }
}
