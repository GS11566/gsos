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
#include "libs/string.h"
#include "mem/kmem.h"
#include "drivers/portio.h"

#define VGA_ADDRESS 0xB8000

// VGA Text Mode Buffer: 80x25, 16-bit entries (char + attribute)
static volatile uint16_t *vga_buffer = (volatile uint16_t *)VGA_ADDRESS;
static uint16_t vga_pos = 0;

void vga_scroll(void) {
    uint16_t blank = ' ' | (0x0F << 8);
    int i;

    memshiftd((void *)VGA_ADDRESS, MAX_X * MAX_Y * 2, MAX_X * 2, (void *)VGA_ADDRESS);
    for (i = 0; i < MAX_X; i++) {
        vga_buffer[(MAX_Y - 1) * MAX_X + i] = blank;
    }
}

void vga_putc(char c, uint8_t color) {
    if (c == '\n') {
        vga_pos = ((vga_pos / MAX_X) + 1) * MAX_X;
    } else if (c == '\r') {
        vga_pos = (vga_pos / MAX_X) * MAX_X;
    } else {
        vga_buffer[vga_pos] = (uint16_t)c | ((uint16_t)color << 8);
        vga_pos++;
    }

    if (vga_pos >= MAX_X * MAX_Y) {
        vga_scroll();
        vga_pos = (MAX_Y - 1) * MAX_X + (vga_pos % MAX_X);
    }
}

void vga_puts(const char *str, uint8_t color) {
    while (*str) {
        vga_putc(*str++, color);
    }
}

void print(const char *str) {
    vga_puts(str, 0x0F);
}

void println(const char *str) {
    vga_puts(str, 0x0F);
    vga_putc('\n', 0x0F);
}

void printc(char c) {
    char temp[2] = {'\0'};
    temp[0] = c;
    print((const char *)temp);
}

void move_cursor(int x, int y) {
    uint16_t pos = y * MAX_X + x;

    // Send the high byte
    outb(0x3D4, 0x0E);            // Select Cursor Location High Register
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));

    // Send the low byte
    outb(0x3D4, 0x0F);            // Select Cursor Location Low Register
    outb(0x3D5, (uint8_t)(pos & 0xFF));
}

void move_caret(int x, int y) {
    vga_pos = x + y * MAX_X;
}

uint8_t get_caret_x(void) {
    uint16_t safe_pos = (vga_pos >= MAX_X * MAX_Y) ? (MAX_X * MAX_Y - 1) : vga_pos;
    return safe_pos % MAX_X;
}

uint8_t get_caret_y(void) {
    uint16_t safe_pos = (vga_pos >= MAX_X * MAX_Y) ? (MAX_X * MAX_Y - 1) : vga_pos;
    return safe_pos / MAX_X;
}

void clear_screen(void) {
    int i = 0;
    uint16_t blank = (0x0F << 8) | ' ';

    for (i = 0; i < MAX_X * MAX_Y; i++) {
        vga_buffer[i] = blank;
    }
    vga_pos = 0;
}

void cursor_disable(void) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void cursor_enable(void) {
    uint8_t cursor_start = 14;
    uint8_t cursor_end = 15;

    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}
