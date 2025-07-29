CC = gcc
LD = ld
AS = nasm

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld
ASFLAGS = -f elf32

ISO_DIR = iso
BUILD_DIR = build

KERNEL_ELF = $(BUILD_DIR)/kernel.elf
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
ISO_FILE = cellos.iso

SRC_C = kernel.c
SRC_ASM = kernel_entry.asm

OBJ = $(BUILD_DIR)/kernel.o $(BUILD_DIR)/kernel_entry.o

all: $(ISO_FILE)

# Compile C code
$(BUILD_DIR)/kernel.o: kernel.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel_entry.o: kernel_entry.asm
	mkdir -p $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

$(KERNEL_BIN): $(OBJ) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJ)

$(ISO_FILE): $(KERNEL_BIN)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/kernel.bin
	cp grub/grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(ISO_DIR)

run: $(ISO_FILE)
	qemu-system-i386 -cdrom $(ISO_FILE)

clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR) $(ISO_FILE)

.PHONY: all clean run
