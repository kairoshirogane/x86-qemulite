# Architecture

The system is designed as a modular set of components:

## Core Components

1. **CPU (`src/emu/cpu.c`)**:
   - Implements the fetch-decode-execute cycle.
   - Manages the 6-byte prefetch queue.
   - Handles interrupts and exceptions.

2. **Memory (`src/emu/memory.c`)**:
   - Manages the 1MB physical address space.
   - Handles segmentation logic (Segment:Offset -> Physical).
   - Maps BIOS and Video memory regions.

3. **Devices (`src/emu/devices.c`)**:
   - Simulates I/O ports.
   - Handles hardware interrupts (PIC, Timer - stubbed).

## Toolchain

- **Assembler**: Converts `.asm` source to flat binary `.bin`.
- **Disassembler**: Decodes binary to mnemonic strings.
- **Debugger**: Hooks into the CPU loop to provide control.
