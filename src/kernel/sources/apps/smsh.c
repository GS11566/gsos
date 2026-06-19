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
#include "apps/smsh.h"
#include "drivers/keyboard/kbd.h"
#include "drivers/stdio.h"
#include "libs/string.h"
#include "sys/power.h"
#include "mem/mmap.h"
#include "apps/license_text.h"

uint8_t cur_x = 0;
uint8_t cur_y = 0;

char buffer[256] = {'\0'};
uint8_t buf_head = 0;

const char *prompt = " #> ";

int str_eq(const char *str1, const char *str2) { return strcmp(str1, str2) == 0; }
int str_st(const char *str1, const char *str2) { return strncmp(str1, str2, strlen(str2)) == 0; }
int str_md(const char *str1, const char *str2) {
    char spaced_str[257];
    size_t len = strlen(str2);
    strcpy((char *)spaced_str, str2);
    spaced_str[len] = ' ';
    spaced_str[len + 1] = '\0';
    return str_eq(str1, str2) | str_st(str1, (const char *)spaced_str);
}

int buf_eq(const char *str) { return str_eq(buffer, str); }
int buf_st(const char *str) { return str_st(buffer, str); }
int buf_md(const char *str) { return str_md(buffer, str); }


void sync_cur_and_vga_pos(void) {
    cur_x = get_caret_x();
    cur_y = get_caret_y();
    move_cursor(cur_x, cur_y);
}

void mmap_entry_print_info(mmap_entry_t *entry) {
    char tmp_str[64];

    print("Base address: ");
    htos(entry->base, (char *)&tmp_str);
    println(prepend_0x((char *)&tmp_str));

    print("Length: ");
    htos(entry->length, (char *)&tmp_str);
    println(prepend_0x((char *)&tmp_str));

    print("Type: ");
    println(itos((uint32_t)entry->type, (char *)&tmp_str));
}


void smsh_show_help(void) {
    println("SMSH - SiMple SHell v0.1");
    println(" - A small, simple shell for GSOS");
    println("Commands:");
    println("- help                 - show this help");
    println("- license              - license commands:");
    println("    |- w[arranty]      - show warranty details");
    println("    |- c[onditions]    - show conditions details");
    println("    |- f[ull]          - show full license");
    println("    |- <no arg>        - show terminal license");
    println("- echo (text)          - print something to the screen");
    println("- clear                - clear the screen");
    println("- reboot               - reboot the system");
    println("- exit                 - exit the smsh");
    println("- mmap                 - memory map commands:");
    println("    |- count           - count of entries in mmap");
    println("    |- info [n]        - print info about `n`th \\");
    println("    /                      entry in mmap (start at 0)");
    println("    |- list            - list info about every mmap entry");
    println("More commands will be added later.");
}

void smsh_clear_screen(void) {
    clear_screen();
    cur_y = 0;
    cur_x = 0;
    move_caret(0, 0);
    move_cursor(0, 0);
    cursor_enable();
}

void smsh_echo(void) {
    char args[256];
    strncpy((char *)args, buffer + 5, sizeof(args) - 1);
    args[sizeof(args) - 1] = '\0';
    println((const char *)args);
}

void smsh_mmap(void) {
    uint16_t mmap_count;
    mmap_entry_t *mmap, entry;
    char args[256];
    char tmp_str[64];
    int i;
    strncpy((char *)args, buffer + 5, sizeof(args) - 1);
    args[sizeof(args) - 1] = '\0';

    if (str_md((char *)args, "count")) {
        mmap_count = mmap_get_count();
        print("Mmap entries count: ");
        println(itos(mmap_count, (char *)&tmp_str));
    }
    else if (str_md((char *)args, "info")) {
        strncpy((char *)args, (const char *)args + 5, sizeof(args) - 1);
        mmap = mmap_get_entries();
        if (strlen((const char *)args) == 0) {
            mmap_entry_print_info(mmap);
        } else {
            mmap_entry_print_info(&mmap[stoi((const char *)args)]);
        }
    }
    else if (str_md((char *)args, "list")) {
        mmap = mmap_get_entries();
        mmap_count = mmap_get_count();

        for (i = 0; i < mmap_count; i++) {
            print("Index: ");
            println(itos(i, (char *)&tmp_str));
            entry = mmap[i];

            mmap_entry_print_info((mmap_entry_t *)&entry);
            if (i < mmap_count - 1) { println("========="); kbd_getchar(); }
        }
    } else {
        println("Usage: mmap <count/info/list> [index]");
    }
}

void smsh_license(void) {
    char args[256];
    strncpy((char *)args, buffer + 8, sizeof(args) - 1);
    args[sizeof(args) - 1] = '\0';

    if (strlen(args) == 0) {
        println(gpl_terminal_text);
    }
    else if (str_md((char *)args, "w") || str_md((char *)args, "warranty")) {
        println(gpl_warranty_text);
    }
    else if (str_md((char *)args, "c") || str_md((char *)args, "conditions")) {
        println(gpl_conditions_text);
    }
    else if (str_md((char *)args, "f") || str_md((char *)args, "full")) {
        println(gpl_full_text);
    }
    else {
        println("Usage: license <full/c[onditions]/w[arranty]/*no args*>");
    }
}


int handle_command(void) {
    if (buf_eq("help")) smsh_show_help();
    else if (buf_md("clear")) smsh_clear_screen();
    else if (buf_md("reboot")) restart();
    else if (buf_md("echo")) smsh_echo();
    else if (buf_md("mmap")) smsh_mmap();
    else if (buf_md("license")) smsh_license();
    else if (buf_md("exit")) {
        println("Exiting...");
        cursor_disable();
        return 100;
    }
    else if (strlen(buffer) == 0);
    else {
        println("Unknown command!");
    }
    return 0;
}

int handle_key(uint8_t code) {
    int err = 0;

    if (code >= 0x80) {
        // Special keys
    } else if (code == '\b') {
        if (buf_head != 0) {
            buffer[buf_head] = 0;
            buf_head--;
            cur_x--;
            move_caret(cur_x, cur_y);
            printc(' ');
            move_caret(cur_x, cur_y);
            move_cursor(cur_x, cur_y);
        }
    } else if (code == '\n') {
        printc('\n');
        err = handle_command();
        cur_y = get_caret_y();
        memset(buffer, 0, 256);
        buf_head = 0;
        print(prompt);
        cur_x = (uint8_t)strlen(prompt);
        move_cursor(cur_x, cur_y);
    } else {
        printc(code);
        buffer[buf_head++] = code;
        move_cursor(++cur_x, cur_y);
    }

    return err;
}

int smsh_main(void) {
    char c;
    int err = 0;

    smsh_clear_screen();
    println(gpl_terminal_text);
    println("Welcome to SMSH!");
    smsh_show_help();
    print(prompt);
    sync_cur_and_vga_pos();

    for (;;) {
        c = kbd_getchar();
        err = handle_key(c);
        if (err != 0) return err;
    }
    return 0;
}
