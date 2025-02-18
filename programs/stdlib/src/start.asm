%%if 0
    creating the first program where it will jump in to itself
%%endif
[BITS 32]

global _start

extern c_start

section .asm

_start:
    call c_start
    ret