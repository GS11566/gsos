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
#include "string.h"

char *itos(uint64_t val, char* buf) {
    char* p = buf;
    char temp[23];
    int i = 0;
    uint64_t rem, div;

    if (val == 0) {
        *p++ = '0';
        *p = '\0';
        return buf;
    }
    
    while (val > 0) {
        div = udiv64(val, 10, (uint64_t *)&rem);
        temp[i++] = rem + '0';
        val = div;
    }
    
    while (i > 0) {
        *p++ = temp[--i];
    }
    
    *p = '\0';
    return buf;
}

char *htos(uint64_t val, char* buf) {
    char* p = buf;
    char temp[19];
    int i = 0;
    uint64_t rem, div;
    static const char hex_chars[] = "0123456789ABCDEF";

    if (val == 0) {
        *p++ = '0';
        *p = '\0';
        return buf;
    }
    
    while (val > 0) {
        div = udiv64(val, 16, (uint64_t *)&rem);
        temp[i++] = hex_chars[rem];
        val = div;
    }

    while (i > 0) {
        *p++ = temp[--i];
    }
    
    *p = '\0';
    return buf;
}

char* prepend_0x(char* buf) {
    int len = strlen(buf);
    int i;

    for (i = len; i >= 0; i--) {
        buf[i + 2] = buf[i];
    }

    buf[0] = '0';
    buf[1] = 'x';

    return buf;
}

uint64_t stoi(const char *str) {
    uint64_t result = 0;

    while (*str == ' ' || *str == '\t' || *str == '\n') {
        str++;
    }

    while (*str >= '0' && *str <= '9') {
        result = (umul64(result, 10)) + (*str - '0');
        str++;
    }

    return result;
}
