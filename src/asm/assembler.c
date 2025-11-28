#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <strings.h>

// ... (keep existing includes)

#include <stdbool.h>

#define MAX_LABELS 1024
#define MAX_LINE 512
#define BIN_SIZE 65536

typedef struct {
    char name[64];
    uint32_t addr;
} label_t;

label_t labels[MAX_LABELS];
int label_count = 0;
uint32_t current_addr = 0;
uint32_t start_addr = 0;
uint8_t bin[BIN_SIZE];
uint32_t bin_idx = 0;
int pass = 1;

void add_label(const char *name, uint32_t addr) {
    if (pass == 1) {
        strcpy(labels[label_count].name, name);
        labels[label_count].addr = addr;
        label_count++;
    }
}

uint32_t get_label(const char *name) {
    for (int i = 0; i < label_count; i++) {
        if (strcmp(labels[i].name, name) == 0) {
            return labels[i].addr;
        }
    }
    return 0;
}

void emit8(uint8_t b) {
    if (pass == 2) {
        if (bin_idx < BIN_SIZE) bin[bin_idx] = b;
        bin_idx++;
    }
    current_addr++;
}

void emit16(uint16_t w) {
    emit8(w & 0xFF);
    emit8(w >> 8);
}

char *trim(char *str) {
    while (isspace(*str)) str++;
    if (*str == 0) return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;
    end[1] = '\0';
    return str;
}

void parse_line(char *line) {
    line = trim(line);
    if (strlen(line) == 0 || line[0] == ';') return;

    char *colon = strchr(line, ':');
    if (colon) {
        *colon = '\0';
        add_label(line, current_addr);
        line = trim(colon + 1);
        if (strlen(line) == 0) return;
    }

    char *token = strtok(line, " ,\t");
    if (!token) return;

    if (strcasecmp(token, "org") == 0) {
        char *arg = strtok(NULL, " ,\t");
        uint32_t addr = strtol(arg, NULL, 0);
        current_addr = addr;
        if (pass == 1) start_addr = addr; // Assume first ORG sets start
        // In pass 2, we don't reset bin_idx, just current_addr tracks logical
    } else if (strcasecmp(token, "db") == 0) {
        char *arg;
        while ((arg = strtok(NULL, " ,\t"))) {
            if (arg[0] == '"' || arg[0] == '\'') {
                for (int i = 1; arg[i] && arg[i] != arg[0]; i++) emit8(arg[i]);
            } else {
                emit8((uint8_t)strtol(arg, NULL, 0));
            }
        }
    } else if (strcasecmp(token, "dw") == 0) {
        char *arg;
        while ((arg = strtok(NULL, " ,\t"))) {
            emit16((uint16_t)strtol(arg, NULL, 0));
        }
    } else if (strcasecmp(token, "times") == 0) {
        // Handle "times 510-($-$$) db 0"
        // Simplified: "times N db V"
        // We will manually parse the expression "510-($-$$)" if it matches exactly
        // Or just generic "times N"
        char *count_expr = strtok(NULL, " \t"); // "510-($-$$)"
        strtok(NULL, " \t"); // "db"
        char *val_str = strtok(NULL, " \t"); // "0"
        
        int count = 0;
        if (strstr(count_expr, "($-$$)")) {
            // Boot sector padding logic
            // $ = current_addr, $$ = start_addr
            // 510 - (current_addr - start_addr)
            int target = 510;
            if (isdigit(count_expr[0])) target = strtol(count_expr, NULL, 0);
            count = target - (current_addr - start_addr);
        } else {
            count = strtol(count_expr, NULL, 0);
        }
        
        int val = strtol(val_str, NULL, 0);
        for (int i = 0; i < count; i++) emit8(val);

    } else if (strcasecmp(token, "mov") == 0) {
        char *dest = strtok(NULL, ",");
        char *src = strtok(NULL, ",");
        dest = trim(dest);
        src = trim(src);

        if (strcasecmp(dest, "ax") == 0) {
            if (strcasecmp(src, "cs") == 0) { emit8(0x8C); emit8(0xC8); } // MOV AX, CS
            else { emit8(0xB8); emit16(strtol(src, NULL, 0)); } // MOV AX, imm16
        } else if (strcasecmp(dest, "ds") == 0) {
            if (strcasecmp(src, "ax") == 0) { emit8(0x8E); emit8(0xD8); } // MOV DS, AX
        } else if (strcasecmp(dest, "es") == 0) {
            if (strcasecmp(src, "ax") == 0) { emit8(0x8E); emit8(0xC0); } // MOV ES, AX
        } else if (strcasecmp(dest, "ss") == 0) {
            if (strcasecmp(src, "ax") == 0) { emit8(0x8E); emit8(0xD0); } // MOV SS, AX
        } else if (strcasecmp(dest, "sp") == 0) {
             emit8(0xBC); emit16(strtol(src, NULL, 0)); // MOV SP, imm16
        } else if (strcasecmp(dest, "si") == 0) {
            // Check if src is label
            uint32_t val = isdigit(src[0]) ? strtol(src, NULL, 0) : get_label(src);
            emit8(0xBE); emit16(val);
        } else if (strcasecmp(dest, "ah") == 0) {
            emit8(0xB4); emit8(strtol(src, NULL, 0));
        } else if (strcasecmp(dest, "al") == 0) {
            emit8(0xB0); emit8(strtol(src, NULL, 0));
        }
    } else if (strcasecmp(token, "jmp") == 0) {
        char *target = strtok(NULL, " ,");
        if (strcmp(target, "$") == 0) {
            emit8(0xEB); emit8(0xFE);
        } else {
            // Near jump E9
            uint32_t addr = get_label(target);
            emit8(0xE9);
            int32_t rel = addr - (current_addr + 2);
            emit16((uint16_t)rel);
        }
    } else if (strcasecmp(token, "int") == 0) {
        char *arg = strtok(NULL, " ,");
        emit8(0xCD); emit8(strtol(arg, NULL, 0));
    } else if (strcasecmp(token, "cli") == 0) emit8(0xFA);
      else if (strcasecmp(token, "sti") == 0) emit8(0xFB);
      else if (strcasecmp(token, "hlt") == 0) emit8(0xF4);
      else if (strcasecmp(token, "add") == 0) {
          char *dest = strtok(NULL, ",");
          char *src = strtok(NULL, ",");
          dest = trim(dest); src = trim(src);
          if (strcasecmp(dest, "al") == 0) { emit8(0x04); emit8(strtol(src, NULL, 0)); }
      }
}

int main(int argc, char **argv) {
    if (argc != 3) { printf("Usage: %s <in> <out>\n", argv[0]); return 1; }

    for (pass = 1; pass <= 2; pass++) {
        current_addr = 0;
        bin_idx = 0;
        FILE *f = fopen(argv[1], "r");
        if (!f) return 1;
        char line[MAX_LINE];
        while (fgets(line, sizeof(line), f)) {
            char buf[MAX_LINE];
            strcpy(buf, line);
            parse_line(buf);
        }
        fclose(f);
    }

    FILE *out = fopen(argv[2], "wb");
    fwrite(bin, 1, bin_idx, out);
    fclose(out);
    return 0;
}
