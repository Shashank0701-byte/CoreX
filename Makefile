# CoreX OS Makefile
# Simple x86 OS build system

# Compiler and tools
CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-ld
NASM = nasm
QEMU = qemu-system-i386

# Flags
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -Iinclude
LDFLAGS = -m elf_i386 -T linker.ld
ASFLAGS = --32

# Directories
BOOTLOADER_DIR = bootloader
KERNEL_DIR = kernel
DRIVERS_DIR = drivers
LIB_DIR = lib
INCLUDE_DIR = include

# Output
BOOTLOADER_BIN = bootloader/boot.bin
KERNEL_ENTRY_BIN = kernel/kernel_entry.bin
KERNEL_BIN = kernel.bin
OS_IMAGE = os-image.bin
ISO_DIR = isodir
ISO_FILE = CoreX.iso

# Source files
KERNEL_SOURCES = $(wildcard $(KERNEL_DIR)/*.c)
DRIVER_SOURCES = $(wildcard $(DRIVERS_DIR)/*.c)
LIB_SOURCES = $(wildcard $(LIB_DIR)/*.c)
ASM_SOURCES = $(wildcard $(KERNEL_DIR)/*.s)

# Object files
KERNEL_OBJECTS = $(KERNEL_SOURCES:.c=.o)
DRIVER_OBJECTS = $(DRIVER_SOURCES:.c=.o)
LIB_OBJECTS = $(LIB_SOURCES:.c=.o)
ASM_OBJECTS = $(ASM_SOURCES:.s=.o)

ALL_OBJECTS = $(ASM_OBJECTS) $(KERNEL_OBJECTS) $(DRIVER_OBJECTS) $(LIB_OBJECTS)

# Default target
all: $(KERNEL_BIN)

# Build kernel binary
$(KERNEL_BIN): $(ALL_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile assembly files
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# Run in QEMU
run: $(KERNEL_BIN)
	$(QEMU) -kernel $(KERNEL_BIN)

# Run in QEMU with debugging
debug: $(KERNEL_BIN)
	$(QEMU) -kernel $(KERNEL_BIN) -s -S

# Clean build artifacts
clean:
	rm -f $(ALL_OBJECTS) $(KERNEL_BIN) $(BOOTLOADER_BIN) $(KERNEL_ENTRY_BIN) $(OS_IMAGE)
	rm -rf $(ISO_DIR) $(ISO_FILE)

# Create bootable ISO (optional)
iso: $(KERNEL_BIN)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/
	echo 'set timeout=0' > $(ISO_DIR)/boot/grub/grub.cfg
	echo 'set default=0' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo 'menuentry "CoreX OS" {' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '    multiboot /boot/$(KERNEL_BIN)' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '    boot' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '}' >> $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO_FILE) $(ISO_DIR)

# Build bootloader
bootloader: $(BOOTLOADER_BIN)

$(BOOTLOADER_BIN): bootloader/boot.asm
	$(NASM) -f bin $< -o $@

# Build kernel entry
kernel-entry: $(KERNEL_ENTRY_BIN)

$(KERNEL_ENTRY_BIN): kernel/kernel_entry.asm
	$(NASM) -f bin $< -o $@

# Create bootable OS image (bootloader + kernel)
os-image: $(OS_IMAGE)

$(OS_IMAGE): $(BOOTLOADER_BIN) $(KERNEL_ENTRY_BIN)
	cat $(BOOTLOADER_BIN) $(KERNEL_ENTRY_BIN) > $(OS_IMAGE)

# Run complete OS in QEMU
run-os: $(OS_IMAGE)
	$(QEMU) -drive format=raw,file=$(OS_IMAGE)

# Test bootloader only (legacy)
test-bootloader: $(BOOTLOADER_BIN)
	$(QEMU) -drive format=raw,file=$(BOOTLOADER_BIN)

.PHONY: all run debug clean iso bootloader kernel-entry os-image run-os test-bootloader
