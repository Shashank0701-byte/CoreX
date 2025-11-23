; Enhanced x86 Bootloader
; Loads kernel from disk sector 2 into memory at 0x1000 and jumps to it
; Must be exactly 512 bytes with boot signature 0xAA55

[BITS 16]           ; 16-bit real mode
[ORG 0x7C00]        ; BIOS loads bootloader at 0x7C00

KERNEL_OFFSET equ 0x1000    ; Memory location to load kernel
KERNEL_SECTORS equ 10       ; Number of sectors to load (5KB kernel)

start:
    ; Initialize segment registers
    xor ax, ax      ; Zero out AX
    mov ds, ax      ; Set DS to 0
    mov es, ax      ; Set ES to 0
    mov ss, ax      ; Set SS to 0
    mov sp, 0x7C00  ; Set stack pointer below bootloader
    
    ; Save boot drive number (BIOS passes it in DL)
    mov [boot_drive], dl

    ; Print bootloader message
    mov si, msg_boot
    call print_string

    ; Load kernel from disk
    call load_kernel

    ; Print success message
    mov si, msg_loaded
    call print_string

    ; Jump to kernel
    jmp KERNEL_OFFSET

; Function: load_kernel
; Loads kernel from disk into memory
load_kernel:
    pusha
    
    mov ah, 0x02            ; BIOS read sector function
    mov al, KERNEL_SECTORS  ; Number of sectors to read
    mov ch, 0               ; Cylinder 0
    mov cl, 2               ; Start from sector 2 (sector 1 is bootloader)
    mov dh, 0               ; Head 0
    mov dl, [boot_drive]    ; Drive number
    mov bx, KERNEL_OFFSET   ; Destination address (ES:BX)
    
    int 0x13                ; Call BIOS disk interrupt
    jc disk_error           ; Jump if carry flag set (error)
    
    ; Check if correct number of sectors were read
    cmp al, KERNEL_SECTORS
    jne disk_error
    
    popa
    ret

disk_error:
    mov si, msg_error
    call print_string
    cli
    hlt

; Function: print_string
; Prints null-terminated string pointed to by SI
print_string:
    pusha
    mov ah, 0x0E    ; BIOS teletype output function

.loop:
    lodsb           ; Load byte from [SI] into AL, increment SI
    cmp al, 0       ; Check if null terminator
    je .done
    int 0x10        ; Call BIOS interrupt to print character
    jmp .loop

.done:
    popa
    ret

; Data section
boot_drive      db 0
msg_boot        db 'CoreX Bootloader v1.0', 0x0D, 0x0A, 'Loading kernel...', 0x0D, 0x0A, 0
msg_loaded      db 'Kernel loaded! Jumping to kernel...', 0x0D, 0x0A, 0
msg_error       db 'Disk read error!', 0x0D, 0x0A, 0

; Pad to 510 bytes and add boot signature
times 510-($-$$) db 0
dw 0xAA55               ; Boot signature
