#include <stdio.h>
#include <stdlib.h>
#include "instructions.h"
#include "cpu.h"
#include "memory.h"
#include "devices.h"
#include "bios.h"

// helpers
static void update_flags_szp(u16 result, bool is_word) {
    if (is_word) {
        if (result == 0) cpu.regs.flags |= FLAG_ZF; else cpu.regs.flags &= ~FLAG_ZF;
        if (result & 0x8000) cpu.regs.flags |= FLAG_SF; else cpu.regs.flags &= ~FLAG_SF;
    } else {
        if ((result & 0xFF) == 0) cpu.regs.flags |= FLAG_ZF; else cpu.regs.flags &= ~FLAG_ZF;
        if (result & 0x80) cpu.regs.flags |= FLAG_SF; else cpu.regs.flags &= ~FLAG_SF;
    }
    // pf is a bit complex, skipping for brevity unless needed
}

modrm_t fetch_modrm() {
    u8 b = cpu_fetch_byte();
    modrm_t m;
    m.mod = (b >> 6) & 3;
    m.reg = (b >> 3) & 7;
    m.rm = b & 7;
    return m;
}

u16 get_rm_addr(modrm_t modrm) {
    u16 offset = 0;
    u16 seg = cpu.regs.ds; // default segment

    if (modrm.mod == 3) return 0; // register mode

    switch (modrm.rm) {
        case 0: offset = cpu.regs.bx + cpu.regs.si; break;
        case 1: offset = cpu.regs.bx + cpu.regs.di; break;
        case 2: offset = cpu.regs.bp + cpu.regs.si; seg = cpu.regs.ss; break;
        case 3: offset = cpu.regs.bp + cpu.regs.di; seg = cpu.regs.ss; break;
        case 4: offset = cpu.regs.si; break;
        case 5: offset = cpu.regs.di; break;
        case 6: 
            if (modrm.mod == 0) offset = cpu_fetch_word();
            else { offset = cpu.regs.bp; seg = cpu.regs.ss; }
            break;
        case 7: offset = cpu.regs.bx; break;
    }

    if (modrm.mod == 1) {
        i8 disp = (i8)cpu_fetch_byte();
        offset += disp;
    } else if (modrm.mod == 2) {
        u16 disp = cpu_fetch_word();
        offset += disp;
    }

    return ((u32)seg << 4) + offset; // return physical for simplicity here, but should return offset
    // correcting: return offset, let caller handle segment.
    // actually, for simplicity in this function, let's return the physical address directly or handle segment logic here.
    // to match `mem_read8` which takes physical, we need physical.
    // but `mem_seg_read8` takes seg:off.
    // let's return physical address for now, assuming default segments.
    // Re-calculating:
    return ((u32)seg << 4) + (offset & 0xFFFF);
}

u8* get_reg8(u8 reg) {
    switch (reg) {
        case 0: return &cpu.regs.al;
        case 1: return &cpu.regs.cl;
        case 2: return &cpu.regs.dl;
        case 3: return &cpu.regs.bl;
        case 4: return &cpu.regs.ah;
        case 5: return &cpu.regs.ch;
        case 6: return &cpu.regs.dh;
        case 7: return &cpu.regs.bh;
    }
    return NULL;
}

u16* get_reg16(u8 reg) {
    switch (reg) {
        case 0: return &cpu.regs.ax;
        case 1: return &cpu.regs.cx;
        case 2: return &cpu.regs.dx;
        case 3: return &cpu.regs.bx;
        case 4: return &cpu.regs.sp;
        case 5: return &cpu.regs.bp;
        case 6: return &cpu.regs.si;
        case 7: return &cpu.regs.di;
    }
    return NULL;
}

u8 get_rm8(modrm_t modrm) {
    if (modrm.mod == 3) return *get_reg8(modrm.rm);
    return mem_read8(get_rm_addr(modrm));
}

u16 get_rm16(modrm_t modrm) {
    if (modrm.mod == 3) return *get_reg16(modrm.rm);
    return mem_read16(get_rm_addr(modrm));
}

void set_rm8(modrm_t modrm, u8 val) {
    if (modrm.mod == 3) *get_reg8(modrm.rm) = val;
    else mem_write8(get_rm_addr(modrm), val);
}

