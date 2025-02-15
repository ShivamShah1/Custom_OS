%%if 0
    creating the first program where it will jump in to itself
%%endif
[BITS 32]

section .asm

global _start

_start:

; looping to print multilple character on screen or terminal
_loop:
    call getkey
    push eax
    mov eax, 3 ; command putchar
    int 0x80
    add esp, 4
    jmp _loop

; provides next key to be popped from the buffer
getkey:
    mov eax, 2 ; command getkey
    int 0x80
    cmp eax, 0x00
    je getkey
    ret

section .data
message: db 'This is from user space', 0