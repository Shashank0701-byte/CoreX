/* Linker script for CoreX kernel (Multiboot/GRUB compatible) */

ENTRY(_start)           /* Entry point is _start from kernel_stub.asm */

SECTIONS
{
    /* Kernel is loaded at 0x10000 by bootloader (above MBR at 0x7C00) */
    . = 0x10000;
    
    .text : ALIGN(4096)
    {
        *(.text)        /* Code section */
    }
    .rodata : ALIGN(4096)
    {
        *(.rodata*)     /* Read-only data */
    }
    
    .data : ALIGN(4096)
    {
        *(.data)        /* Initialized data */
    }
    
    .bss : ALIGN(4096)
    {
        *(COMMON)       /* Uninitialized data */
        *(.bss)
    }
}
