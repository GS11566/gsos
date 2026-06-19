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
#include "drivers/stdio.h"
#include "drivers/keyboard/kbd.h"
#include "idt/idt.h"
#include "long_mode_switch.h"
#include "drivers/fat12.h"
#include "libs/string.h"

__attribute__((used, noinline))
void kmain(void) {
    int res;
    uint8_t *dest;

    vga_puts("Kernel booted!\n", 0x0F);
    vga_puts("Welcome!\n", 0x0A);
    idt_init();
    println("IDT initialized!");
    kbd_init();
    println("Keyboard initialized!");

    println("Trying to load kernel64.bin from disk...");
    dest = (uint8_t *)0x200000;
    res = fat12_load_file(0, "KERNEL64BIN", dest, 0x800000);
    if (res != 0) {
        print("Loading kernel failed! Code (+10_000): ");
        println(itos(res + 10000, (char *)dest));
        for (;;);
    }
    println("Kernel64 loaded!");

    println("Trying to switch to long mode...");
    enable_long_mode();
    println("Failed to switch to long mode!");

    for (;;); // Halt loop
}
