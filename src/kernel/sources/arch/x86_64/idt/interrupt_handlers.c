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

// Division by zero
__attribute__((interrupt)) void isr00_handler(struct interrupt_frame *frame) {
    println("Division by zero!");
}

// Double Fault
__attribute__((interrupt)) void isr08_handler(struct interrupt_frame *frame, uint64_t error_code) {
    println("Double Fault");
}

// General Protection Fault
__attribute__((interrupt)) void isr0D_handler(struct interrupt_frame *frame, uint64_t error_code) {
    println("General Protection Fault!");
    while (1) {}
}

// Page Fault
__attribute__((interrupt)) void isr0E_handler(struct interrupt_frame *frame, uint64_t error_code) {
    println("Page Fault");
}

// Programmable Interval Timer
__attribute__((interrupt)) void irq0_handler(struct interrupt_frame *frame) {
    outb(0x20, 0x20);
}

// PS/2 keyboard interrupt request
__attribute__((interrupt)) void irq1_handler(struct interrupt_frame *frame) {
    kbd_on_irq();
    // Send EOI to PIC
    outb(0x20, 0x20);
}
