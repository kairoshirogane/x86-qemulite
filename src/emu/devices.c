#include <stdio.h>
#include "devices.h"

void devices_init() {
}

u8 io_read8(u16 port) {
    // 0x60 = keyboard data
    // 0x64 = keyboard status
    if (port == 0x60) return 0;
    if (port == 0x64) return 0;
    return 0xFF;
}

void io_write8(u16 port, u8 val) {
    // 0x3D4/0x3D5 = CRT controller
    if (port == 0x3F8) {
        putchar(val); // serial output
    }
}
