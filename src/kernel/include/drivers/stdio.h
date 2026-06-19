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
#pragma once
#include "libs/stdint.h"

#define MAX_X 80
#define MAX_Y 25

void vga_scroll(void);

void vga_putc(char c, uint8_t color);
void vga_puts(const char *str, uint8_t color);

void print(const char *str);
void println(const char *str);
void printc(char c);

void move_cursor(int x, int y);
void move_caret(int x, int y);

uint8_t get_caret_x(void);
uint8_t get_caret_y(void);

void clear_screen(void);

void cursor_disable(void);
void cursor_enable(void);
