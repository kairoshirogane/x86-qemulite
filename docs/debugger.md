# Debugger

The debugger is built into the emulator.

## Commands

- `s`: Step one instruction.
- `c`: Continue execution until halt or breakpoint.
- `r`: Dump registers.
- `m <addr>`: Dump 16 bytes of memory at hex address.
- `u`: Disassemble next 5 instructions.
- `q`: Quit.

## Interface

The debugger uses a CLI REPL. It pauses execution on startup.
