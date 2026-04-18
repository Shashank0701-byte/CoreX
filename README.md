# CoreX OS - A Custom x86 Operating System

![OS Development](https://img.shields.io/badge/OS-Development-blue)
![Language](https://img.shields.io/badge/Language-C%20%7C%20Assembly-green)
![Architecture](https://img.shields.io/badge/Architecture-x86-orange)

A custom 32-bit operating system built from scratch in C and x86 Assembly. This project demonstrates low-level systems programming, hardware abstraction, and operating system development concepts.

## Features

### Core System
- **Custom Bootloader**: Written in x86 Assembly, responsible for loading the kernel into memory.
- **32-bit Protected Mode**: Complete transition from Real Mode to Protected Mode.
- **Global Descriptor Table (GDT)**: Proper memory segmentation and privilege level setup.
- **Interrupt Descriptor Table (IDT)**: Complete setup of 32 exception handlers and 16 IRQ handlers.
- **Programmable Interrupt Controller (PIC)**: IRQ remapping to avoid hardware conflicts.

### Memory Management
- **Physical Memory Manager (PMM)**: Bitmap-based page frame allocation.
- **Paging Support**: Implementation of 4KB page tables featuring identity mapping.
- **Dynamic Allocation**: Page-level memory allocation and deallocation systems.

### I/O and Drivers
- **VGA Text Mode Driver**: 80x25 hardware text output with scrolling, dynamic color support, and absolute coordinate rendering.
- **PS/2 Keyboard Driver**: Advanced scancode to ASCII conversion, including full support for Shift, Caps Lock, and extended scancodes (directional arrows).
- **PIT Timer**: Programmable Interval Timer configuration for time-based operations and process scheduling.
- **VGA Graphics**: Implementation of Mode 13h (320x200) for pixel plotting and geometric rendering demos.

### User Interface and Applications
- **Interactive Shell**: A robust command-line interface featuring:
  - System commands: `help`, `clear`, `version`, `sysinfo`, `reboot`.
  - Application commands: `calc` for integer arithmetic, `gfx` for graphics mode testing.
  - Subsystem checks: `tasks` for viewing the active scheduler queues.
  - Quality of Life: Command history tracking with arrow-key navigation.
- **Text Editor**: A full-screen interactive text editor (`edit`) with buffer management and ESC-to-save functionality.
- **Embedded Gaming**: A fully functional Snake game utilizing timer interrupts, absolute VGA positioning, and extended keyboard input.

### File System
- **In-Memory File System**: Capable of file creation (`touch`), reading (`cat`), updating (`edit`), and deletion (`rm`).
- **Directory Operations**: Listing capabilities via the `ls` command.

## Architecture

```
CoreX OS
├── Bootloader (Assembly)
│   ├── Stage 1: Boot sector (512 bytes)
│   └── Stage 2: Kernel loader
├── Kernel (C + Assembly)
│   ├── Core
│   │   ├── Entry point (kernel_stub.asm)
│   │   ├── Main kernel (kernel.c)
│   │   └── Interrupt handlers (isr.asm)
│   ├── Memory Management
│   │   ├── PMM (pmm.c)
│   │   └── Paging (paging.c)
│   ├── Drivers
│   │   ├── VGA (kernel.c / graphics.c)
│   │   ├── Keyboard (keyboard.c)
│   │   └── Timer (timer.c)
│   └── System
│       ├── IDT (idt.c)
│       ├── PIC (pic.c)
│       ├── Shell (shell.c)
│       ├── Scheduler (scheduler.c)
│       └── File System (fs.c)
```

## Building and Deployment

### Prerequisites
- **NASM**: Netwide Assembler
- **GCC**: GNU Compiler Collection (configured for `-m32` cross-compilation)
- **LD**: GNU Linker
- **QEMU**: Virtualization environment for testing (`qemu-system-i386`)

### Build Commands

Using the provided Makefile:
```bash
make all
make run-c-os
```

Manual compilation steps:
```bash
# Build bootloader
nasm -f bin bootloader/boot.asm -o bootloader/boot.bin

# Build kernel objects
nasm -f elf32 kernel/kernel_stub.asm -o kernel/kernel_stub.o
nasm -f elf32 kernel/isr.asm -o kernel/isr.o
gcc -m32 -ffreestanding -c kernel/*.c -Iinclude
ld -m i386pe -T kernel/linker.ld -o kernel/kernel.bin <objects>

# Create OS image
cat bootloader/boot.bin kernel/kernel.bin > os-image.bin

# Execute in QEMU
qemu-system-i386 -drive format=raw,file=os-image.bin
```

## Technical Highlights

- **Low-Level Execution**: Direct hardware manipulation utilizing port I/O and inline assembly for mission-critical operations.
- **Hardware Interaction**: Bootloader development, manual stack/register management, and interrupt-driven device interaction.
- **Software Engineering**: Modular architecture enforcing a strict separation of concerns, comprehensive documentation, and error handling protocols.

## Learning Outcomes

This project demonstrates practical proficiency in:
- Systems architecture (x86 architecture, interrupts, hardware abstraction).
- Operating system internals (memory management, multitasking, I/O processing).
- Low-level programming paradigms using C and Assembly.
- Systems-level debugging methodologies utilizing QEMU and GDB.

## Project Status

**Completed Objectives:**
- Bootloader initialization and protected mode transition.
- IDT configuration with exception and IRQ handlers.
- Advanced VGA text and graphics modes.
- Physical memory allocation mapping.
- Interrupt-driven keyboard input with shift state tracking.
- Preemptive multitasking scheduler implementation.
- In-memory file system and full-screen text editor.
- Shell command infrastructure with application support (Calculator, Snake).

## Resources

- OSDev Wiki
- Intel x86 Architecture Software Developer's Manual
- "Operating Systems: Three Easy Pieces" by Remzi H. Arpaci-Dusseau

## License

This project is open-source and distributed under the MIT License.

## Author

**Shashank Chakraborty**
- GitHub: [Shashank0701-byte](https://github.com/Shashank0701-byte)
- LinkedIn: [Shashank_Chakraborty](https://www.linkedin.com/in/shashank-chakraborty/)
