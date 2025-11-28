# CPU Specification

The emulator implements a subset of the Intel 8086/80186 architecture.

## Registers

- **General Purpose**: AX, BX, CX, DX
- **Index/Pointer**: SI, DI, BP, SP
- **Segment**: CS, DS, SS, ES
- **Control**: IP, FLAGS

## Flags

- **CF**: Carry Flag
- **ZF**: Zero Flag
- **SF**: Sign Flag
- **IF**: Interrupt Flag
- **TF**: Trap Flag (Single step)

## Pipeline

The CPU simulates a prefetch queue of 6 bytes. Instructions are fetched from the queue, not directly from memory (mostly). Jumps flush the queue.

## Interrupts

- **INT 10h**: Video Services
- **INT 13h**: Disk Services
- **INT 16h**: Keyboard Services
