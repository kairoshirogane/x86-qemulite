#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "memory.h"
#include "devices.h"

u8 ram[MEMORY_SIZE];

void mem_init() {
    memset(ram, 0, MEMORY_SIZE);
}

static u32 seg_to_phys(u16 seg, u16 off) {
    return ((u32)seg << 4) + off;
}

u8 mem_read8(u32 addr) {
    addr &= 0xFFFFF;
    if (addr >= 0xA0000 && addr < 0xC0000) {
        // Video memory window, handled by devices if needed, or just RAM for now
    }
    return ram[addr];
}

u16 mem_read16(u32 addr) {
    return mem_read8(addr) | (mem_read8(addr + 1) << 8);
}

void mem_write8(u32 addr, u8 val) {
    addr &= 0xFFFFF;
    if (addr >= 0xF0000) {
        // ROM, ignore write
        return;
    }
    ram[addr] = val;
    if (addr == 0xB8000) {
        // Simple hook for video update if needed
    }
}

void mem_write16(u32 addr, u16 val) {
    mem_write8(addr, val & 0xFF);
    mem_write8(addr + 1, val >> 8);
}

u8 mem_seg_read8(u16 seg, u16 off) {
    return mem_read8(seg_to_phys(seg, off));
}

u16 mem_seg_read16(u16 seg, u16 off) {
    return mem_read16(seg_to_phys(seg, off));
}

void mem_seg_write8(u16 seg, u16 off, u8 val) {
    mem_write8(seg_to_phys(seg, off), val);
}

void mem_seg_write16(u16 seg, u16 off, u16 val) {
    mem_write16(seg_to_phys(seg, off), val);
}

void mem_load_bin(const char *filename, u32 addr) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("mem_load_bin");
        return;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (addr + size > MEMORY_SIZE) {
        size = MEMORY_SIZE - addr;
    }
    if (fread(&ram[addr], 1, size, f) != (size_t)size) {
        perror("fread");
    }
    fclose(f);
}
