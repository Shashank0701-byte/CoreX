# CoreX OS

A simple x86 operating system written from scratch, featuring a custom bootloader and kernel.

## 📁 Project Structure

```
CoreX/
├── bootloader/     # Boot sector code (NASM assembly)
├── kernel/         # Kernel source code (C)
├── drivers/        # Device drivers
├── lib/            # Utility libraries
├── include/        # Header files
└── Makefile        # Build system
```

## 🛠️ Prerequisites

To build and run CoreX, you need:

1. **NASM** - Netwide Assembler for bootloader compilation
   - Windows: Download from [nasm.us](https://www.nasm.us) or `choco install nasm`
   - Linux: `sudo apt-get install nasm`

2. **GCC Cross-Compiler** - i686-elf-gcc for kernel compilation
   - Follow the [OSDev Wiki guide](https://wiki.osdev.org/GCC_Cross-Compiler)

3. **QEMU** - For testing and emulation
   - Windows: Download from [qemu.org](https://www.qemu.org/download/#windows) or `choco install qemu`
   - Linux: `sudo apt-get install qemu-system-x86`

## 🚀 Building

### Build the bootloader:
```bash
nasm -f bin bootloader/boot.asm -o bootloader/boot.bin
```

### Build the kernel (when implemented):
```bash
make
```

### Clean build artifacts:
```bash
make clean
```

## 🧪 Testing

### Test the bootloader in QEMU:
```bash
qemu-system-i386 -drive format=raw,file=bootloader/boot.bin
```

### Run the full OS (when kernel is implemented):
```bash
make run
```

## 📝 Current Status

- ✅ **Bootloader**: 512-byte boot sector that prints "Bootloader OK" and halts
- ⏳ **Kernel**: Coming soon
- ⏳ **Drivers**: Coming soon

## 📚 Resources

- [OSDev Wiki](https://wiki.osdev.org/)
- [NASM Documentation](https://www.nasm.us/docs.php)
- [Intel x86 Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)

## 📄 License

MIT License - See LICENSE file for details

## 🤝 Contributing

Contributions are welcome! Feel free to open issues or submit pull requests.
