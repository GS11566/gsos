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
OUTPUT_FORMAT(binary)
ENTRY(entry)

SECTIONS
{
    . = 0x10000;

    .entry : ALIGN(16) {
        *(.entry16)
        *(.entry32)
        *(.gdt)
    }

    .text : {
        *(.text)
        *(.text.*)
        *(CODE)
    }

    .rodata : ALIGN(16) {
        *(.rodata)
        *(.rodata.*)
    }

    .data : {
        *(.data)
        *(.data.*)
        *(DATA)
    }

    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }

    /DISCARD/ : {
        *(.note*)
        *(.comment)
        *(.debug*)
        *(.eh_frame)
        *(.eh_frame_hdr)
    }
}