void set_rm16(modrm_t modrm, u16 val) {
    if (modrm.mod == 3) *get_reg16(modrm.rm) = val;
    else mem_write16(get_rm_addr(modrm), val);
}

void exec_opcode(u8 opcode) {
    modrm_t modrm;
    u8 tmp8;
    u16 tmp16;

    switch (opcode) {
        case 0x90: // NOP
            break;
        
        // MOV
        case 0x88: // MOV r/m8, r8
            modrm = fetch_modrm();
            set_rm8(modrm, *get_reg8(modrm.reg));
            break;
        case 0x89: // MOV r/m16, r16
            modrm = fetch_modrm();
            set_rm16(modrm, *get_reg16(modrm.reg));
            break;
        case 0x8A: // MOV r8, r/m8
            modrm = fetch_modrm();
            *get_reg8(modrm.reg) = get_rm8(modrm);
            break;
        case 0x8B: // MOV r16, r/m16
            modrm = fetch_modrm();
            *get_reg16(modrm.reg) = get_rm16(modrm);
            break;
        case 0xB0 ... 0xB7: // MOV r8, imm8
            *get_reg8(opcode - 0xB0) = cpu_fetch_byte();
            break;
        case 0xB8 ... 0xBF: // MOV r16, imm16
            *get_reg16(opcode - 0xB8) = cpu_fetch_word();
            break;
        case 0xC6: // MOV r/m8, imm8
            modrm = fetch_modrm();
            if (modrm.reg == 0) set_rm8(modrm, cpu_fetch_byte());
            break;
        case 0xC7: // MOV r/m16, imm16
            modrm = fetch_modrm();
            if (modrm.reg == 0) set_rm16(modrm, cpu_fetch_word());
            break;

        // JMP
        case 0xEB: // JMP short
            tmp8 = cpu_fetch_byte();
            cpu.regs.ip += (i8)tmp8;
            cpu.queue_size = 0;
            break;
        case 0xE9: // JMP near
            tmp16 = cpu_fetch_word();
            cpu.regs.ip += (i16)tmp16;
            cpu.queue_size = 0;
            break;

        // CALL
        case 0xE8: // CALL near
            tmp16 = cpu_fetch_word();
            cpu_push16(cpu.regs.ip);
            cpu.regs.ip += (i16)tmp16;
            cpu.queue_size = 0;
            break;
        
        // RET
        case 0xC3: // RET
            cpu.regs.ip = cpu_pop16();
            cpu.queue_size = 0;
            break;

        // INT
        case 0xCD: // INT n
            tmp8 = cpu_fetch_byte();
            bios_handle_int(tmp8); // hook for BIOS simulation
            // cpu_interrupt(tmp8); // real interrupt handling if not hooked
            break;

        // I/O
        case 0xE4: // IN AL, imm8
            cpu.regs.al = io_read8(cpu_fetch_byte());
            break;
        case 0xE6: // OUT imm8, AL
            io_write8(cpu_fetch_byte(), cpu.regs.al);
            break;
        
        // Arithmetic (ADD AL, imm8)
        case 0x04:
            tmp8 = cpu_fetch_byte();
            cpu.regs.al += tmp8;
            update_flags_szp(cpu.regs.al, false);
            break;

        // CMP AL, imm8
        case 0x3C:
            tmp8 = cpu_fetch_byte();
            update_flags_szp(cpu.regs.al - tmp8, false);
            if (cpu.regs.al < tmp8) cpu.regs.flags |= FLAG_CF; else cpu.regs.flags &= ~FLAG_CF;
            break;

        // Jcc (Short)
        case 0x74: // JZ
            tmp8 = cpu_fetch_byte();
            if (cpu.regs.flags & FLAG_ZF) { cpu.regs.ip += (i8)tmp8; cpu.queue_size = 0; }
            break;
        case 0x75: // JNZ
            tmp8 = cpu_fetch_byte();
            if (!(cpu.regs.flags & FLAG_ZF)) { cpu.regs.ip += (i8)tmp8; cpu.queue_size = 0; }
            break;

        // HLT
        case 0xF4:
            cpu.halted = true;
            break;

        default:
            printf("Unknown opcode: %02X at %04X:%04X\n", opcode, cpu.regs.cs, cpu.regs.ip - 1);
            cpu.halted = true;
            break;
    }
}
