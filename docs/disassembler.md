# Disassembler

The `x86dis` tool decodes binary files into assembly mnemonics.

## Usage

```bash
./x86dis <binary_file>
```

## Output Format

```
OFFSET: BYTES       MNEMONIC
0000: B8 00 10    mov ax, 0x1000
```
