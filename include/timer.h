// PIT (Programmable Interval Timer) Implementation
// Provides system timer functionality

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// PIT I/O ports
#define PIT_CHANNEL0    0x40
#define PIT_CHANNEL1    0x41
#define PIT_CHANNEL2    0x42
#define PIT_COMMAND     0x43

// PIT frequency
#define PIT_FREQUENCY   1193182

// Initialize PIT timer
void timer_init(uint32_t frequency);

// Get current tick count
uint32_t timer_get_ticks();

// Timer interrupt handler (called from IRQ0)
void timer_handler();

#endif // TIMER_H
