#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "memory.h"
#include "instructions.h"
#include "bios.h"

cpu_t cpu;

void cpu_init() {
    memset(&cpu, 0, sizeof(cpu));
    cpu_reset();
}

void cpu_reset() {
    cpu.regs.cs = 0x0000;
    cpu.regs.ip = 0x7C00;
    cpu.regs.flags = 0x0002; // reserved bit always 1
    cpu.queue_size = 0;
    cpu.halted = false;
    
    // initial register state (simulating BIOS handoff)
    cpu.regs.dx = 0x0000;
}

void cpu_fill_queue() {
    while (cpu.queue_size < 6) {
        u32 phys = ((u32)cpu.regs.cs << 4) + cpu.regs.ip + cpu.queue_size;
        cpu.prefetch_queue[cpu.queue_size++] = mem_read8(phys);
    }
}

u8 cpu_fetch_byte() {
    if (cpu.queue_size == 0) {
        cpu_fill_queue();
    }
    u8 b = cpu.prefetch_queue[0];
    // shift queue
    for (int i = 0; i < cpu.queue_size - 1; i++) {
        cpu.prefetch_queue[i] = cpu.prefetch_queue[i+1];
    }
    cpu.queue_size--;
    cpu.regs.ip++;
    cpu_fill_queue(); // refill immediately
    return b;
}

u16 cpu_fetch_word() {
    u8 low = cpu_fetch_byte();
    u8 high = cpu_fetch_byte();
    return low | (high << 8);
}

void cpu_push16(u16 val) {
    cpu.regs.sp -= 2;
    mem_seg_write16(cpu.regs.ss, cpu.regs.sp, val);
}

u16 cpu_pop16() {
    u16 val = mem_seg_read16(cpu.regs.ss, cpu.regs.sp);
    cpu.regs.sp += 2;
    return val;
}

void cpu_interrupt(u8 int_num) {
    cpu_push16(cpu.regs.flags);
    cpu_push16(cpu.regs.cs);
    cpu_push16(cpu.regs.ip);
    
    cpu.regs.flags &= ~(FLAG_IF | FLAG_TF);
    
    // read IVT
    u16 new_ip = mem_read16(int_num * 4);
    u16 new_cs = mem_read16(int_num * 4 + 2);
    
    cpu.regs.ip = new_ip;
    cpu.regs.cs = new_cs;
    cpu.queue_size = 0; // flush queue on jump
}

void cpu_step() {
    if (cpu.halted) return;

    if (cpu.int_pending) {
        cpu.int_pending = false;
        cpu_interrupt(cpu.int_number);
        return;
    }

    u8 opcode = cpu_fetch_byte();
    exec_opcode(opcode);
}

void cpu_dump() {
    printf("AX=%04X BX=%04X CX=%04X DX=%04X SP=%04X BP=%04X SI=%04X DI=%04X\n",
           cpu.regs.ax, cpu.regs.bx, cpu.regs.cx, cpu.regs.dx,
           cpu.regs.sp, cpu.regs.bp, cpu.regs.si, cpu.regs.di);
    printf("CS=%04X DS=%04X SS=%04X ES=%04X IP=%04X FLAGS=%04X\n",
           cpu.regs.cs, cpu.regs.ds, cpu.regs.ss, cpu.regs.es,
           cpu.regs.ip, cpu.regs.flags);
}
