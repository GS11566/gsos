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
bits 32
global enable_long_mode

enable_long_mode:
    cli

    sub esp, 12288
    mov edi, esp
    and edi, 0xFFFFF000
    xor eax, eax
    mov ecx, 3072
    rep stosd

    mov ebx, edi

    mov eax, ebx
    add eax, 4096
    or eax, 0x3
    mov [ebx], eax

    mov eax, ebx
    add eax, 8192
    or eax, 0x3
    mov [ebx + 4096], eax

    mov edi, ebx
    add edi, 8192
    xor edx, edx
    mov eax, 0x83
    mov ecx, 8
.map_loop:
    mov [edi], eax
    mov [edi + 4], edx
    add edi, 8
    add eax, 0x200000
    adc edx, 0
    loop .map_loop

    lgdt [gdt64_ptr]

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov eax, ebx
    mov cr3, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    rdmsr
    test eax, 1 << 8
    jz .long_mode_fail

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    jmp 0x08:enter_64

.long_mode_fail:
    hlt

bits 64
enter_64:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rax, 0x200000
    jmp rax

    hlt

align 8
gdt64:
    dq 0x0000000000000000       ; Null
CODE64_SEG equ $ - gdt64
    dq 0x00209A0000000000       ; L=1, D=0, P=1, S=1, Type=1010 (Exec/Read)
DATA64_SEG equ $ - gdt64
    dq 0x0000920000000000       ; P=1, S=1, Type=0010 (Read/Write)

gdt64_ptr:
    dw gdt64_end - gdt64 - 1    ; Limit
    dd gdt64                    ; Base
gdt64_end:
