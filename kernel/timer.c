// PIT (Programmable Interval Timer) Implementation
// Provides system timer at configurable frequency

#include "timer.h"
#include "pic.h"

// External print functions
extern void print(const char* str);
extern void print_hex(unsigned int num);

// Global tick counter
static volatile uint32_t tick_count = 0;
static uint32_t timer_frequency = 0;

// I/O port operations
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

// Initialize PIT timer
void timer_init(uint32_t frequency) {
    timer_frequency = frequency;
    
    // Calculate divisor
    uint32_t divisor = PIT_FREQUENCY / frequency;
    
    // Send command byte
    outb(PIT_COMMAND, 0x36);    // Channel 0, lo/hi byte, rate generator
    
    // Send divisor
    outb(PIT_CHANNEL0, divisor & 0xFF);         // Low byte
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);  // High byte
    
    print("PIT timer initialized at ");
    print_hex(frequency);
    print(" Hz\n");
}

// Get current tick count
uint32_t timer_get_ticks() {
    return tick_count;
}

// Timer interrupt handler
void timer_handler() {
    tick_count++;
    
    // Disabled printing to prevent screen updates
    // Uncomment to see uptime every 5 seconds
    /*
    if (timer_frequency > 0 && (tick_count % (timer_frequency * 5)) == 0) {
        print("Uptime: ");
        print_hex(tick_count / timer_frequency);
        print(" seconds\n");
    }
    */
    
    // Send EOI to PIC
    pic_send_eoi(0);
}
