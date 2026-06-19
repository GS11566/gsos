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
#include "sys/power.h"
#include "libs/stdint.h"
#include "drivers/portio.h"

void restart(void) {
    uint8_t good = 0x02;
    while (good & 0x02) {
        good = inb(0x64);
    }

    outb(0x64, 0xFE);

    // If didn't work, just throw a triple fault
    {
        struct {
            uint16_t limit;
            uint32_t base;
        } __attribute__((packed)) empty_idt = {0, 0};

        // Why is GCC inline assembly so much harder than open-watcom-v2's
        __asm__ __volatile__ (
            "lidt %0\n\t"    // Load Interrupt Descriptor Table
            "int $3"         // Trigger breakpoint interrupt
            :                // No output operands
            : "m"(empty_idt) // Input: memory reference to the IDT structure
            : "memory"       // Notify compiler memory is affected
        );
    }
}
