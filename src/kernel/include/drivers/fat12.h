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

#define BYTES_PER_SECTOR    512
#define SECTORS_PER_FAT     9
#define RESERVED_SECTORS    1
#define FAT_COUNT           2
#define ROOT_DIR_ENTRIES    224
#define SECTORS_PER_CLUSTER 1

/* Load a file from FAT12 filesystem.
 * drive: 0 = Primary Master, 1 = Primary Slave (ATA device select)
 * filename_11: 8.3 name padded with spaces, uppercase, 11 bytes (e.g., "KERNEL  BIN")
 * dest: Linear destination pointer (identity-mapped or properly paged)
 * max_dest_size: Buffer size limit to prevent overflow
 */
int fat12_load_file(uint8_t drive, const char *filename_11, uint8_t *dest, uint32_t max_dest_size);
