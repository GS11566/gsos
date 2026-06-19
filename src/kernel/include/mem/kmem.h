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
#include "libs/stddef.h"

static inline void memshiftd(void* addr, uint32_t size, uint32_t amount, void* lb) {
    uint8_t *s = (uint8_t*)addr + amount, *d = (uint8_t*)addr, *end = d + size;
    while (d < end) {
        if (d >= (uint8_t*)lb) *d = (s < end + amount) ? *s : 0; 
        d++; s++;
    }
}

static inline void memshiftu(void* addr, uint32_t size, uint32_t amount, void* ub) {
    uint8_t *s = (uint8_t*)addr + size, *d = s + amount, *start = (uint8_t*)addr;
    while (s > start) {
        if (--d <= (uint8_t*)ub) *d = *--s;
        else --s;
    }
}

