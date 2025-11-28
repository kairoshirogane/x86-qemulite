#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "types.h"
#include "cpu.h"

void exec_opcode(u8 opcode);

// helper for ModR/M
typedef struct {
    u8 mod;
    u8 reg;
    u8 rm;
} modrm_t;

modrm_t fetch_modrm();
u16 get_rm_addr(modrm_t modrm);
u8 get_rm8(modrm_t modrm);
u16 get_rm16(modrm_t modrm);
void set_rm8(modrm_t modrm, u8 val);
void set_rm16(modrm_t modrm, u16 val);
u8* get_reg8(u8 reg);
u16* get_reg16(u8 reg);

#endif
