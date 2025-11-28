CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude -O2
LDFLAGS = 

SRC_COMMON = src/common/types.h
SRC_EMU = src/emu/cpu.c src/emu/memory.c src/emu/instructions.c src/emu/bios.c src/emu/devices.c
SRC_ASM = src/asm/assembler.c
SRC_DISASM = src/disasm/disassembler.c
SRC_DBG = src/dbg/debugger.c
SRC_MAIN = src/main.c

OBJ_EMU = $(SRC_EMU:.c=.o)
OBJ_ASM = $(SRC_ASM:.c=.o)
OBJ_DISASM = $(SRC_DISASM:.c=.o)
OBJ_DBG = $(SRC_DBG:.c=.o)
OBJ_MAIN = $(SRC_MAIN:.c=.o)

TARGET = x86emu
ASM_TARGET = x86asm
DISASM_TARGET = x86dis

all: $(TARGET) $(ASM_TARGET) $(DISASM_TARGET) examples

$(TARGET): $(OBJ_MAIN) $(OBJ_EMU) $(OBJ_DBG) $(OBJ_DISASM)
	$(CC) $(LDFLAGS) -o $@ $^

$(ASM_TARGET): $(OBJ_ASM)
	$(CC) $(LDFLAGS) -o $@ $^

$(DISASM_TARGET): src/disasm/standalone.o $(OBJ_DISASM)
	$(CC) $(LDFLAGS) -o $@ $^

src/disasm/standalone.o: src/disasm/standalone.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

examples: $(ASM_TARGET)
	./$(ASM_TARGET) examples/boot.asm examples/boot.bin
	./$(ASM_TARGET) examples/math.asm examples/math.bin
	./$(ASM_TARGET) examples/video.asm examples/video.bin

clean:
	rm -f $(OBJ_EMU) $(OBJ_ASM) $(OBJ_DISASM) $(OBJ_DBG) $(OBJ_MAIN) src/disasm/standalone.o $(TARGET) $(ASM_TARGET) $(DISASM_TARGET) examples/*.bin

.PHONY: all clean examples
