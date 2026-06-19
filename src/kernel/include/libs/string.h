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
#include "libs/stddef.h"
#include "libs/stdint.h"
#include "libs/math.h"

static inline void *memset(void *s, uchar c, size_t n) {
    uchar *p;
    if (s == NULL) return NULL;
    p = (uchar *)s;
    while (n--) *p++ = (uchar)c;
    return s;
}

static inline void *memcpy(void *dest, const void *src, size_t n) {
    uchar *d;
    const uchar *s;
    if (dest == NULL || src == NULL) return NULL;
    d = (uchar *)dest;
    s = (const uchar *)src;
    while (n--) *d++ = *s++;
    return dest;
}

static inline void *memmove(void *dest, const void *src, size_t n) {
    uchar *d;
    const uchar *s;
    if (dest == NULL || src == NULL) return NULL;
    d = (uchar *)dest;
    s = (const uchar *)src;
    if (d <= s || d >= s + n) {
        while (n--) *d++ = *s++;
    } else {
        d += n; s += n;
        while (n--) *--d = *--s;
    }
    return dest;
}

static inline int memcmp(const void *s1, const void *s2, size_t n) {
    const uchar *p1, *p2;
    if (s1 == NULL || s2 == NULL) return 0;
    p1 = (const uchar *)s1;
    p2 = (const uchar *)s2;
    while (n--) {
        if (*p1 != *p2) return *p1 - *p2;
        p1++; p2++;
    }
    return 0;
}

// String operations (assume null-terminated)
static inline size_t strlen(const char *s) {
    size_t len;
    if (s == NULL) return 0;
    len = 0;
    while (*s++) len++;
    return len;
}

static inline char *strcpy(char *dest, const char *src) {
    char *d;
    if (dest == NULL || src == NULL) return NULL;
    d = dest;
    while ((*d++ = *src++));
    return dest;
}

static inline char *strncpy(char *dest, const char *src, size_t n) {
    char *d;
    if (dest == NULL || src == NULL) return NULL;
    d = dest;
    while (n && (*d++ = *src++)) n--;
    while (n--) *d++ = '\0';
    return dest;
}

static inline int strcmp(const char *s1, const char *s2) {
    if (s1 == NULL || s2 == NULL) return 0;
    while (*s1 && *s1 == *s2) { s1++; s2++; }
    return *(uchar *)s1 - *(uchar *)s2;
}

static inline int strncmp(const char *s1, const char *s2, size_t n) {
    if (s1 == NULL || s2 == NULL) return 0;
    while (n && *s1 && *s1 == *s2) { s1++; s2++; n--; }
    if (n == 0) return 0;
    return *(uchar *)s1 - *(uchar *)s2;
}

char *itos(uint64_t val, char* buf);
char *htos(uint64_t val, char* buf);
char* prepend_0x(char* buf);
uint64_t stoi(const char *str);
