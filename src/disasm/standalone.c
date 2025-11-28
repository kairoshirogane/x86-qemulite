#include <stdio.h>
#include <stdlib.h>
#include "types.h"

// forward declaration
void disasm_instruction(u8 *memory, u32 *offset, char *buf);

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <binary file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    u8 *buf = malloc(size);
    if (fread(buf, 1, size, f) != (size_t)size) {
        perror("fread");
        free(buf);
        fclose(f);
        return 1;
    }
    fclose(f);

    u32 offset = 0;
    char line[256];
    while (offset < (u32)size) {
        u32 old_offset = offset;
        disasm_instruction(buf, &offset, line);
        printf("%04X: ", old_offset);
        for (u32 i = old_offset; i < offset; i++) {
            printf("%02X ", buf[i]);
        }
        for (u32 i = 0; i < (8 - (offset - old_offset)); i++) {
            printf("   ");
        }
        printf(" %s\n", line);
    }

    free(buf);
    return 0;
}
