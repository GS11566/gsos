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

#define INT64_MIN (-0x7FFFFFFFFFFFFFFFLL - 1)

uint64_t udiv64(uint64_t dividend, uint64_t divisor, uint64_t *remainder);
int64_t sdiv64(int64_t dividend, int64_t divisor, int64_t *remainder);
uint64_t umul64(uint64_t a, uint64_t b);
int64_t smul64(int64_t a, int64_t b);

typedef struct {
    uint32_t lo;
    uint32_t hi;
} uint64_split;

typedef union {
    uint64_t full;
    uint64_split parts;
} uint64_conv;

uint64_t lshift64(uint64_t val, uint32_t n);
uint64_t rshift64(uint64_t val, uint32_t n);
