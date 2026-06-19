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
#include "drivers/keyboard/kbd.h"
#include "kbd_ps2.h"
#include "sys/cpu.h"

#define BUFFER_SIZE 64

static char buffer[BUFFER_SIZE];
static volatile int head = 0, tail = 0;

uint16_t kbd_modifiers = 0;

void kbd_enqueue_char(uint8_t c) {
    int next_tail = (tail + 1) % BUFFER_SIZE;
    if (next_tail != head) {  // Buffer not full
        buffer[tail] = (char)c;
        tail = next_tail;
    }
}

char kbd_getchar(void) {
    char c;
    while (head == tail) { CPU_HLT(); }
    c = buffer[head];
    head = (head + 1) % BUFFER_SIZE;
    return c;
}

void kbd_on_irq(void) {
    kbd_ps2_on_irq();
}

void kbd_init(void) {
    kbd_ps2_init();
}
