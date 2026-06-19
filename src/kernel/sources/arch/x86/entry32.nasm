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
section .entry16
bits 16

extern kmain
global entry

%macro DBG_CHR 2
    pusha
    mov ax, 0xB800
    mov es, ax
    mov byte [es:%1], %2
    mov byte [es:%1+1], 0x0E  ; Yellow on black
    popa
%endmacro

entry:
    DBG_CHR 0, '1'
    cli                     ; disable interrupts
    xor ax, ax
    mov ds, ax              ; set DS=0
    mov es, ax
    mov ss, ax
    mov sp, 0x9000

    ; enable A20
    in al, 0x92
    or al, 0x02
    out 0x92, al

    DBG_CHR 2, '2'

    xor ax, ax
    mov ds, ax

    ; load GDT
    push ds
    mov ax, 0x1000
    mov ds, ax
    a32 lgdt [gdt_desc - 0x10000]
    pop ds

    ; enter Protected Mode
    mov eax, cr0
    or eax, 1               ; set PE bit
    mov cr0, eax

    ; far jump to flush CPU instruction queue and load 32-bit CS
    jmp dword CODE_SEG:pm_start

section .entry32
bits 32
pm_start:

    ; set up 32-bit segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov ebp, 0x50000
    mov esp, ebp
    
    mov edi, 0xB8000

    ; call C kernel entry point
    call kmain

    ; if kmain ever returns, halt the CPU
    cli
    hlt
    jmp $

section .gdt
align 16
gdt_start:
    dq 0                          ; Null descriptor
gdt_code:
    dw 0xFFFF                     ; Limit (low)
    dw 0x0000                     ; Base (low)
    db 0x00                       ; Base (mid)
    db 10011010b                  ; Access: Present, DPL=0, Code, Executable, Readable
    db 11001111b                  ; Flags/Limit high: G=1, D=1 (32-bit), Limit=0xF
    db 0x00                       ; Base (high)
gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b                  ; Access: Present, DPL=0, Data, Writable
    db 11001111b
    db 0x00
gdt_end:

gdt_desc:
    dw gdt_end - gdt_start - 1    ; Limit (size - 1)
    dd gdt_start                  ; Base address

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
