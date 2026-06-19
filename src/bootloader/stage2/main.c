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
#include "stdint.h"
#include "stdio.h"
#include "fat12.h"

void _cdecl cstart_(uint16_t bootDrive)
{
    char* kernel_filename = "KERNEL  BIN";
    int res;

    puts("Stage2 booted successfully!\n\r");
    putc(bootDrive + 48);
    puts("Loading kernel...\n\r");
    res = fat12_load_file(bootDrive, kernel_filename);
    if (res != 0) {
        puts("Loading kernel failed! Code (+6): ");
        putc(48 + 6 + res);
        for (;;);
    }
    puts("Kernel loaded!\n\r");
    
    collect_memory_map();
    puts("Memory map collected!\n\r");

    puts("Jumping to kernel...\n\r");

    __asm {
        mov ax, KERNEL_LOAD_SEG
        mov ds, ax
        mov es, ax
        mov ss, ax
        mov sp, 0xFFFE      // Reset stack to top of segment
        push KERNEL_LOAD_SEG
        push KERNEL_LOAD_OFF
        retf                // Far jump to KERNEL_SEG:KERNEL_OFF
    }

    for (;;);
}
