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

#if defined(__WATCOMC__)
  #define CPU_HLT() __asm { hlt }
#elif defined(__GNUC__)
  #define CPU_HLT() __asm__ __volatile__("hlt")
#endif

#if defined(__WATCOMC__)
  #define CPU_DBZ() __asm { int 0 }
#elif defined(__GNUC__)
  #define CPU_DBZ() __asm__ __volatile__("int $0")
#endif
