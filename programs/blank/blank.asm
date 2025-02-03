%%if 0
    creating the first program where it will jump in to itself
%%endif
[BITS 32]

section .asm

global _start

_start:

label:
    jmp label