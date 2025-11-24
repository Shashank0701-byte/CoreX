; CoreX Kernel Entry Point (32-bit Protected Mode)
; Loaded at 0x1000 by the bootloader
; Runs in 32-bit protected mode

[BITS 32]
[ORG 0x1000]

; VGA text mode constants
VIDEO_MEMORY equ 0xB8000
WHITE_ON_BLACK equ 0x0F

kernel_start:
    ; Clear screen
    call clear_screen
    
    ; Print kernel message
    mov esi, kernel_msg
    call print_string_pm
    
    ; Halt the CPU
    cli
    hlt

; Function: clear_screen
; Clears the VGA text mode screen
clear_screen:
    pusha
    mov edi, VIDEO_MEMORY
    mov ecx, 80 * 25        ; 80 columns x 25 rows
    mov ax, 0x0F20          ; White on black space
    rep stosw
    popa
    ret

; Function: print_string_pm
; Prints null-terminated string in protected mode
; ESI = pointer to string
print_string_pm:
    pusha
    mov edx, VIDEO_MEMORY
    
.loop:
    lodsb                   ; Load byte from ESI into AL
    cmp al, 0
    je .done
    
    cmp al, 0x0A            ; Check for newline
    je .newline
    
    mov ah, WHITE_ON_BLACK
    mov [edx], ax
    add edx, 2
    jmp .loop

.newline:
    ; Move to next line (80 chars * 2 bytes per char)
    mov eax, edx
    sub eax, VIDEO_MEMORY
    xor edx, edx
    mov ebx, 160            ; 80 * 2
    div ebx
    inc eax
    mul ebx
    add eax, VIDEO_MEMORY
    mov edx, eax
    jmp .loop

.done:
    popa
    ret

; Data
kernel_msg db '=== CoreX Kernel v2.0 ===', 0x0A
           db 'Running in 32-bit Protected Mode', 0x0A
           db 'GDT loaded successfully', 0x0A
           db 'A20 line enabled', 0x0A
           db 'System initialized!', 0x0A, 0x0A
           db 'Kernel halted.', 0

; Pad the kernel to fill sectors
times 5120-($-$$) db 0  ; 10 sectors * 512 bytes = 5120 bytes
