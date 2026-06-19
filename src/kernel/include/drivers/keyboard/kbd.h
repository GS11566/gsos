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

// Bitmask for modifier keys
extern uint16_t kbd_modifiers;

#define MOD_LSHIFT  0x01
#define MOD_RSHIFT  0x02
#define MOD_LCTRL   0x04
#define MOD_RCTRL   0x08
#define MOD_LALT    0x10
#define MOD_RALT    0x20
#define MOD_LWIN    0x40
#define MOD_RWIN    0x80
#define MOD_CAPS    0x100
#define MOD_TAB     0x200
#define MOD_ENTER   0x400
#define MOD_KPENTER 0x800
#define MOD_FN      0x1000
#define MOD_KPLOCK  0x2000

// Special key codes
#define KEY_F1          0x80
#define KEY_F2          0x81
#define KEY_F3          0x82
#define KEY_F4          0x83
#define KEY_F5          0x84
#define KEY_F6          0x85
#define KEY_F7          0x86
#define KEY_F8          0x87
#define KEY_F9          0x88
#define KEY_F10         0x89
#define KEY_F11         0x8A
#define KEY_F12         0x8B

#define KEY_UP          0x90
#define KEY_DOWN        0x91
#define KEY_LEFT        0x92
#define KEY_RIGHT       0x93

#define KEY_INSERT      0xA0
#define KEY_DELETE      0xA1
#define KEY_HOME        0xA2
#define KEY_END         0xA3
#define KEY_PGUP        0xA4
#define KEY_PGDN        0xA5

#define KEY_PRINTSCR    0xB0
#define KEY_PAUSE       0xB1

#define KEY_KP_0        0xC0
#define KEY_KP_1        0xC1
#define KEY_KP_2        0xC2
#define KEY_KP_3        0xC3
#define KEY_KP_4        0xC4
#define KEY_KP_5        0xC5
#define KEY_KP_6        0xC6
#define KEY_KP_7        0xC7
#define KEY_KP_8        0xC8
#define KEY_KP_9        0xC9
#define KEY_KP_DOT      0xCA
#define KEY_KP_ENTER    0xCB
#define KEY_KP_DIV      0xCC
#define KEY_KP_MUL      0xCD
#define KEY_KP_MINUS    0xCE
#define KEY_KP_PLUS     0xCF

// Keyboard API
char kbd_getchar(void);
void kbd_init(void);

void kbd_on_irq(void);

void kbd_enqueue_char(uint8_t c);
