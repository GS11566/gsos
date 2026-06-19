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
#include "idt/interrupt_handlers.h"
#include "drivers/stdio.h"
#include "drivers/portio.h"
#include "libs/stdint.h"
#include "drivers/keyboard/kbd.h"

// Division by zero - NO error code
__attribute__((interrupt))
void isr00_handler(struct interrupt_frame *frame) {
    (void)frame;  // Suppress unused warning
    println("Division by zero!");
}

// Double Fault - NO error code
__attribute__((interrupt))
void isr08_handler(struct interrupt_frame *frame) {
    (void)frame;
    println("Double Fault");
}

// General Protection Fault - HAS error code (MANDATORY parameter)
__attribute__((interrupt))
void isr0D_handler(struct interrupt_frame *frame, uint32_t error_code) {
    (void)frame;
    println("General Protection Fault! Error: 0x");
    // Optional: print_hex(error_code);
    while (1) {}
}

// Page Fault - HAS error code + CR2 contains faulting address
__attribute__((interrupt))
void isr0E_handler(struct interrupt_frame *frame, uint32_t error_code) {
    (void)frame;
    uint32_t cr2;
    __asm__ __volatile__("mov %%cr2, %0" : "=r" (cr2));
    println("Page Fault at 0x");
    // Optional: print_hex(cr2);
    println(", error: 0x");
    // Optional: print_hex(error_code);
}

// IRQ1 (Keyboard) - NO error code
__attribute__((interrupt))
void irq1_handler(struct interrupt_frame *frame) {
    (void)frame;
    kbd_on_irq();
    outb(0x20, 0x20);  // Send EOI to master PIC
}
