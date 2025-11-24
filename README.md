# CoreX OS

A simple x86 operating system written from scratch in C and Assembly, featuring a custom bootloader, protected mode kernel, and hardware interrupt handling.

![OS Type](https://img.shields.io/badge/OS-x86-blue)
![Language](https://img.shields.io/badge/Language-C%20%2B%20Assembly-green)
![License](https://img.shields.io/badge/License-MIT-yellow)

## 🎯 Features

### Bootloader
- ✅ 512-byte NASM bootloader with boot signature
- ✅ Loads kernel from disk (sector 2+)
- ✅ Switches from 16-bit real mode to 32-bit protected mode
- ✅ Sets up GDT (Global Descriptor Table)
- ✅ Enables A20 line for extended memory access
- ✅ VGA text mode initialization

### Kernel
- ✅ Written in C with minimal assembly
- ✅ 32-bit protected mode execution
- ✅ VGA text buffer output (0xB8000)
- ✅ Screen scrolling support
- ✅ Custom print functions (print, putchar, print_hex)

### Interrupt Handling
- ✅ IDT (Interrupt Descriptor Table) with 256 entries
- ✅ 32 CPU exception handlers (divide by zero, page fault, etc.)
- ✅ 16 hardware IRQ handlers (IRQ0-IRQ15)
- ✅ PIC (Programmable Interrupt Controller) remapping
- ✅ PIT (Programmable Interval Timer) at 100 Hz
- ✅ Exception messages with interrupt number and error code

## 📁 Project Structure

```
CoreX/
├── bootloader/         # Boot sector code
│   ├── boot.asm       # NASM bootloader (512 bytes)
│   └── README.md      # Bootloader documentation
├── kernel/            # Kernel source code
│   ├── kernel.c       # Main kernel entry point
│   ├── kernel_stub.asm # Assembly entry stub
│   ├── idt.c          # Interrupt Descriptor Table
│   ├── isr.asm        # Interrupt Service Routines
│   ├── pic.c          # PIC controller
│   ├── timer.c        # PIT timer
│   └── linker.ld      # Linker script
├── include/           # Header files
│   ├── idt.h
│   ├── pic.h
│   ├── timer.h
│   └── stdint.h
├── drivers/           # Device drivers (future)
├── lib/               # Utility libraries (future)
├── Makefile           # Build system
├── README.md          # This file
└── LICENSE            # MIT License
```

## 🛠️ Prerequisites

### Required Tools

1. **NASM** - Netwide Assembler
   ```bash
   # Windows
   choco install nasm
   
   # Linux
   sudo apt-get install nasm
   ```

2. **GCC** - GNU Compiler Collection (32-bit support)
   ```bash
   # Linux
   sudo apt-get install gcc-multilib
   ```

3. **QEMU** - For testing and emulation
   ```bash
   # Windows
   choco install qemu
   
   # Linux
   sudo apt-get install qemu-system-x86
   ```

## 🚀 Building and Running

### Quick Start

```bash
# Build bootloader
nasm -f bin bootloader/boot.asm -o bootloader/boot.bin

# Build kernel
nasm -f elf32 kernel/kernel_stub.asm -o kernel/kernel_stub.o
nasm -f elf32 kernel/isr.asm -o kernel/isr.o
gcc -m32 -ffreestanding -fno-pie -O2 -Wall -Wextra -Iinclude -nostdlib -nostdinc -c kernel/kernel.c -o kernel/kernel.o
gcc -m32 -ffreestanding -fno-pie -O2 -Wall -Wextra -Iinclude -nostdlib -nostdinc -c kernel/idt.c -o kernel/idt.o
gcc -m32 -ffreestanding -fno-pie -O2 -Wall -Wextra -Iinclude -nostdlib -nostdinc -c kernel/pic.c -o kernel/pic.o
gcc -m32 -ffreestanding -fno-pie -O2 -Wall -Wextra -Iinclude -nostdlib -nostdinc -c kernel/timer.c -o kernel/timer.o

# Link kernel
ld -m i386pe -T kernel/linker.ld -o kernel/kernel_c.tmp kernel/kernel_stub.o kernel/kernel.o kernel/idt.o kernel/isr.o kernel/pic.o kernel/timer.o --entry=_start
objcopy -O binary kernel/kernel_c.tmp kernel/kernel_c.bin

# Create OS image
cat bootloader/boot.bin kernel/kernel_c.bin > os-image.bin

# Run in QEMU
qemu-system-i386 -drive format=raw,file=os-image.bin
```

### Using Makefile (if make is available)

```bash
make os-image-c    # Build OS image
make run-c-os      # Build and run in QEMU
make clean         # Clean build artifacts
```

## 🎮 What You'll See

When you run CoreX in QEMU, you'll see:

```
=================================
    CoreX Kernel v3.2 (C)
=================================

Running in 32-bit Protected Mode
GDT loaded successfully
A20 line enabled
VGA text mode initialized

Initializing IDT...
IDT initialized with 32 exception handlers and 16 IRQ handlers

Kernel features:
  - C language kernel
  - VGA text output
  - Screen scrolling support
  - IDT with exception handlers

System initialized successfully!

Kernel running. System stable.
```

## 🔧 Current Status

### Working
- ✅ Bootloader loads and executes kernel
- ✅ Protected mode with GDT
- ✅ VGA text output
- ✅ IDT with exception handlers
- ✅ PIC and timer code (disabled by default)

### In Progress
- ⏳ Hardware interrupts (timer disabled to prevent boot issues)
- ⏳ Keyboard input
- ⏳ Memory management

### Planned
- 📋 File system
- 📋 User mode
- 📋 Multitasking

## 🧪 Testing Exception Handlers

To test the exception handling, uncomment one of these lines in `kernel/kernel.c`:

```c
// Test divide by zero
int x = 1 / 0;

// Test invalid opcode
__asm__ __volatile__("ud2");
```

You'll see the exception handler display the error details!

## 📚 Resources

- [OSDev Wiki](https://wiki.osdev.org/)
- [NASM Documentation](https://www.nasm.us/docs.php)
- [Intel x86 Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [Writing a Simple Operating System from Scratch](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf)

## 📄 License

MIT License - See [LICENSE](LICENSE) file for details

## 🤝 Contributing

Contributions are welcome! Feel free to:
- Report bugs
- Suggest features
- Submit pull requests

## 👨‍💻 Author

Built as a learning project to understand OS fundamentals from the ground up.

---

**Note:** This is an educational OS project. It's not intended for production use.
