#include "shell.h"
#include "keyboard.h"
#include "timer.h"

extern void clear_screen();
extern void print_colored(const char* str, unsigned char color);
extern void put_char_at(int x, int y, char c, unsigned char color);
extern void print_dec(unsigned int num);
extern void print(const char* str);
extern uint32_t timer_get_ticks();

void cmd_pong() {
    clear_screen();
    print_colored("--- PONG ---\n\n", 0x0E);
    print("Choose game mode:\n");
    print("1. 1 Player (vs CPU)\n");
    print("2. 2 Players (Local)\n\n");
    print("Press 1 or 2...");
    
    int is_cpu_active = 0;
    while (1) {
        if (keyboard_available()) {
            char c = keyboard_getchar();
            if (c == '1') {
                is_cpu_active = 1;
                break;
            } else if (c == '2') {
                is_cpu_active = 0;
                break;
            } else if (c == 27) { // ESC to abort
                clear_screen();
                return;
            }
        }
        __asm__ __volatile__("hlt");
    }
    
    int p1_y = 10;
    int p2_y = 10;
    int paddle_height = 5;
    
    int ball_x = 40;
    int ball_y = 12;
    int ball_dx = 1;
    int ball_dy = 1;
    
    int score1 = 0;
    int score2 = 0;
    
    int game_over = 0;
    uint32_t last_tick = timer_get_ticks();
    
    clear_screen();
    
    while (!game_over) {
        // Render step
        clear_screen();
        
        // Draw top and bottom borders
        for (int x = 0; x < 80; x++) {
            put_char_at(x, 0, '=', 0x0F);
            put_char_at(x, 24, '=', 0x0F);
        }
        
        // Draw net (middle dotted line)
        for (int y = 1; y < 24; y += 2) {
            put_char_at(39, y, '|', 0x08); // Dark gray
        }
        
        // Draw paddles
        for (int i = 0; i < paddle_height; i++) {
            put_char_at(2, p1_y + i, ']', 0x0B); // P1 (Light Cyan)
            put_char_at(77, p2_y + i, '[', 0x0C); // P2/CPU (Light Red)
        }
        
        // Draw ball
        put_char_at(ball_x, ball_y, 'O', 0x0E); // Ball (Yellow)
        
        // Draw scores
        put_char_at(20, 2, '0' + score1, 0x0B);
        put_char_at(60, 2, '0' + score2, 0x0C);
        
        put_char_at(2, 23, 'W', 0x07); put_char_at(4, 23, 'S', 0x07);
        if (!is_cpu_active) {
            put_char_at(74, 23, '^', 0x07); put_char_at(76, 23, 'v', 0x07);
        } else {
            put_char_at(74, 23, 'C', 0x0C); put_char_at(75, 23, 'P', 0x0C); put_char_at(76, 23, 'U', 0x0C);
        }
        print_colored("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", 0x07); // flush
        
        // Input and timing
        while (timer_get_ticks() - last_tick < 4) { // Adjust for speed
            if (keyboard_available()) {
                char c = keyboard_getchar();
                if (c == 27) { game_over = 1; break; } // ESC
                
                // P1 controls (W / S)
                if ((c == 'w' || c == 'W') && p1_y > 1) p1_y--;
                if ((c == 's' || c == 'S') && p1_y < 24 - paddle_height) p1_y++;
                
                // P2 controls (Up / Down arrows)
                if (!is_cpu_active) {
                    if (c == 17 && p2_y > 1) p2_y--; // Up
                    if (c == 18 && p2_y < 24 - paddle_height) p2_y++; // Down
                }
            }
            __asm__ __volatile__("hlt");
        }
        
        if (game_over) break;
        last_tick = timer_get_ticks();
        
        // Simple CPU AI for Player 2
        if (is_cpu_active && ball_dx > 0 && ball_x > 40) {
            // Add a little delay to the CPU so it's beatable
            if (timer_get_ticks() % 2 == 0) {
                int center_p2 = p2_y + (paddle_height / 2);
                if (center_p2 < ball_y && p2_y < 24 - paddle_height) p2_y++;
                else if (center_p2 > ball_y && p2_y > 1) p2_y--;
            }
        }
        
        // Ball movement
        ball_x += ball_dx;
        ball_y += ball_dy;
        
        // Top/Bottom wall collisions
        if (ball_y <= 1) {
            ball_y = 1;
            ball_dy = -ball_dy;
        } else if (ball_y >= 23) {
            ball_y = 23;
            ball_dy = -ball_dy;
        }
        
        // Paddle collisions
        if (ball_x == 3 && ball_y >= p1_y && ball_y < p1_y + paddle_height) {
            ball_dx = -ball_dx;
            ball_x = 4;
        } else if (ball_x == 76 && ball_y >= p2_y && ball_y < p2_y + paddle_height) {
            ball_dx = -ball_dx;
            ball_x = 75;
        }
        
        // Scoring (Left/Right walls)
        if (ball_x <= 0) {
            score2++;
            ball_x = 40; ball_y = 12; ball_dx = 1;
            // Delay after score
            uint32_t wait = timer_get_ticks();
            while(timer_get_ticks() - wait < 50) __asm__ __volatile__("hlt");
        } else if (ball_x >= 79) {
            score1++;
            ball_x = 40; ball_y = 12; ball_dx = -1;
            // Delay after score
            uint32_t wait = timer_get_ticks();
            while(timer_get_ticks() - wait < 50) __asm__ __volatile__("hlt");
        }
        
        // Win condition
        if (score1 >= 9 || score2 >= 9) {
            game_over = 1;
        }
    }
    
    clear_screen();
    print_colored("\n--- GAME OVER ---\n", 0x0E);
    if (score1 > score2) {
        print_colored("Player 1 Wins!\n\n", 0x0B);
    } else if (score2 > score1) {
        if (is_cpu_active) print_colored("CPU Wins!\n\n", 0x0C);
        else print_colored("Player 2 Wins!\n\n", 0x0C);
    } else {
        print("Game aborted. It's a tie!\n\n");
    }
}
