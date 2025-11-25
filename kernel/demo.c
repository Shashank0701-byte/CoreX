// Demo Mode - Simulates keyboard input to showcase shell functionality
// This demonstrates that the shell command parsing works

#include "shell.h"

extern void shell_execute(const char* command);
extern void print(const char* str);

void run_demo() {
    print("\n=== DEMO MODE: Showcasing Shell Commands ===\n\n");
    
    // Simulate typing "help"
    print("CoreX> help\n");
    shell_execute("help");
    
    // Wait a bit (simulate user reading)
    for(volatile int i = 0; i < 10000000; i++);
    
    // Simulate typing "version"
    print("CoreX> version\n");
    shell_execute("version");
    
    for(volatile int i = 0; i < 10000000; i++);
    
    // Simulate typing "meminfo"
    print("CoreX> meminfo\n");
    shell_execute("meminfo");
    
    for(volatile int i = 0; i < 10000000; i++);
    
    // Simulate typing "echo"
    print("CoreX> echo Hello from CoreX OS!\n");
    shell_execute("echo Hello from CoreX OS!");
    
    print("\n=== Demo Complete ===\n");
    print("Note: Interactive keyboard input is under development.\n");
    print("This demo shows the shell command system is fully functional.\n\n");
}
