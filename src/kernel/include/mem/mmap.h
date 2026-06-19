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

#define MMAP_BUFFER 0x6000
#define MMAP_COUNT  0x5FFE

#pragma pack(push, 1)
typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
} mmap_entry_t;
#pragma pack(pop)

static inline uint16_t mmap_get_count(void) {
    return *(volatile uint16_t*)MMAP_COUNT;
}

static inline mmap_entry_t* mmap_get_entries(void) {
    return (mmap_entry_t*)MMAP_BUFFER;
}
