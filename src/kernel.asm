[BITS 32]

global _start
global problem
global kernel_registers

extern kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10

; after the boot complete the process the system is provided to kernel to start working
; so this is the start of the kernel so the kernel will jump to the given location
_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp

    ; enabling A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; remap the master PIC (Programmable Interrupt Controller)
    ; ISR address start from 0x20
    ; so int 0 at 0x20, int 1 at 0x21, and so
    mov al, 00010001b
    out 0x20,al ; tell master PIC

    mov al, 0x20 ; interrupt 0x20 is where master ISR should start
    out 0x21, al

    mov al, 00000001b
    out 0x21, al
    ;end remap of the master PIC
    
    call kernel_main

    jmp $

kernel_registers:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    ret

problem:
    ; in idt.c there we are declaring int 0 for how to manage it
    ; so we are calling interrupt here
    ; if there idt_set has 32 then we make here int 32
    int 0

times 512-($ - $$) db 0 ; for alingning properly with 16 bit system as 512/16=0