#include <stdio.h>
#include "cpu.h"
#include "memory.h"
#include "bios.h"
#include "devices.h"

// From debugger.c
void debugger_run();

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <boot.bin>\n", argv[0]);
        return 1;
    }

    mem_init();
    cpu_init();
    bios_init();
    devices_init();

    printf("Loading %s...\n", argv[1]);
    mem_load_bin(argv[1], 0x7C00);

    // Start in debugger
    debugger_run();

    return 0;
}
