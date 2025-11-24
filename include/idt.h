// IDT (Interrupt Descriptor Table) Implementation
// Handles CPU exceptions and interrupts

#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// IDT entry structure
struct idt_entry {
    uint16_t offset_low;    // Lower 16 bits of handler address
    uint16_t selector;      // Kernel segment selector
    uint8_t  zero;          // Always zero
    uint8_t  type_attr;     // Type and attributes
    uint16_t offset_high;   // Upper 16 bits of handler address
} __attribute__((packed));

// IDT pointer structure
struct idt_ptr {
    uint16_t limit;         // Size of IDT - 1
    uint32_t base;          // Address of IDT
} __attribute__((packed));

// Number of IDT entries
#define IDT_ENTRIES 256

// Initialize IDT
void idt_init();

// Set an IDT gate
void idt_set_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t flags);

// Exception handler (called from assembly)
void exception_handler(uint32_t int_no, uint32_t err_code);

#endif // IDT_H
