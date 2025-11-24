// PIC (Programmable Interrupt Controller) Implementation
// Remaps IRQs to avoid conflicts with CPU exceptions

#include "pic.h"

// I/O port operations
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Initialize and remap PIC
// Remaps IRQ 0-15 to interrupts 32-47 (after CPU exceptions)
void pic_init() {
    uint8_t mask1, mask2;
    
    // Save masks
    mask1 = inb(PIC1_DATA);
    mask2 = inb(PIC2_DATA);
    
    // Start initialization sequence (ICW1)
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    
    // Set vector offsets (ICW2)
    outb(PIC1_DATA, 0x20);      // Master PIC offset to 32
    outb(PIC2_DATA, 0x28);      // Slave PIC offset to 40
    
    // Tell Master PIC about Slave (ICW3)
    outb(PIC1_DATA, 0x04);      // Slave on IRQ2
    outb(PIC2_DATA, 0x02);      // Slave identity
    
    // Set mode (ICW4)
    outb(PIC1_DATA, 0x01);      // 8086 mode
    outb(PIC2_DATA, 0x01);      // 8086 mode
    
    // Restore masks
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

// Send End of Interrupt signal
void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}
