%%if 0
    This is the assembly file for the paging to load and enable paging
    as C lang does not support this instructions in Intel
%%endif

[BITS 32]

section .asm

global paging_load_directory
global enable_paging

paging_load_directory:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]
    mov cr3, eax
    pop ebp
    ret

enable_paging:
    push ebp
    mov ebp, esp
    mov eax, cr0
    or eax, 0x80000000
    mov cr3, eax
    pop ebp
    ret