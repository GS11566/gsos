/*
    GSOS - a simple OS written from scratch.
    Copyright (C) 2026  Vadim Gladushev

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "idt/idt.h"
#include "idt/interrupt_handlers.h"
#include "drivers/portio.h"

struct idt_entry idt[256] __attribute__((aligned(16)));
struct idt_ptr idtp;

void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
    idt[num].offset_low  = base & 0xFFFF;
    idt[num].offset_mid  = (base >> 16) & 0xFFFF;
    idt[num].offset_high = (uint32_t)(base >> 32);
    
    idt[num].selector = sel;
    idt[num].ist = 0;              // NOTE: No interrupt stack table
    idt[num].type_attr = flags;    // e.g., 0x8E = present, ring 0, interrupt gate
    idt[num].zero = 0;             // reserved
}

void idt_load(void) {
    __asm__ __volatile__ (
        "lidt %0\n\t"
        "sti"
        :
        : "m" (idtp)
        : "memory", "cc"
    );
}

void idt_init(void) {
    idt_pic_remap();

    idtp.limit = sizeof(struct idt_entry) * 256 - 1;
    idtp.base  = (uint64_t)&idt;

    // CPU exceptions
    // Division by 0
    idt_set_gate(0, (uint64_t)(void *)isr00_handler, 0x08, 0x8E);
    // Double Fault
    idt_set_gate(0x08, (uint64_t)(void *)isr08_handler, 0x08, 0x8E);
    // General Protection Fault
    idt_set_gate(0x0D, (uint64_t)(void *)isr0D_handler, 0x08, 0x8E);
    // Page Fault
    idt_set_gate(0x0E, (uint64_t)(void *)isr0E_handler, 0x08, 0x8E);

    // Hardware IRQs
    idt_set_gate(0x20, (uint64_t)(void *)irq0_handler, 0x08, 0x8E);  // PIT
    idt_set_gate(0x21, (uint64_t)(void *)irq1_handler, 0x08, 0x8E);  // Keyboard
    // idt_set_gate(0x22, (uint64_t)(void *)irq2_handler, 0x08, 0x8E);  // Cascade
    // idt_set_gate(0x23, (uint64_t)(void *)irq3_handler, 0x08, 0x8E);  // COM2
    // idt_set_gate(0x24, (uint64_t)(void *)irq4_handler, 0x08, 0x8E);  // COM1
    // idt_set_gate(0x25, (uint64_t)(void *)irq5_handler, 0x08, 0x8E);  // LPT2
    // idt_set_gate(0x26, (uint64_t)(void *)irq6_handler, 0x08, 0x8E);  // Floppy
    // idt_set_gate(0x27, (uint64_t)(void *)irq7_handler, 0x08, 0x8E);  // LPT1
    // idt_set_gate(0x28, (uint64_t)(void *)irq8_handler, 0x08, 0x8E);  // RTC
    // idt_set_gate(0x29, (uint64_t)(void *)irq9_handler, 0x08, 0x8E);  // PCI/ACPI
    // idt_set_gate(0x2A, (uint64_t)(void *)irqA_handler, 0x08, 0x8E);  // Open
    // idt_set_gate(0x2B, (uint64_t)(void *)irqB_handler, 0x08, 0x8E);  // Open
    // idt_set_gate(0x2C, (uint64_t)(void *)irqC_handler, 0x08, 0x8E);  // Mouse
    // idt_set_gate(0x2D, (uint64_t)(void *)irqD_handler, 0x08, 0x8E);  // FPU
    // idt_set_gate(0x2E, (uint64_t)(void *)irqE_handler, 0x08, 0x8E);  // Primary ATA
    // idt_set_gate(0x2F, (uint64_t)(void *)irqF_handler, 0x08, 0x8E);  // Secondary ATA

    idt_load();

    // Unmask all IRQs
    // I am too lazy to implement all the handlers,
    // so I will only implement them as needed
    // outb(0x21, 0xFE);
}

void idt_pic_remap(void) {
    // ICW1: start initialization
    outb(0x20, 0x11); outb(0xA0, 0x11);
    // ICW2: new base vectors
    outb(0x21, 0x20); outb(0xA1, 0x28);  // <- Master: 0x20, Slave: 0x28
    // ICW3: cascade identity
    outb(0x21, 0x04); outb(0xA1, 0x02);
    // ICW4: 8086 mode
    outb(0x21, 0x01); outb(0xA1, 0x01);
    // Mask all IRQs initially
    outb(0x21, 0xFF); outb(0xA1, 0xFF);
}
