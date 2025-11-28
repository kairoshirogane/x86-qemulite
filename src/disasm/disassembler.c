#include <stdio.h>
#include <string.h>
#include "types.h"

// helper to read bytes safely
static u8 fetch(u8 *memory, u32 *offset) {
    return memory[(*offset)++];
}

static u16 fetch16(u8 *memory, u32 *offset) {
    u8 l = fetch(memory, offset);
    u8 h = fetch(memory, offset);
    return l | (h << 8);
}

void disasm_instruction(u8 *memory, u32 *offset, char *buf) {
    u8 b = fetch(memory, offset);
    
    switch (b) {
        case 0x90: sprintf(buf, "nop"); break;
        case 0xF4: sprintf(buf, "hlt"); break;
        case 0xFA: sprintf(buf, "cli"); break;
        case 0xFB: sprintf(buf, "sti"); break;
        case 0xC3: sprintf(buf, "ret"); break;
        
        case 0xB0 ... 0xB7: 
            sprintf(buf, "mov r8, 0x%02X", fetch(memory, offset)); 
            break;
        case 0xB8 ... 0xBF: 
            sprintf(buf, "mov r16, 0x%04X", fetch16(memory, offset)); 
            break;
            
        case 0xCD:
            sprintf(buf, "int 0x%02X", fetch(memory, offset));
            break;
            
        case 0xEB:
            sprintf(buf, "jmp short 0x%02X", fetch(memory, offset));
            break;
        case 0xE9:
            sprintf(buf, "jmp near 0x%04X", fetch16(memory, offset));
            break;
            
        case 0x04:
            sprintf(buf, "add al, 0x%02X", fetch(memory, offset));
            break;
            
        case 0x8E: // mov sreg, r/m16
            fetch(memory, offset); // skip ModRM for now
            sprintf(buf, "mov sreg, r16");
            break;
        case 0x8C: // mov r/m16, sreg
            fetch(memory, offset);
            sprintf(buf, "mov r16, sreg");
            break;
            
        default:
            sprintf(buf, "db 0x%02X", b);
            break;
    }
}
