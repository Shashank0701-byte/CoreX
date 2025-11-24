// Task Scheduler Header
// Cooperative multitasking with round-robin scheduling

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

// Task states
#define TASK_READY      0
#define TASK_RUNNING    1
#define TASK_BLOCKED    2
#define TASK_TERMINATED 3

// Maximum number of tasks
#define MAX_TASKS 8

// Task stack size (4KB)
#define TASK_STACK_SIZE 4096

// Task structure
typedef struct task {
    uint32_t id;
    uint32_t esp;           // Stack pointer
    uint32_t ebp;           // Base pointer
    uint32_t eip;           // Instruction pointer
    uint32_t state;
    uint32_t stack[TASK_STACK_SIZE / 4];  // Task stack
    struct task* next;
} task_t;

// Task function pointer
typedef void (*task_func_t)(void);

// Initialize scheduler
void scheduler_init();

// Create a new task
int task_create(task_func_t func);

// Yield CPU to next task
void task_yield();

// Schedule next task (called from timer)
void schedule();

// Get current task ID
uint32_t get_current_task_id();

#endif // SCHEDULER_H
