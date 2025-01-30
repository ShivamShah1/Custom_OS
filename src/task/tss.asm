/*
    this is a assembly file for the TSS(Task Switching Segment)
    When the kernel switches from kernel mode to user mode it
    will pull this segment to continue its old work 
    or from user mode to kernel mode, it will push this segment
    for future completion. 
*/
section .asm

global tss_load

tss_load:
    push ebp
    mov ebp, esp
    mov ax, [ebp+8] ; TSS segment
    ltr ax
    pop ebp
    ret