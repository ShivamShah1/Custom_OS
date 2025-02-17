[BITS 32]

section .asm

global print:function
global getkey:function
global peachos_malloc:function
global peachos_free:function

; void print(const char* message)
print:
    push ebp
    mov ebp, esp
    push dword[ebp+8]
    mov eax, 1 ; command print
    int 0x80
    add esp, 4

    pop ebp
    ret

; int getkey()
getkey:
    push ebp
    mov ebp, esp
    mov eax, 2 ; command getkey
    int 0x80
    pop ebp
    ret

; void* peachos_malloc(size_t size)
peachos_malloc:
    push ebp
    mov ebp, esp
    mov eax, 4 ; command malloc (allocates memroy for the process)
    push dword[ebp+8] ; variable size
    int 0x80
    add esp, 4
    pop ebp
    ret

; void peachos_free(void* ptr)
peachos_free:
    push ebp
    mov ebp, esp
    mov eax, 5 ; command 5 frees the allocatedd memory of the process
    push dword[ebp+8] ; variable ptr
    int 0x80
    add esp, 4
    pop ebp
    ret