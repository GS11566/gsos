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

struct idt_entry {
    uint16_t offset_lower; // Lower 16 bit of the handler offset
    uint16_t selector;     // Code segment selector
    uint8_t  zero;         // Always 0
    uint8_t  type_attr;    // Entry type and access
    uint16_t offset_upper; // Upper 16 bit of the handler offset
};

struct idt_ptr {
    uint16_t limit;        // Size - 1
    uint32_t base;         // Table linear address
};

void idt_init(void);

void idt_load(const struct idt_ptr *idtp);

void idt_pic_remap(void);
