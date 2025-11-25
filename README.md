# CoreX OS - A Custom x86 Operating System

![OS Development](https://img.shields.io/badge/OS-Development-blue)
![Language](https://img.shields.io/badge/Language-C%20%7C%20Assembly-green)
![Architecture](https://img.shields.io/badge/Architecture-x86-orange)

A custom 32-bit operating system built from scratch in C and x86 Assembly, demonstrating low-level systems programming and OS development concepts.

## 🚀 Features

### Core System
- **Custom Bootloader** - Written in x86 Assembly, loads kernel into memory
- **32-bit Protected Mode** - Full transition from Real Mode to Protected Mode
- **GDT (Global Descriptor Table)** - Proper memory segmentation setup
- **IDT (Interrupt Descriptor Table)** - 32 exception handlers + 16 IRQ handlers
- **PIC (Programmable Interrupt Controller)** - IRQ remapping to avoid conflicts

### Memory Management
- **Physical Memory Manager (PMM)** - Bitmap-based page frame allocator
- **Paging Support** - 4KB page tables with identity mapping
- **Dynamic Memory Allocation** - Page-level memory allocation and deallocation

### I/O & Drivers
- **VGA Text Mode Driver** - 80x25 color text output with scrolling
- **PS/2 Keyboard Driver** - Scancode to ASCII conversion with shift/caps support
- **PIT Timer** - Programmable Interval Timer for time-based operations

### User Interface
- **Interactive Shell** - Command-line interface with multiple built-in commands
  - `help` - Display available commands
  - `clear` - Clear the screen
  - `version` - Show OS version information
  - `meminfo` - Display memory statistics
  - `echo` - Echo text to screen

### File System
- **In-Memory File System** - Simple file creation, reading, and deletion
- **Directory Support** - Basic directory structure

## 🏗️ Architecture

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
│   │   ├── VGA (kernel.c)
│   │   ├── Keyboard (keyboard.c)
│   │   └── Timer (timer.c)
│   └── System
│       ├── IDT (idt.c)
│       ├── PIC (pic.c)
│       ├── Shell (shell.c)
│       └── File System (fs.c)
```

## 🛠️ Building

### Prerequisites
- **NASM** - Netwide Assembler
- **GCC** - GNU Compiler Collection (with `-m32` support)
- **LD** - GNU Linker
- **QEMU** - For testing (qemu-system-i386)

### Build Commands

```bash
# Build bootloader
nasm -f bin bootloader/boot.asm -o bootloader/boot.bin

# Build kernel
nasm -f elf32 kernel/kernel_stub.asm -o kernel/kernel_stub.o
nasm -f elf32 kernel/isr.asm -o kernel/isr.o
gcc -m32 -ffreestanding -c kernel/*.c -Iinclude
ld -m i386pe -T kernel/linker.ld -o kernel/kernel.bin <objects>

# Create OS image
cat bootloader/boot.bin kernel/kernel.bin > os-image.bin

# Run in QEMU
qemu-system-i386 -drive format=raw,file=os-image.bin
```

Or use the provided Makefile:
```bash
make all
make run
```

## 🎯 Technical Highlights

### Low-Level Programming
- Direct hardware manipulation via port I/O
- Inline assembly for critical operations
- Manual stack and register management
- Interrupt handling in Assembly

### Systems Concepts
- Bootloader development and disk I/O
- CPU mode transitions (Real → Protected)
- Memory segmentation and paging
- Interrupt-driven I/O
- Device driver development

### Code Quality
- Modular architecture with clear separation of concerns
- Comprehensive comments and documentation
- Proper error handling
- Clean code structure following OS development best practices

## 📚 Learning Outcomes

This project demonstrates proficiency in:
- **Low-level programming** (C, x86 Assembly)
- **Operating system concepts** (memory management, process scheduling, I/O)
- **Hardware interaction** (BIOS, VGA, keyboard, timer)
- **Systems architecture** (x86 architecture, interrupts, paging)
- **Debugging** (QEMU, GDB, serial debugging)

## 🔧 Current Status

**Working:**
- ✅ Bootloader successfully loads kernel
- ✅ Protected mode transition
- ✅ IDT with exception and IRQ handlers
- ✅ VGA text output with scrolling
- ✅ Physical memory manager
- ✅ Shell command system
- ✅ File system operations

**In Development:**
- 🔨 Interactive keyboard input (hardware interrupt handling)
- 🔨 Multi-tasking scheduler
- 🔨 Virtual file system
- 🔨 Network stack

## 📸 Screenshots

![CoreX OS Boot](docs/screenshot-boot.png)
*CoreX OS booting and initializing subsystems*

![Shell Interface](docs/screenshot-shell.png)
*Interactive shell with command support*

## 🎓 Educational Value

This project was built as a learning exercise to understand:
- How operating systems work at the lowest level
- The boot process from power-on to running code
- Memory management and virtual memory
- Hardware abstraction and device drivers
- System call interfaces

## 📖 Resources Used

- [OSDev Wiki](https://wiki.osdev.org/)
- Intel x86 Architecture Manuals
- "Operating Systems: Three Easy Pieces" by Remzi H. Arpaci-Dusseau
- Various OS development tutorials and documentation

## 📝 License

This project is open source and available under the MIT License.

## 👤 Author

**Your Name**
- GitHub: [Shashank0701-byte]([https://github.com/yourusername](https://github.com/Shashank0701-byte))
- LinkedIn: [Shashank_Chakraborty]([https://linkedin.com/in/yourprofile)](https://www.linkedin.com/in/shashank-chakraborty/)

---

*Built with ❤️ and lots of debugging*
