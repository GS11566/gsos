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

struct interrupt_frame {
    uint32_t eip;   // Instruction pointer
    uint32_t cs;    // Code segment
    uint32_t eflags;// EFLAGS register
    uint32_t esp;   // Stack pointer (if privilege changed)
    uint32_t ss;    // Stack segment (if privilege changed)
};

__attribute__((interrupt))
void isr00_handler(struct interrupt_frame *frame);  // #DE: Divide Error

__attribute__((interrupt))
void isr08_handler(struct interrupt_frame *frame);  // #DF: Double Fault

__attribute__((interrupt))
void isr0D_handler(struct interrupt_frame *frame, uint32_t error_code);  // #GP

__attribute__((interrupt))
void isr0E_handler(struct interrupt_frame *frame, uint32_t error_code);  // #PF

__attribute__((interrupt))
void irq1_handler(struct interrupt_frame *frame);  // Keyboard
