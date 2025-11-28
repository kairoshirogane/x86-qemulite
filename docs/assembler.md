# Assembler

The `x86asm` tool is a two-pass assembler.

## Usage

```bash
./x86asm <input.asm> <output.bin>
```

## Syntax

- **Labels**: `label_name:`
- **Directives**:
  - `org <addr>`: Set origin address.
  - `db <val/string>`: Define bytes.
  - `dw <val>`: Define words.
  - `times <count> db <val>`: Repeat data.
- **Instructions**:
  - `mov dest, src`
  - `add dest, src`
  - `jmp target`
  - `int n`
  - `cli`, `sti`, `hlt`

## Example

```asm
org 0x7C00
start:
    mov ax, 0x0E41
    int 0x10
    jmp $
```
