%%if 0
    creating the first program where it will jump in to itself
%%endif
[BITS 32]

section .asm

global _start

_start:
    push message
    mov eax, 1 ; command print
    int 0x80
    add esp, 4
    
    jmp $

section .data
message: db 'This is from user space', 0