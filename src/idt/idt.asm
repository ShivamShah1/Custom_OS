; to tell the processor where is the interrupt script table is
; as using C we cannot do that as it cannot tell processor 
; the location of the table
 
section .asm

extern int21g_handler
extern no_interrupt_handler

global int21h
global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret
    
idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8]
    lidt [ebx]
    pop ebp
    ret

int21h:
    cli
    pushad
    call int21h_handler
    popad
    sti
    iret

; we write this interrupt bcoz we dont want the kernel to do anything else if keyboard 
; interrupt occurs otherwise it will loop in bios mode continously
no_interrupt:
    cli
    pushad
    call no_interrupt_handler
    popad
    sti
    iret