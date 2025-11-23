; Kernel Entry Point (16-bit)
; This is the initial kernel code loaded by the bootloader
; Loaded at 0x1000 by the bootloader

[BITS 16]
[ORG 0x1000]

kernel_start:
    ; Print kernel message
    mov si, kernel_msg
    call print_string
    
    ; Halt the CPU
    cli
    hlt

; Function: print_string
print_string:
    pusha
    mov ah, 0x0E

.loop:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .loop

.done:
    popa
    ret

; Data
kernel_msg db 'CoreX Kernel v1.0 loaded successfully!', 0x0D, 0x0A, 'System halted.', 0x0D, 0x0A, 0

; Pad the kernel to fill sectors
times 5120-($-$$) db 0  ; 10 sectors * 512 bytes = 5120 bytes
