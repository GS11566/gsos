;    GSOS - a simple OS written from scratch.
;    Copyright (C) 2026  Vadim Gladushev
;
;    This program is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation, either version 3 of the License, or
;    (at your option) any later version.
;
;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with this program.  If not, see <https://www.gnu.org/licenses/>.
bits 16

global collect_memory_map_

collect_memory_map_:
    push es
    push di
    push bx
    push cx
    push dx
    push bp

    mov ax, 0x0000
    mov es, ax              ; ES:0 -> segment 0
    mov di, MMAP_BUFFER     ; Buffer destination
    xor ebx, ebx            ; Continuation cookie
    xor bp, bp              ; Entry counter

.e820_loop:
    mov eax, 0xE820
    mov edx, E820_SIG       ; Required signature
    mov ecx, 20             ; Request 20 bytes (base+len+type)
    int 0x15

    jc .e820_done           ; CF=1 -> error or end
    cmp eax, E820_SIG
    jne .e820_done          ; Invalid signature

    test ecx, ecx
    jz .e820_done           ; BIOS wrote 0 bytes

    inc bp                  ; Valid entry
    add di, cx              ; Advance by actual size returned (20 or 24)
    test ebx, ebx
    jnz .e820_loop          ; EBX != 0 -> more entries

.e820_done:
    mov [es:MMAP_COUNT], bp ; Store total entries
    pop bp
    pop dx
    pop cx
    pop bx
    pop di
    pop es
    ret


; Constants
MMAP_BUFFER equ 0x6000      ; 24KB mark
MMAP_COUNT  equ 0x5FFE      ; 16-bit word
E820_SIG    equ 0x534D4150  ; "SMAP"
