# Usage Guide

## Building

Run `make` in the root directory.

## Running Examples

1. **Bootloader**:
   ```bash
   ./x86emu examples/boot.bin
   ```
   This will print "Hello" via BIOS INT 10h.

2. **Math**:
   ```bash
   ./x86emu examples/math.bin
   ```
   Step through to see registers change.

## Creating Your Own Programs

1. Write assembly code in `myprog.asm`.
2. Assemble it:
   ```bash
   ./x86asm myprog.asm myprog.bin
   ```
3. Run it:
   ```bash
   ./x86emu myprog.bin
   ```
