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
#include "paging/paging.h"

void paging_setup_paging(void) {
    int i;

    for(i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 4096) | PAGE_PRESENT | PAGE_WRITE;
    }

    page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_WRITE;
    
    for(i = 1; i < 1024; i++) {
        page_directory[i] = 0x00000000; 
    }

    paging_enable_paging((uint32_t)page_directory);
}

void paging_enable_paging(uint32_t pd_addr) {
    __asm__ __volatile__ (
        "movl %0, %%eax\n\t"
        "movl %%eax, %%cr3\n\t"
        "movl %%cr0, %%eax\n\t"
        "orl $0x80000000, %%eax\n\t"
        "movl %%eax, %%cr0\n\t"
        "jmp 1f\n\t"
        "1:"
        :
        : "r" (pd_addr)
        : "eax", "memory"
    );
}
