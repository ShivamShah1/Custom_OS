; to tell the processor where is the interrupt script table is
; as using C we cannot do that as it cannot tell processor 
; the location of the table
 
section .asm

extern int21g_handler
extern no_interrupt_handler
extern isr80h_handler

global int21h
global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr80h_wrapper

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
    pushad
    call int21h_handler
    popad
    iret

; we write this interrupt bcoz we dont want the kernel to do anything else if keyboard 
; interrupt occurs otherwise it will loop in bios mode continously
no_interrupt:
    pushad
    call no_interrupt_handler
    popad
    iret

; calling kernel from the userspace
; selecting 0x80 as interrupt to give control to kernel from userspace
isr80h_wrapper:
    ; interrupt frame start
    ; already pushed to us by the processor upon entry to this interrupt
    ; uint32_t ip;
    ; uint32_t cs;
    ; uint32_t flags
    ; uint32_t sp;
    ; uint32_t ss;
    ; pushes the general purpose regs to the stack
    pushad

    ; interrupt frame end

    ; push the stack pointer so that we are pointing to
    ; the interrupt frame
    push esp

    ; EAX holds our cmd lets push it to the stack for isr80h_handler
    push eax
    call isr80h_handler
    mov dword[tmp_res], eax
    add esp, 8

    ; restore general purpose regs for user land
    popad
    mov eax, [tmp_res]
    iretd

section .data
; inside here is stored the return result from isr80h_handler
tmp_res dd 0