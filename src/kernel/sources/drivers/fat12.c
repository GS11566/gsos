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
#include "fat12.h"
#include "drivers/portio.h"
#include "libs/string.h"

#pragma pack(push, 1)
typedef struct {
    uint8_t  name[11];
    uint8_t  attr;
    uint8_t  reserved[10];
    uint16_t time;
    uint16_t date;
    uint16_t first_cluster;
    uint32_t size;
} FatDirEntry;
#pragma pack(pop)

#define ROOT_DIR_LBA   (RESERVED_SECTORS + (FAT_COUNT * SECTORS_PER_FAT))
#define ROOT_DIR_SIZE  ((ROOT_DIR_ENTRIES * 32 + BYTES_PER_SECTOR - 1) / BYTES_PER_SECTOR)
#define DATA_AREA_LBA  (ROOT_DIR_LBA + ROOT_DIR_SIZE)

static uint8_t fat_buf[SECTORS_PER_FAT * BYTES_PER_SECTOR];
static uint8_t dir_buf[ROOT_DIR_SIZE * BYTES_PER_SECTOR];

static void fdc_dma_setup(uint32_t phys_addr, uint16_t count) {
    uint8_t lo, hi, pg;
    uint16_t cnt;

    cnt = count - 1;
    lo = (uint8_t)(phys_addr & 0xFF);
    hi = (uint8_t)((phys_addr >> 8) & 0xFF);
    pg = (uint8_t)((phys_addr >> 16) & 0xFF);

    outb(0x0C, 0x00);          // Reset DMA flip-flop
    outb(0x04, lo);            // Base/Current Address
    outb(0x04, hi);
    outb(0x81, pg);            // External Page Register
    outb(0x05, (uint8_t)(cnt & 0xFF));
    outb(0x05, (uint8_t)((cnt >> 8) & 0xFF));
    outb(0x0B, 0x56);          // Mode: single, inc, read, ch2
    outb(0x0A, 0x02);          // Unmask channel 2
}

static int fdc_read_sector(uint8_t drive, uint16_t cyl, uint8_t head, uint8_t sect, void *buf) {
    uint8_t msr, st0, st1, st2;
    uint32_t phys;
    int i, timeout;

    // So GCC wouldn't freak out because reasons ig
    st0 = 0;
    st0 = st0;

    phys = (uint32_t)buf;
    fdc_dma_setup(phys, 512);

    outb(0x3F2, 0x1C | drive); // DOR: motor on, DMA enable, drive select
    io_wait();

    while ((inb(0x3F4) & 0xC0) != 0x80);
    outb(0x3F5, 0xE6);         // CMD: Read Data (MFM)
    outb(0x3F5, (head << 2) | drive);
    outb(0x3F5, cyl);
    outb(0x3F5, head);
    outb(0x3F5, sect);
    outb(0x3F5, 2);            // N: 512 bytes
    outb(0x3F5, 18);           // EOT
    outb(0x3F5, 0x1B);         // GPL
    outb(0x3F5, 0xFF);         // DTL

    timeout = 100000;
    while (timeout > 0) {
        msr = inb(0x3F4);
        if ((msr & 0xC0) == 0xC0) break; // RQM=1, DIO=1
        timeout--;
    }
    if (timeout == 0) return -1;

    // Result Phase: 7 bytes
    for (i = 0; i < 7; i++) {
        while ((inb(0x3F4) & 0xC0) != 0xC0);
        msr = inb(0x3F5);
        if (i == 0) st0 = msr;
        else if (i == 1) st1 = msr;
        else if (i == 2) st2 = msr;
    }

    outb(0x3F2, 0x0C | drive); // Motor off
    return (st1 | st2) ? -1 : 0;
}

static int match_name(const uint8_t *entry, const char *target) {
    int i;
    for (i = 0; i < 11; i++) {
        if (entry[i] != (uint8_t)target[i]) return 0;
    }
    return 1;
}

static uint16_t get_next_cluster(uint16_t current) {
    uint32_t offset;
    uint16_t val;
    offset = current + (current / 2);
    val = fat_buf[offset] | (fat_buf[offset + 1] << 8);
    return (current & 1) ? (val >> 4) : (val & 0x0FFF);
}

int fat12_load_file(uint8_t drive, const char *filename_11, uint8_t *dest, uint32_t max_dest_size) {
    FatDirEntry *entries;
    uint16_t start_cluster, cluster;
    uint32_t file_size, bytes_read, chunk, lba;
    int i;
    uint8_t sec_buf[BYTES_PER_SECTOR];

    lba = ROOT_DIR_LBA;
    for (i = 0; i < ROOT_DIR_SIZE; i++) {
        uint16_t c, h, s;
        s = (lba % 18) + 1;
        h = (lba / 18) % 2;
        c = (lba / 36);
        if (fdc_read_sector(drive, c, h, s, dir_buf + (i * 512)) != 0) return -6;
        lba++;
    }

    entries = (FatDirEntry *)dir_buf;
    start_cluster = 0;
    file_size = 0;

    for (i = 0; i < ROOT_DIR_ENTRIES; i++) {
        if (entries[i].name[0] == 0x00) break;
        if (entries[i].name[0] == 0xE5) continue;
        if (entries[i].attr & 0x10) continue;

        if (match_name(entries[i].name, filename_11)) {
            start_cluster = entries[i].first_cluster;
            file_size = entries[i].size;
            break;
        }
    }
    if (start_cluster == 0) return -2;
    if (file_size > max_dest_size) return -5;

    lba = RESERVED_SECTORS;
    for (i = 0; i < SECTORS_PER_FAT; i++) {
        uint16_t c, h, s;
        s = (lba % 18) + 1;
        h = (lba / 18) % 2;
        c = (lba / 36);
        if (fdc_read_sector(drive, c, h, s, fat_buf + (i * 512)) != 0) return -7;
        lba++;
    }

    cluster = start_cluster;
    bytes_read = 0;

    while (cluster >= 0x002 && cluster <= 0xFF6 && bytes_read < file_size) {
        lba = DATA_AREA_LBA + (cluster - 2) * SECTORS_PER_CLUSTER;
        {
            uint16_t c, h, s;
            s = (lba % 18) + 1;
            h = (lba / 18) % 2;
            c = (lba / 36);
            if (fdc_read_sector(drive, c, h, s, (void *)sec_buf) != 0) return -8;
        }

        chunk = (file_size - bytes_read < BYTES_PER_SECTOR) ? (file_size - bytes_read) : BYTES_PER_SECTOR;
        memcpy(dest + bytes_read, (const void *)sec_buf, chunk);

        bytes_read += chunk;
        cluster = get_next_cluster(cluster);
    }

    return 0;
}
