#    GSOS - a simple OS written from scratch.
#    Copyright (C) 2026  Vadim Gladushev
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.
ASM  := nasm
CC16 := ../watcom/binl/wcc
LD16 := ../watcom/binl/wlink
CC32 := gcc
LD32 := gcc
CC64 := gcc
LD64 := gcc

SRC_DIR   := src
BUILD_DIR := build

STAGE2_CFLAGS16  := -4 -d3 -s -wx -ms -zl -zq -fr=$(BUILD_DIR)/
STAGE2_NASMFLAGS := -f obj

KERNEL_CFLAGS32 := -m32 -fno-pie -fno-pic -ffreestanding -nostdinc -nostdlib -mno-red-zone -fno-stack-protector -mgeneral-regs-only -mno-sse -mno-mmx -mno-avx -Wall -O2 -fno-builtin -I$(SRC_DIR)/kernel/include -I$(SRC_DIR)/kernel/include/arch/x86
KERNEL_LDFLAGS32 := -T $(SRC_DIR)/kernel/linker32.ld -nostdlib -static -m32 -Wl,-Map=$(BUILD_DIR)/kernel32.map -Wl,-no-pie -Wl,--nmagic
KERNEL_NASMFLAGS32 := -f elf32

KERNEL_CFLAGS64    := -m64 -fno-pie -fno-pic -ffreestanding -nostdinc -nostdlib -mno-red-zone -fno-stack-protector -mgeneral-regs-only -mno-sse -mno-mmx -mno-avx -Wall -O2 -fno-builtin -I$(SRC_DIR)/kernel/include -I$(SRC_DIR)/kernel/include/arch/x86_64
KERNEL_LDFLAGS64   := -T $(SRC_DIR)/kernel/linker64.ld -nostdlib -static -m64 -Wl,-no-pie -Wl,--nmagic
KERNEL_NASMFLAGS64 := -f elf64

QEMU_RUN := qemu-system-x86_64 -accel kvm -drive file=$(BUILD_DIR)/main_floppy.img,format=raw,if=floppy


.PHONY: all run floppy_image kernel32 kernel64 bootloader stage1 stage2 clean always

run: floppy_image
	$(QEMU_RUN)


floppy_image: $(BUILD_DIR)/main_floppy.img
$(BUILD_DIR)/main_floppy.img: bootloader kernel32 kernel64
	dd if=/dev/zero of=$(BUILD_DIR)/main_floppy.img bs=512 count=2880
	mkfs.fat -F 12 -n "MYOS" $(BUILD_DIR)/main_floppy.img
	dd if=$(BUILD_DIR)/stage1.bin of=$(BUILD_DIR)/main_floppy.img conv=notrunc
	mcopy -i $(BUILD_DIR)/main_floppy.img $(BUILD_DIR)/stage2.bin "::stage2.bin"
	mcopy -i $(BUILD_DIR)/main_floppy.img $(BUILD_DIR)/kernel32.bin "::kernel.bin"
	mcopy -i $(BUILD_DIR)/main_floppy.img $(BUILD_DIR)/kernel64.bin "::kernel64.bin"


bootloader: stage1 stage2


stage1: $(BUILD_DIR)/stage1.bin
$(BUILD_DIR)/stage1.bin: always
	mkdir -p $(dir $@)
	$(ASM) $(SRC_DIR)/bootloader/stage1/boot.nasm -f bin -o $(BUILD_DIR)/stage1.bin


