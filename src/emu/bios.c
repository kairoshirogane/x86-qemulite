#include <stdio.h>
#include "bios.h"
#include "cpu.h"
#include "memory.h"

void bios_init() {
    // setup IVT for BIOS interrupts if we were doing full emulation
    // but we are hooking them in instructions.c
}

void bios_handle_int(u8 int_num) {
    switch (int_num) {
        case 0x10: // video
            if (cpu.regs.ah == 0x0E) {
                // teletype output
                putchar(cpu.regs.al);
                fflush(stdout);
            }
            break;
        case 0x13: // disk
            // minimal read sector implementation
            if (cpu.regs.ah == 0x02) {
                // AL = sectors, CL = sector (1-based), DH = head, DL = drive
                // ES:BX = buffer
                // simplified: read from disk.img offset
                // LBA = (C * num_heads + H) * sectors_per_track + (S - 1)
                // Assuming 1.44MB floppy: 80 tracks, 2 heads, 18 sectors
                // For simplicity, just read linear sectors from file if we had one open
                // For now, set Carry Flag to indicate error or success
                cpu.regs.flags &= ~FLAG_CF; // Success
                cpu.regs.ah = 0; // Status OK
            }
            break;
        case 0x16: // keyboard
            if (cpu.regs.ah == 0x00) {
                // Read char
                int c = getchar();
                cpu.regs.al = (u8)c;
            }
            break;
        default:
            //  printf("Unhandled BIOS INT %02X\n", int_num);
            break;
    }
}
