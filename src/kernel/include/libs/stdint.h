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

typedef unsigned char  uint8_t;
typedef signed char    int8_t;
typedef unsigned short uint16_t;
typedef signed short   int16_t;

#if defined(__x86_64__) || defined(_M_X64)
    typedef unsigned int uint32_t;
    typedef signed int   int32_t;
    typedef unsigned long uint64_t;
    typedef signed long   int64_t;
    typedef unsigned long uintptr_t;
#else
    typedef unsigned long uint32_t;
    typedef signed long   int32_t;
    typedef unsigned long long uint64_t;
    typedef signed long long   int64_t;
    typedef unsigned int uintptr_t;
#endif

typedef unsigned char uchar;
typedef signed char schar;