stage2: $(BUILD_DIR)/stage2.bin
STAGE2_SOURCES_C   := $(wildcard $(SRC_DIR)/bootloader/stage2/*.c)
STAGE2_SOURCES_ASM := $(wildcard $(SRC_DIR)/bootloader/stage2/*.nasm)
STAGE2_OBJECTS_C   = $(patsubst %.c, $(BUILD_DIR)/stage2/c/%.obj, $(notdir $(STAGE2_SOURCES_C)))
STAGE2_OBJECTS_ASM = $(patsubst %.nasm, $(BUILD_DIR)/stage2/nasm/%.obj, $(notdir $(STAGE2_SOURCES_ASM)))

$(BUILD_DIR)/stage2.bin: $(STAGE2_OBJECTS_ASM) $(STAGE2_OBJECTS_C)
	mkdir -p $(dir $@)
	$(LD16) NAME $@ FILE \{ $(STAGE2_OBJECTS_ASM) $(STAGE2_OBJECTS_C) \} OPTION MAP=$(BUILD_DIR)/stage2.map @$(SRC_DIR)/bootloader/stage2/build_stage2.lnk

$(BUILD_DIR)/stage2/c/%.obj: $(SRC_DIR)/bootloader/stage2/%.c
	mkdir -p $(dir $@)
	$(CC16) $(STAGE2_CFLAGS16) $(GENERIC_CFLAGS) -fo=$@ $<

$(BUILD_DIR)/stage2/nasm/%.obj: $(SRC_DIR)/bootloader/stage2/%.nasm
	mkdir -p $(dir $@)
	$(ASM) $(STAGE2_NASMFLAGS) -o $@ $<


kernel32: $(BUILD_DIR)/kernel32.bin
KERNEL32_SOURCES_C   := $(shell find $(SRC_DIR)/kernel/sources -name '*.c' -not -path "*/x86_64/*")
KERNEL32_SOURCES_ASM := $(shell find $(SRC_DIR)/kernel/sources -name '*.nasm' -not -path "*/x86_64/*")
KERNEL32_OBJECTS_C   := $(patsubst $(SRC_DIR)/kernel/sources/%.c, $(BUILD_DIR)/kernel32/c/%.o, $(KERNEL32_SOURCES_C))
KERNEL32_OBJECTS_ASM := $(patsubst $(SRC_DIR)/kernel/sources/%.nasm, $(BUILD_DIR)/kernel32/nasm/%.o, $(KERNEL32_SOURCES_ASM))

$(BUILD_DIR)/kernel32.bin: $(KERNEL32_OBJECTS_ASM) $(KERNEL32_OBJECTS_C)
	$(LD32) $(KERNEL_LDFLAGS32) -Wl,--oformat=binary -o $@ $^

$(BUILD_DIR)/kernel32/c/%.o: $(SRC_DIR)/kernel/sources/%.c
	mkdir -p $(dir $@)
	$(CC32) $(KERNEL_CFLAGS32) -c -I$(subst /sources/,/include/,$(dir $<)) $< -o $@

$(BUILD_DIR)/kernel32/nasm/%.o: $(SRC_DIR)/kernel/sources/%.nasm
	mkdir -p $(dir $@)
	$(ASM) $(KERNEL_NASMFLAGS32) -o $@ $<


kernel64: $(BUILD_DIR)/kernel64.bin
KERNEL64_SOURCES_C   := $(shell find $(SRC_DIR)/kernel/sources -name '*.c' -not -path "*/x86/*")
KERNEL64_SOURCES_ASM := $(shell find $(SRC_DIR)/kernel/sources -name '*.nasm' -not -path "*/x86/*")
KERNEL64_OBJECTS_C   := $(patsubst $(SRC_DIR)/kernel/sources/%.c, $(BUILD_DIR)/kernel64/c/%.o, $(KERNEL64_SOURCES_C))
KERNEL64_OBJECTS_ASM := $(patsubst $(SRC_DIR)/kernel/sources/%.nasm, $(BUILD_DIR)/kernel64/nasm/%.o, $(KERNEL64_SOURCES_ASM))

$(BUILD_DIR)/kernel64.bin: $(KERNEL64_OBJECTS_ASM) $(KERNEL64_OBJECTS_C)
	$(LD64) $(KERNEL_LDFLAGS64) -Wl,--oformat=binary -o $@ $^

$(BUILD_DIR)/kernel64/c/%.o: $(SRC_DIR)/kernel/sources/%.c
	mkdir -p $(dir $@)
	$(CC64) $(KERNEL_CFLAGS64) -c -I$(subst /sources/,/include/,$(dir $<)) $< -o $@

$(BUILD_DIR)/kernel64/nasm/%.o: $(SRC_DIR)/kernel/sources/%.nasm
	mkdir -p $(dir $@)
	$(ASM) $(KERNEL_NASMFLAGS64) -o $@ $<

clean:
	rm -rf $(BUILD_DIR)/*
