; Enhanced x86 Bootloader with Protected Mode
; Loads kernel from disk, switches to 32-bit protected mode, and jumps to kernel
; Must be exactly 512 bytes with boot signature 0xAA55

[BITS 16]
[ORG 0x7C00]

KERNEL_OFFSET equ 0x1000    ; Memory location to load kernel
KERNEL_SECTORS equ 20       ; Number of sectors to load (10KB)

start:
    ; Initialize segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    ; Save boot drive number
    mov [boot_drive], dl

    ; Print bootloader message
    mov si, msg_boot
    call print_string

    ; Load kernel from disk
    call load_kernel

    ; Print loaded message
    mov si, msg_loaded
    call print_string

    ; Enable A20 line
    call enable_a20

    ; Load GDT
    cli
    lgdt [gdt_descriptor]

    ; Switch to protected mode
    mov eax, cr0
    or eax, 0x1         ; Set PE (Protection Enable) bit
    mov cr0, eax

    ; Far jump to flush pipeline and enter 32-bit code
    jmp CODE_SEG:protected_mode_start

; ============ 16-bit Functions ============

; Function: load_kernel
load_kernel:
    pusha
    
    mov ah, 0x02
    mov al, KERNEL_SECTORS
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    mov bx, KERNEL_OFFSET
    
    int 0x13
    jc disk_error
    
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

; Function: enable_a20
; Enables A20 line using keyboard controller method
enable_a20:
    pusha
    
    call .wait_input
    mov al, 0xAD        ; Disable keyboard
    out 0x64, al
    
    call .wait_input
    mov al, 0xD0        ; Read from input
    out 0x64, al
    
    call .wait_output
    in al, 0x60
    push ax
    
    call .wait_input
    mov al, 0xD1        ; Write to output
    out 0x64, al
    
    call .wait_input
    pop ax
    or al, 2            ; Enable A20 bit
    out 0x60, al
    
    call .wait_input
    mov al, 0xAE        ; Enable keyboard
    out 0x64, al
    
    call .wait_input
    popa
    ret

.wait_input:
    in al, 0x64
    test al, 2
    jnz .wait_input
    ret

.wait_output:
    in al, 0x64
    test al, 1
    jz .wait_output
    ret

; ============ GDT (Global Descriptor Table) ============

gdt_start:

gdt_null:               ; Null descriptor (required)
    dd 0x0
    dd 0x0

gdt_code:               ; Code segment descriptor
    dw 0xFFFF           ; Limit (bits 0-15)
    dw 0x0              ; Base (bits 0-15)
    db 0x0              ; Base (bits 16-23)
    db 10011010b        ; Access byte: present, ring 0, code, executable, readable
    db 11001111b        ; Flags + Limit (bits 16-19): 4KB granularity, 32-bit
    db 0x0              ; Base (bits 24-31)

gdt_data:               ; Data segment descriptor
    dw 0xFFFF           ; Limit (bits 0-15)
    dw 0x0              ; Base (bits 0-15)
    db 0x0              ; Base (bits 16-23)
    db 10010010b        ; Access byte: present, ring 0, data, writable
    db 11001111b        ; Flags + Limit (bits 16-19): 4KB granularity, 32-bit
    db 0x0              ; Base (bits 24-31)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of GDT
    dd gdt_start                 ; Address of GDT

; GDT segment selectors
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; ============ 32-bit Protected Mode Code ============

[BITS 32]

protected_mode_start:
    ; Set up segment registers for protected mode
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Set up stack
    mov ebp, 0x90000
    mov esp, ebp

    ; Print protected mode message (VGA text mode)
    mov ebx, 0xB8000    ; VGA text buffer
    mov byte [ebx], 'P'
    mov byte [ebx+1], 0x0F
    mov byte [ebx+2], 'M'
    mov byte [ebx+3], 0x0F

    ; Jump to kernel
    jmp KERNEL_OFFSET

; ============ Data Section ============

[BITS 16]

boot_drive      db 0
msg_boot        db 'CoreX Bootloader v2.0', 0x0D, 0x0A, 'Loading kernel...', 0x0D, 0x0A, 0
msg_loaded      db 'Kernel loaded! Switching to protected mode...', 0x0D, 0x0A, 0
msg_error       db 'Disk read error!', 0x0D, 0x0A, 0

; Pad to 510 bytes and add boot signature
times 510-($-$$) db 0
dw 0xAA55
