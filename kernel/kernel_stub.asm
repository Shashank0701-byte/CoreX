; Kernel Entry Stub (32-bit)
; This assembly code is the entry point that sets up the environment
; and calls the C kernel's kmain() function

[BITS 32]
[EXTERN _kmain]         ; Declare external C function (with underscore for Windows GCC)

; Entry point - this is where the bootloader jumps
global _start
_start:
    ; Set up segment registers (already done by bootloader, but ensure)
    mov ax, 0x10        ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Set up stack
    mov ebp, 0x90000
    mov esp, ebp
    
    ; Call C kernel main function
    call _kmain
    
    ; If kmain returns (it shouldn't), halt
    cli
    hlt
    
    ; Infinite loop just in case
.hang:
    jmp .hang
