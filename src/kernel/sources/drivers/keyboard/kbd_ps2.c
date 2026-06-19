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
#include "kbd_ps2.h"
#include "drivers/keyboard/kbd.h"
#include "drivers/portio.h"

// Helper macros for modifier checks
#define IS_SHIFT_PRESSED()  (kbd_modifiers & (MOD_LSHIFT | MOD_RSHIFT))
#define IS_CTRL_PRESSED()   (kbd_modifiers & (MOD_LCTRL  | MOD_RCTRL))
#define IS_ALT_PRESSED()    (kbd_modifiers & (MOD_LALT   | MOD_RALT))

// Scancode set 1 to ASCII/base code mapping
static const uint8_t scancode_to_ascii[128] = {
    /* 0x00-0x0F */ 0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    /* 0x10-0x1F */ 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's',
    /* 0x20-0x2F */ 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    /* 0x30-0x3F */ 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5,
    /* 0x40-0x4F */ KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, 0, 0, KEY_HOME, KEY_UP, KEY_PGUP, 0, KEY_LEFT, 0, KEY_RIGHT, 0, KEY_END,
    /* 0x50-0x5F */ KEY_DOWN, KEY_PGDN, KEY_INSERT, KEY_DELETE, 0, 0, 0, KEY_F11, KEY_F12, 0, 0, 0, 0, 0, 0, 0,
    /* 0x60-0x7F */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x80-0x9F */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static uint8_t kbd_state = 0;
#define KBD_NORMAL   0
#define KBD_EXT      1  // Saw 0xE0
#define KBD_PAUSE_1  2  // Saw 0xE1
#define KBD_PAUSE_2  3  // Saw E1 1D
#define KBD_PAUSE_3  4  // Saw E1 1D 45
#define KBD_PAUSE_4  5  // Saw E1 1D 45 E1
#define KBD_PAUSE_5  6  // Saw E1 1D 45 E1 9D

static uint8_t kbd_caps_lock = 0;
static uint8_t kbd_num_lock = 1;

void kbd_ps2_on_irq(void) {
    uint8_t sc = inb(PS2_KBD_DATA_PORT);
    uint8_t is_break = (sc & 0x80);
    uint8_t code = sc & 0x7F;
    char c = 0;
    uint8_t emit = 0;

    // State machine for extended sequences
    switch (kbd_state) {
    case KBD_NORMAL:
        if (code == 0xE0) { kbd_state = KBD_EXT; return; }
        if (code == 0xE1) { kbd_state = KBD_PAUSE_1; return; }
        break;
    case KBD_EXT:
        kbd_state = KBD_NORMAL;
        goto process_key;
    case KBD_PAUSE_1: kbd_state = (code == 0x1D) ? KBD_PAUSE_2 : KBD_NORMAL; return;
    case KBD_PAUSE_2: kbd_state = (code == 0x45) ? KBD_PAUSE_3 : KBD_NORMAL; return;
    case KBD_PAUSE_3: kbd_state = (code == 0xE1) ? KBD_PAUSE_4 : KBD_NORMAL; return;
    case KBD_PAUSE_4: kbd_state = (code == 0x9D) ? KBD_PAUSE_5 : KBD_NORMAL; return;
    case KBD_PAUSE_5:
        kbd_state = KBD_NORMAL;
        if (!is_break) { kbd_enqueue_char(KEY_PAUSE); }
        return;
    }

process_key:
    // Handle modifier key press/release
    if (!is_break) {
        if (code == 0x2A) kbd_modifiers |= MOD_LSHIFT;
        else if (code == 0x36) kbd_modifiers |= MOD_RSHIFT;
        else if (code == 0x1D && kbd_state != KBD_EXT) kbd_modifiers |= MOD_LCTRL;
        else if (code == 0x38 && kbd_state != KBD_EXT) kbd_modifiers |= MOD_LALT;
        else if (code == 0x1D && kbd_state == KBD_EXT) kbd_modifiers |= MOD_RCTRL;
        else if (code == 0x38 && kbd_state == KBD_EXT) kbd_modifiers |= MOD_RALT;
        else if (code == 0x5B && kbd_state == KBD_EXT) kbd_modifiers |= MOD_LWIN;
        else if (code == 0x5C && kbd_state == KBD_EXT) kbd_modifiers |= MOD_RWIN;
        else if (code == 0x3A) kbd_caps_lock = !kbd_caps_lock;
        else if (code == 0x45) kbd_num_lock = !kbd_num_lock;
        else if (code == 0x0F) kbd_modifiers |= MOD_TAB;
        else if (code == 0x1C) {
            if (kbd_state == KBD_EXT) kbd_modifiers |= MOD_KPENTER;
            else kbd_modifiers |= MOD_ENTER;
        }
    } else {
        // Key release - update modifiers
        if (code == 0x2A) kbd_modifiers &= ~MOD_LSHIFT;
        else if (code == 0x36) kbd_modifiers &= ~MOD_RSHIFT;
        else if (code == 0x1D && kbd_state != KBD_EXT) kbd_modifiers &= ~MOD_LCTRL;
        else if (code == 0x38 && kbd_state != KBD_EXT) kbd_modifiers &= ~MOD_LALT;
        else if (code == 0x1D && kbd_state == KBD_EXT) kbd_modifiers &= ~MOD_RCTRL;
        else if (code == 0x38 && kbd_state == KBD_EXT) kbd_modifiers &= ~MOD_RALT;
        else if (code == 0x5B && kbd_state == KBD_EXT) kbd_modifiers &= ~MOD_LWIN;
        else if (code == 0x5C && kbd_state == KBD_EXT) kbd_modifiers &= ~MOD_RWIN;
        else if (code == 0x0F) kbd_modifiers &= ~MOD_TAB;
        else if (code == 0x1C) kbd_modifiers &= ~(MOD_ENTER | MOD_KPENTER);
        return; // Don't emit on break
    }

    if (code >= 128) return;
    c = (char)scancode_to_ascii[code];
    if (!c) return;

    // Extended key handling
    if (kbd_state == KBD_EXT) {
        if (code == 0x37) { c = KEY_PRINTSCR; emit = 1; }
        else if (code == 0x1C) { c = KEY_KP_ENTER; emit = 1; }
        else if (code == 0x35) { c = KEY_KP_DIV; emit = 1; }
        else if (kbd_num_lock && code >= 0x47 && code <= 0x53) {
            static const uint8_t kp[] = {KEY_KP_7,KEY_KP_8,KEY_KP_9,0,KEY_KP_4,KEY_KP_5,KEY_KP_6,0,KEY_KP_1,KEY_KP_2,KEY_KP_3,KEY_KP_0,KEY_KP_DOT};
            c = (char)kp[code - 0x47];
            if (c) emit = 1;
        }
    }

    if (!emit && c != 0) {
        // Letters: apply shift/CapsLock
        if (c >= 'a' && c <= 'z') {
            uint8_t upper = (kbd_modifiers & (MOD_LSHIFT|MOD_RSHIFT)) ^ kbd_caps_lock;
            if (upper) c = c - 'a' + 'A';
            emit = 1;
        }
        // Special keys (0x80+)
        else if (c >= 0x80) {
            emit = 1;
        }
        // Apply shift for symbol row
        else {
            if (kbd_modifiers & (MOD_LSHIFT|MOD_RSHIFT)) {
                switch (c) {
                    case '1': c = '!'; break; case '2': c = '@'; break;
                    case '3': c = '#'; break; case '4': c = '$'; break;
                    case '5': c = '%'; break; case '6': c = '^'; break;
                    case '7': c = '&'; break; case '8': c = '*'; break;
                    case '9': c = '('; break; case '0': c = ')'; break;
                    case '-': c = '_'; break; case '=': c = '+'; break;
                    case '[': c = '{'; break; case ']': c = '}'; break;
                    case ';': c = ':'; break; case '\'': c = '"'; break;
                    case '`': c = '~'; break; case '\\': c = '|'; break;
                    case ',': c = '<'; break; case '.': c = '>'; break;
                    case '/': c = '?'; break;
                    default: break;
                }
            }
            emit = 1;
        }
    }

    if (emit) {
        // Ctrl combos: a-z -> 0x01-0x1A
        if ((kbd_modifiers & (MOD_LCTRL|MOD_RCTRL)) && c >= 'a' && c <= 'z') {
            c = c - 'a' + 1;
        }
        kbd_enqueue_char((uint8_t)c);
    }
}

void kbd_ps2_init(void) {
    // Unmask IRQ1 on PIC (0x21)
    outb(0x21, inb(0x21) & 0xFD);
}
