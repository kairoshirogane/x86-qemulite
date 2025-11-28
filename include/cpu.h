#ifndef CPU_H
#define CPU_H

#include "types.h"

#define FLAG_CF 0x0001
#define FLAG_PF 0x0004
#define FLAG_AF 0x0010
#define FLAG_ZF 0x0040
#define FLAG_SF 0x0080
#define FLAG_TF 0x0100
#define FLAG_IF 0x0200
#define FLAG_DF 0x0400
#define FLAG_OF 0x0800

typedef struct {
    union {
        struct { u8 al, ah; };
        u16 ax;
    };
    union {
        struct { u8 bl, bh; };
        u16 bx;
    };
    union {
        struct { u8 cl, ch; };
        u16 cx;
    };
    union {
        struct { u8 dl, dh; };
        u16 dx;
    };
    u16 si, di, bp, sp;
    u16 cs, ds, ss, es;
    u16 ip;
    u16 flags;
} registers_t;

typedef struct {
    registers_t regs;
    u8 prefetch_queue[6];
    int queue_size;
    bool halted;
    bool int_pending;
    u8 int_number;
} cpu_t;

extern cpu_t cpu;

void cpu_init();
void cpu_reset();
void cpu_step();
void cpu_dump();
u8 cpu_fetch_byte();
u16 cpu_fetch_word();
void cpu_push16(u16 val);
u16 cpu_pop16();
void cpu_interrupt(u8 int_num);

#endif
