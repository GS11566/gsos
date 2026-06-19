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
#include "libs/math.h"
#include "sys/cpu.h"

uint64_t udiv64(uint64_t dividend, uint64_t divisor, uint64_t *remainder) {
    union { uint64_t v; uint32_t w[2]; } u_div = {0}, u_divisor = {0};
    uint32_t d_hi, d_lo, r_hi, r_lo, q_hi, q_lo, div_hi, div_lo;
    uint32_t bit, borrow;
    union { uint64_t v; uint32_t w[2]; } u_rem, u_quot;
    int i;

    if (divisor == 0) { CPU_DBZ(); }

    u_div.v = dividend;
    d_lo = u_div.w[0];
    d_hi = u_div.w[1];

    u_divisor.v = divisor;
    div_lo = u_divisor.w[0];
    div_hi = u_divisor.w[1];

    r_hi = 0; r_lo = 0;
    q_hi = 0; q_lo = 0;

    for (i = 63; i >= 0; i--) {
        r_hi = (r_hi << 1) | (r_lo >> 31);
        r_lo = (r_lo << 1);

        bit = (i >= 32) ? ((d_hi >> (i - 32)) & 1) : ((d_lo >> i) & 1);
        r_lo |= bit;

        if (r_hi > div_hi || (r_hi == div_hi && r_lo >= div_lo)) {
            borrow = (r_lo < div_lo) ? 1 : 0;
            r_lo -= div_lo;
            r_hi -= div_hi + borrow;

            if (i >= 32) {
                q_hi |= (1U << (i - 32));
            } else {
                q_lo |= (1U << i);
            }
        }
    }

    if (remainder) {
        u_rem.w[0] = r_lo;
        u_rem.w[1] = r_hi;
        *remainder = u_rem.v;
    }

    u_quot.w[0] = q_lo;
    u_quot.w[1] = q_hi;
    return u_quot.v;
}

int64_t sdiv64(int64_t dividend, int64_t divisor, int64_t *remainder) {
    uint64_t udividend, udivisor;
    uint64_t urem, uquot;
    int64_t quot, rem;
    int sign = 1;

    if (divisor == 0) { CPU_DBZ(); }
    if (dividend == INT64_MIN && divisor == -1) { CPU_DBZ(); }

    if (dividend < 0) {
        udividend = (uint64_t)(-dividend);
        sign = -sign;
    } else {
        udividend = (uint64_t)dividend;
    }

    if (divisor < 0) {
        udivisor = (uint64_t)(-divisor);
        sign = -sign;
    } else {
        udivisor = (uint64_t)divisor;
    }

    uquot = udiv64(udividend, udivisor, (uint64_t *)&urem);

    quot = (int64_t)uquot;
    rem  = (int64_t)urem;

    if (sign < 0) quot = -quot;
    if (dividend < 0) rem = -rem;

    if (remainder) *remainder = rem;
    return quot;
}

uint64_t umul64(uint64_t a, uint64_t b) {
    uint64_t res;
    int i;

    res = 0;
    for (i = 0; i < 64; i++) {
        if (b & 1) {
            res += a;
        }
        a = lshift64(a, 1);
        b = rshift64(b, 1);
    }
    return res;
}

int64_t smul64(int64_t a, int64_t b) {
    uint64_t ua, ub, ures;
    int64_t res;
    int sign;

    sign = 1;
    if (a < 0) { ua = (uint64_t)(-a); sign = -sign; } else { ua = (uint64_t)a; }
    if (b < 0) { ub = (uint64_t)(-b); sign = -sign; } else { ub = (uint64_t)b; }

    ures = umul64(ua, ub);
    res = (int64_t)ures;

    if (sign < 0) res = -res;
    return res;
}

uint64_t lshift64(uint64_t val, uint32_t n) {
    uint64_conv u = {0};
    uint32_t low;
    uint32_t high;
    
    u.full = val;
    low = u.parts.lo;
    high = u.parts.hi;
    
    if (n >= 64) {
        low = 0;
        high = 0;
    } else if (n >= 32) {
        high = low << (n - 32);
        low = 0;
    } else if (n > 0) {
        high = (high << n) | (low >> (32 - n));
        low = low << n;
    }
    
    u.parts.lo = low;
    u.parts.hi = high;
    return u.full;
}

uint64_t rshift64(uint64_t val, uint32_t n) {
    uint64_conv u = {0};
    uint32_t low;
    uint32_t high;

    u.full = val;
    low = u.parts.lo;
    high = u.parts.hi;
    
    if (n >= 64) {
        low = 0;
        high = 0;
    } else if (n >= 32) {
        low = high >> (n - 32);
        high = 0;
    } else if (n > 0) {
        low = (low >> n) | (high << (32 - n));
        high = high >> n;
    }
    
    u.parts.lo = low;
    u.parts.hi = high;
    return u.full;
}
