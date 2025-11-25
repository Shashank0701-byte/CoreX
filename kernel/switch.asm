; Task switching assembly code
; Saves and restores CPU context

[BITS 32]

; void switch_task(uint32_t* old_esp, uint32_t new_esp)
global _switch_task
_switch_task:
    ; Save old task context
    push ebp
    push esi
    push edi
    push ebx
    push edx
    push ecx
    push eax
    
    ; Save old ESP
    mov eax, [esp + 32]     ; Get old_esp parameter (after 7 pushes + ret addr)
    mov [eax], esp          ; Save current ESP
    
    ; Load new ESP
    mov esp, [esp + 36]     ; Get new_esp parameter
    
    ; Restore new task context
    pop eax
    pop ecx
    pop edx
    pop ebx
    pop edi
    pop esi
    pop ebp
    
    ret
