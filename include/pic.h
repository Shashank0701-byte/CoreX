// PIC (Programmable Interrupt Controller) Implementation
// Handles hardware interrupt remapping and masking

#ifndef PIC_H
#define PIC_H

#include <stdint.h>

// PIC ports
#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1

// PIC commands
#define PIC_EOI         0x20    // End of Interrupt

// Initialize and remap PIC
void pic_init();

// Send End of Interrupt signal
void pic_send_eoi(uint8_t irq);

#endif // PIC_H
