; Simple x86 Bootloader
; Prints "Bootloader OK" and halts the CPU
; Must be exactly 512 bytes with boot signature 0xAA55

[BITS 16]           ; 16-bit real mode
[ORG 0x7C00]        ; BIOS loads bootloader at 0x7C00

start:
    ; Initialize segment registers
    xor ax, ax      ; Zero out AX
    mov ds, ax      ; Set DS to 0
    mov es, ax      ; Set ES to 0
    mov ss, ax      ; Set SS to 0
    mov sp, 0x7C00  ; Set stack pointer below bootloader

    ; Print message
    mov si, msg     ; Load address of message into SI
    call print_string

    ; Halt the CPU
    cli             ; Clear interrupts
    hlt             ; Halt the processor

; Function: print_string
; Prints null-terminated string pointed to by SI
print_string:
    pusha           ; Save all registers
    mov ah, 0x0E    ; BIOS teletype output function

.loop:
    lodsb           ; Load byte from [SI] into AL, increment SI
    cmp al, 0       ; Check if null terminator
    je .done        ; If null, we're done
    int 0x10        ; Call BIOS interrupt to print character
    jmp .loop       ; Repeat for next character

.done:
    popa            ; Restore all registers
    ret

; Data section
msg db 'Bootloader OK', 0x0D, 0x0A, 0  ; Message with CR, LF, and null terminator

; Pad to 510 bytes and add boot signature
times 510-($-$$) db 0   ; Fill remaining space with zeros
dw 0xAA55               ; Boot signature (little-endian)
