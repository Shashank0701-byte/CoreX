// Simple Shell Header
// Command-line interface for CoreX OS

#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

// Shell configuration
#define SHELL_BUFFER_SIZE 256
#define SHELL_PROMPT "CoreX> "

// Initialize shell
void shell_init();

// Run shell (main loop)
void shell_run();

// Handle keyboard input
void shell_handle_input(char c);

// Execute command
void shell_execute(const char* command);

#endif // SHELL_H
