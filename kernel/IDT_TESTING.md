# IDT Exception Testing Guide

## Testing CPU Exceptions

The CoreX kernel now has a fully functional IDT with 32 CPU exception handlers.

### To Test Exceptions:

Edit `kernel/kernel.c` and uncomment one of the test lines in `kmain()`:

#### Test 1: Divide By Zero Exception
```c
print("Testing divide by zero exception...\n");
int x = 1 / 0;  // This will trigger exception 0
```

#### Test 2: Invalid Opcode Exception
```c
print("Testing invalid opcode exception...\n");
__asm__ __volatile__("ud2");  // This will trigger exception 6
```

### Expected Output:

When an exception occurs, you'll see:

```
!!! EXCEPTION !!!
Exception: Division By Zero
Interrupt Number: 0x00000000
Error Code: 0x00000000

System Halted.
```

### Supported Exceptions:

- 0: Division By Zero
- 1: Debug
- 2: Non Maskable Interrupt
- 3: Breakpoint
- 4: Overflow
- 5: Bound Range Exceeded
- 6: Invalid Opcode
- 7: Device Not Available
- 8: Double Fault
- 9: Coprocessor Segment Overrun
- 10: Invalid TSS
- 11: Segment Not Present
- 12: Stack-Segment Fault
- 13: General Protection Fault
- 14: Page Fault
- 15-31: Various reserved and CPU exceptions

### Rebuild After Changes:

```bash
gcc -m32 -ffreestanding -fno-pie -O2 -Wall -Wextra -Iinclude -nostdlib -nostdinc -c kernel/kernel.c -o kernel/kernel.o
ld -m i386pe -T kernel/linker.ld -o kernel/kernel_c.tmp kernel/kernel_stub.o kernel/kernel.o kernel/idt.o kernel/isr.o --entry=_start
objcopy -O binary kernel/kernel_c.tmp kernel/kernel_c.bin
cat bootloader/boot.bin kernel/kernel_c.bin > os-image.bin
"/c/Program Files/qemu/qemu-system-i386.exe" -drive format=raw,file=os-image.bin
```
