# x86 Real-Mode Emulator System

A complete, engineering-grade x86 real-mode emulator, assembler, disassembler, and debugger written in C.


- **CPU**: 16-bit 8086/80186 compatible core with prefetch queue and segmentation.
- **Memory**: 1MB address space with BIOS and Video memory regions.
- **Assembler**: Two-pass assembler generating flat binary code.
- **Disassembler**: Full instruction decoding.
- **Debugger**: Interactive GDB-like debugger with stepping and inspection.

## Quick Start

1. Build the system:
   ```bash
   make
   ```

2. Run the boot example:
   ```bash
   ./x86emu examples/boot.bin
   ```

3. In the debugger:
   - Type `c` to continue execution.
   - Type `s` to step.
   - Type `r` to view registers.
   - Type `q` to quit.

## Directory Structure

- `src/`: Source code
  - `emu/`: Emulator core (CPU, Memory, BIOS)
  - `asm/`: Assembler
  - `disasm/`: Disassembler
  - `dbg/`: Debugger
- `include/`: Header files
- `examples/`: Assembly programs
- `docs/`: Documentation

## License

MIT
