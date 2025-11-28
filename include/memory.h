#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

#define MEMORY_SIZE (1024 * 1024)

extern u8 ram[MEMORY_SIZE];

void mem_init();
u8 mem_read8(u32 addr);
u16 mem_read16(u32 addr);
void mem_write8(u32 addr, u8 val);
void mem_write16(u32 addr, u16 val);

u8 mem_seg_read8(u16 seg, u16 off);
u16 mem_seg_read16(u16 seg, u16 off);
void mem_seg_write8(u16 seg, u16 off, u8 val);
void mem_seg_write16(u16 seg, u16 off, u16 val);

void mem_load_bin(const char *filename, u32 addr);

#endif
