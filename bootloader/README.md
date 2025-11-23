# Build Instructions for CoreX Bootloader

## Prerequisites
You need to install NASM (Netwide Assembler) to compile the bootloader.

### Windows Installation:
1. Download NASM from: https://www.nasm.us/pub/nasm/releasebuilds/
2. Install and add to PATH, or use: `choco install nasm` (if you have Chocolatey)

### Linux Installation:
```bash
sudo apt-get install nasm  # Debian/Ubuntu
sudo yum install nasm      # RedHat/CentOS
```

## Building the Bootloader

Once NASM is installed, run:
```bash
nasm -f bin bootloader/boot.asm -o bootloader/boot.bin
```

Or use the Makefile:
```bash
make bootloader
```

## Testing the Bootloader

Test in QEMU:
```bash
qemu-system-i386 -drive format=raw,file=bootloader/boot.bin
```

Or use the Makefile:
```bash
make test-bootloader
```

## Verifying Boot Signature

Check that the file is exactly 512 bytes with proper boot signature:
```bash
hexdump -C bootloader/boot.bin | tail
```

The last two bytes should be `55 aa` (0xAA55 in little-endian).
