#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cpu.h"
#include "memory.h"

// forward declaration
void disasm_instruction(u8 *memory, u32 *offset, char *buf);

void debugger_run() {
    char cmd[64];
    printf("Debugger started. Type 'h' for help.\n");

    while (1) {
        printf("(dbg) ");
        if (!fgets(cmd, sizeof(cmd), stdin)) break;
        
        if (cmd[0] == 'q') break;
        else if (cmd[0] == 'h') {
            printf("Commands:\n s - step\n c - continue\n r - regs\n m <addr> - memory\n u - disasm\n q - quit\n");
        } else if (cmd[0] == 's') {
            cpu_step();
            cpu_dump();
        } else if (cmd[0] == 'r') {
            cpu_dump();
        } else if (cmd[0] == 'c') {
            // run until halt or interrupt (simplified)
            while (!cpu.halted) {
                cpu_step();
            }
            printf("CPU Halted.\n");
        } else if (cmd[0] == 'u') {
            u32 offset = ((u32)cpu.regs.cs << 4) + cpu.regs.ip;
            char buf[64];
            // disassemble next 5 instructions
            for (int i=0; i<5; i++) {
                u32 old = offset;
                disasm_instruction(ram, &offset, buf);
                printf("%04X: %s\n", old, buf);
            }
        } else if (cmd[0] == 'm') {
            u32 addr = strtol(cmd + 2, NULL, 16);
            for (int i=0; i<16; i++) {
                printf("%02X ", mem_read8(addr + i));
            }
            printf("\n");
        }
    }
}
