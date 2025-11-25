// Task Scheduler Implementation
// Cooperative round-robin multitasking

#include "scheduler.h"
#include "pmm.h"

// External print functions
extern void print(const char* str);
extern void print_hex(unsigned int num);

// Task list
static task_t tasks[MAX_TASKS];
static task_t* current_task = 0;
static task_t* task_list_head = 0;
static uint32_t next_task_id = 0;
static int scheduler_enabled = 0;

// Assembly function to switch tasks
extern void switch_task(uint32_t* old_esp, uint32_t new_esp);

// Task entry wrapper
static void task_entry() {
    // Enable interrupts (new tasks start with IF=0 from switch_task)
    __asm__ __volatile__("sti");
    
    // Get the actual task function from the stack
    // Note: We stored the function pointer in the task struct's EIP field
    // purely for storage. It's not used by switch_task (which uses the stack).
    task_func_t func = (task_func_t)current_task->eip;
    
    // Call the task function
    func();
    
    // If task returns, mark as terminated
    current_task->state = TASK_TERMINATED;
    
    // Yield to next task
    while(1) {
        task_yield();
    }
}

// Initialize scheduler
void scheduler_init() {
    // Clear task array
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].state = TASK_TERMINATED;
        tasks[i].next = 0;
    }
    
    // Create idle task (task 0)
    tasks[0].id = next_task_id++;
    tasks[0].state = TASK_RUNNING;
    tasks[0].next = &tasks[0];  // Points to itself
    
    current_task = &tasks[0];
    task_list_head = &tasks[0];
    
    scheduler_enabled = 1;
    
    print("Scheduler initialized\n");
}

// Create a new task
int task_create(task_func_t func) {
    if (!scheduler_enabled) {
        return -1;
    }
    
    // Find free task slot
    int slot = -1;
    for (int i = 1; i < MAX_TASKS; i++) {
        if (tasks[i].state == TASK_TERMINATED) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) {
        print("Scheduler: No free task slots\n");
        return -1;
    }
    
    task_t* task = &tasks[slot];
    
    // Initialize task
    task->id = next_task_id++;
    task->state = TASK_READY;
    task->eip = (uint32_t)func;
    
    // Set up stack (grows downward)
    task->esp = (uint32_t)&task->stack[TASK_STACK_SIZE / 4 - 1];
    task->ebp = task->esp;
    
    // Push initial values on stack for context switch
    uint32_t* stack = (uint32_t*)task->esp;
    
    // Push registers (as they would be saved by switch_task)
    // Stack grows downwards, so we push in reverse order of switch_task pops
    // switch_task pops: EAX, ECX, EDX, EBX, EDI, ESI, EBP, RET
    // So we push: EBP, ESI, EDI, EBX, EDX, ECX, EAX
    
    // Return address for switch_task (pops into EIP)
    *(--stack) = (uint32_t)task_entry;
    
    *(--stack) = task->ebp;         // EBP
    *(--stack) = 0;                 // ESI
    *(--stack) = 0;                 // EDI
    *(--stack) = 0;                 // EBX
    *(--stack) = 0;                 // EDX
    *(--stack) = 0;                 // ECX
    *(--stack) = 0;                 // EAX
    
    task->esp = (uint32_t)stack;
    
    // Add to task list (circular linked list)
    task->next = task_list_head->next;
    task_list_head->next = task;
    
    print("Task created: ID ");
    print_hex(task->id);
    print("\n");
    
    return task->id;
}

// Yield CPU to next task
void task_yield() {
    if (!scheduler_enabled || !current_task) {
        return;
    }
    
    schedule();
}

// Schedule next task
void schedule() {
    if (!scheduler_enabled || !current_task) {
        return;
    }
    
    task_t* old_task = current_task;
    task_t* next_task = current_task->next;
    
    // Find next ready task
    int count = 0;
    while (next_task->state != TASK_READY && 
           next_task->state != TASK_RUNNING && 
           count < MAX_TASKS) {
        next_task = next_task->next;
        count++;
    }
    
    // If no ready task found, stay with current
    if (count >= MAX_TASKS) {
        return;
    }
    
    // Switch tasks
    if (old_task != next_task) {
        old_task->state = (old_task->state == TASK_RUNNING) ? TASK_READY : old_task->state;
        next_task->state = TASK_RUNNING;
        current_task = next_task;
        
        // Perform context switch
        switch_task(&old_task->esp, next_task->esp);
    }
}

// Get current task ID
uint32_t get_current_task_id() {
    if (current_task) {
        return current_task->id;
    }
    return 0;
}
