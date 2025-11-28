org 0x7C00
mov ax, 0xB800
mov es, ax
; My assembler is limited, so we just set segments to show we can
mov ax, 0x1234
hlt
