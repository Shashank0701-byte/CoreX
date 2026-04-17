; CoreX bootloader — CHS reads only (INT 13h AH=02). Extended LBA (AH=42) can hang on QEMU.
; KERNEL_SECTORS from kernel/build_info.inc (Makefile pads kernel to whole sectors).

[BITS 16]
[ORG 0x7C00]

%include "build_info.inc"

KERNEL_OFFSET equ 0x10000      ; Above MBR (0x7C00) — prevents self-overwrite
KERNEL_SEG    equ 0x1000        ; Real-mode segment: 0x1000:0x0000 = 0x10000
SPT equ 63
HPC equ 16

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov [boot_drive], dl

    mov si, msg_boot
    call print_string

    call load_kernel

    mov si, msg_loaded
    call print_string

    call enable_a20

    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp CODE_SEG:protected_mode_start

; LBA -> CHS (63 SPT, 16 heads): quot=LBA/63 = C*16+H; S = (LBA%63)+1
; INT 13h AH=02: AL=count, CH/CL/DH, DL=drive, ES:BX=buffer
load_kernel:
    pusha
    xor ax, ax
    mov ds, ax

    mov ax, KERNEL_SEG
    mov es, ax                  ; ES = 0x1000
    xor bx, bx                  ; BX = 0 → ES:BX = 0x1000:0x0000 = physical 0x10000
    mov word [sectors_left], KERNEL_SECTORS
    mov si, 1

.read_loop:
    ; LBA in SI -> CH, CL, DH (save cyl high byte before pop overwrites ax)
    mov ax, si
    xor dx, dx
    mov cx, SPT
    div cx
    push dx
    mov cx, HPC
    xor dx, dx
    div cx
    mov ch, al
    mov bp, ax
    mov dh, dl
    pop ax
    mov cl, al
    inc cl
    mov ax, bp
    shr ax, 8
    and al, 0x03
    shl al, 6
    or cl, al

    mov ah, 0x02
    mov al, 1
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    add bx, 512
    jnc .no_es
    mov ax, es
    add ax, 0x1000
    mov es, ax
.no_es:
    inc si
    dec word [sectors_left]
    jnz .read_loop

    popa
    ret

disk_error:
    xor ax, ax
    mov ds, ax
    mov si, msg_error
    call print_string
    cli
    hlt

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

enable_a20:
    pusha
    call .wi
    mov al, 0xAD
    out 0x64, al
    call .wi
    mov al, 0xD0
    out 0x64, al
    call .wo
    in al, 0x60
    push ax
    call .wi
    mov al, 0xD1
    out 0x64, al
    call .wi
    pop ax
    or al, 2
    out 0x60, al
    call .wi
    mov al, 0xAE
    out 0x64, al
    call .wi
    popa
    ret
.wi:
    in al, 0x64
    test al, 2
    jnz .wi
    ret
.wo:
    in al, 0x64
    test al, 1
    jz .wo
    ret

gdt_start:
gdt_null:    dd 0, 0
gdt_code:
    dw 0xFFFF, 0x0
    db 0x0, 10011010b, 11001111b, 0x0
gdt_data:
    dw 0xFFFF, 0x0
    db 0x0, 10010010b, 11001111b, 0x0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

[BITS 32]
protected_mode_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x90000
    mov esp, ebp
    mov ebx, 0xB8000
    mov byte [ebx], 'P'
    mov byte [ebx+1], 0x0F
    mov byte [ebx+2], 'M'
    mov byte [ebx+3], 0x0F
    jmp KERNEL_OFFSET

[BITS 16]

boot_drive    db 0
sectors_left  dw 0

msg_boot   db 'CoreX Bootloader v2.0', 0x0D, 0x0A, 'Loading kernel...', 0x0D, 0x0A, 0
msg_loaded db 'Kernel loaded! Switching to protected mode...', 0x0D, 0x0A, 0
msg_error  db 'Disk read error!', 0x0D, 0x0A, 0

times 510-($-$$) db 0
dw 0xAA55
