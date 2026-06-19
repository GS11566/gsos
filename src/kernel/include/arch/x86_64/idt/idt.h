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
    uint16_t offset_low;    // bits 0-15 of handler address
    uint16_t selector;      // code segment selector
    uint8_t  ist;           // bits 0-2: IST index; bits 3-7: reserved (set to 0)
    uint8_t  type_attr;     // type and attributes (e.g., 0x8E)
    uint16_t offset_mid;    // bits 16-31 of handler address
    uint32_t offset_high;   // bits 32-63 of handler address
    uint32_t zero;          // reserved, must be zero
} __attribute__((packed));  // Total: 16 bytes

struct idt_ptr {
    uint16_t limit;         // Size - 1
    uint64_t base;          // Table linear address
} __attribute__((packed));

void idt_init(void);

void idt_load(void);

void idt_pic_remap(void);
