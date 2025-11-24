; ISR (Interrupt Service Routine) Handlers
; Assembly stubs for CPU exceptions

[BITS 32]

; External C exception handler
extern _exception_handler

; Macro to create ISR without error code
%macro ISR_NOERRCODE 1
    global _isr%1
    _isr%1:
        cli                     ; Disable interrupts
        push dword 0            ; Push dummy error code
        push dword %1           ; Push interrupt number
        jmp isr_common_stub
%endmacro

; Macro to create ISR with error code
%macro ISR_ERRCODE 1
    global _isr%1
    _isr%1:
        cli                     ; Disable interrupts
        push dword %1           ; Push interrupt number
        jmp isr_common_stub
%endmacro

; CPU Exception ISRs (0-31)
ISR_NOERRCODE 0     ; Divide By Zero
ISR_NOERRCODE 1     ; Debug
ISR_NOERRCODE 2     ; Non Maskable Interrupt
ISR_NOERRCODE 3     ; Breakpoint
ISR_NOERRCODE 4     ; Overflow
ISR_NOERRCODE 5     ; Bound Range Exceeded
ISR_NOERRCODE 6     ; Invalid Opcode
ISR_NOERRCODE 7     ; Device Not Available
ISR_ERRCODE   8     ; Double Fault
ISR_NOERRCODE 9     ; Coprocessor Segment Overrun
ISR_ERRCODE   10    ; Invalid TSS
ISR_ERRCODE   11    ; Segment Not Present
ISR_ERRCODE   12    ; Stack-Segment Fault
ISR_ERRCODE   13    ; General Protection Fault
ISR_ERRCODE   14    ; Page Fault
ISR_NOERRCODE 15    ; Reserved
ISR_NOERRCODE 16    ; x87 Floating-Point Exception
ISR_ERRCODE   17    ; Alignment Check
ISR_NOERRCODE 18    ; Machine Check
ISR_NOERRCODE 19    ; SIMD Floating-Point Exception
ISR_NOERRCODE 20    ; Virtualization Exception
ISR_NOERRCODE 21    ; Reserved
ISR_NOERRCODE 22    ; Reserved
ISR_NOERRCODE 23    ; Reserved
ISR_NOERRCODE 24    ; Reserved
ISR_NOERRCODE 25    ; Reserved
ISR_NOERRCODE 26    ; Reserved
ISR_NOERRCODE 27    ; Reserved
ISR_NOERRCODE 28    ; Reserved
ISR_NOERRCODE 29    ; Reserved
ISR_ERRCODE   30    ; Security Exception
ISR_NOERRCODE 31    ; Reserved

; Hardware IRQ handlers (32-47)
; IRQs are remapped to interrupts 32-47 by PIC

; Macro to create IRQ handler
%macro IRQ 2
    global _irq%1
    _irq%1:
        cli
        push dword 0            ; Dummy error code
        push dword %2           ; Interrupt number (32 + IRQ number)
        jmp irq_common_stub
%endmacro

IRQ 0, 32       ; PIT Timer
IRQ 1, 33       ; Keyboard
IRQ 2, 34       ; Cascade (never raised)
IRQ 3, 35       ; COM2
IRQ 4, 36       ; COM1
IRQ 5, 37       ; LPT2
IRQ 6, 38       ; Floppy disk
IRQ 7, 39       ; LPT1
IRQ 8, 40       ; CMOS real-time clock
IRQ 9, 41       ; Free for peripherals
IRQ 10, 42      ; Free for peripherals
IRQ 11, 43      ; Free for peripherals
IRQ 12, 44      ; PS2 Mouse
IRQ 13, 45      ; FPU / Coprocessor
IRQ 14, 46      ; Primary ATA
IRQ 15, 47      ; Secondary ATA

; Common ISR stub
; Saves processor state, calls C handler, restores state
isr_common_stub:
    ; Save all registers
    pusha
    
    ; Save segment registers
    push ds
    push es
    push fs
    push gs
    
    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Push stack pointer (contains register state)
    mov eax, esp
    push eax
    
    ; Call C exception handler
    ; Arguments: interrupt number, error code (already on stack)
    call _exception_handler
    
    ; Clean up pushed arguments
    pop eax
    
    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds
    
    ; Restore all registers
    popa
    
    ; Clean up error code and interrupt number
    add esp, 8
    
    ; Return from interrupt
    iret

; Common IRQ stub
; Similar to ISR stub but calls IRQ handler
extern _irq_handler

irq_common_stub:
    ; Save all registers
    pusha
    
    ; Save segment registers
    push ds
    push es
    push fs
    push gs
    
    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Push stack pointer
    mov eax, esp
    push eax
    
    ; Call C IRQ handler
    call _irq_handler
    
    ; Clean up
    pop eax
    
    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds
    
    ; Restore all registers
    popa
    
    ; Clean up error code and interrupt number
    add esp, 8
    
    ; Return from interrupt
    iret

; Load IDT function
global _idt_load
_idt_load:
    mov eax, [esp + 4]      ; Get IDT pointer parameter
    lidt [eax]              ; Load IDT
    ret
