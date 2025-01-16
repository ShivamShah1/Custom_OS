/*
    First we need to specify the origin so that Os knows the offset needed for the data
    Ideally the origin should be 0x0000 and then the system should jump to the 
    designated location in the case of this system is 0x7c00.

    Then we need to specify the how much is our bit system. Here it is 16 bit system.  

    Here int is interrupt and 0x10 is the BIOS interrupt location. So we are calling BIOS routine.

    To find which interrupt is at which location we can use Ralf Brown's Interrupt list.
    We can see that 
        int 10/ah=0eh - is a video teletypr output
            means display a char on the screen, advancing the ccursor and scrolling the screen as necessary.

    Here we are using jump so that it does not go to boot signature as I am continously displaying 'A' on screen

    Here we are using times cmd as we atleast need to fill 510 bytes of data. Here if the given instructions
    are not enough to fill 510 bytes then it will pad it with 0's.

    Now then providing the boot signature which is 0x55AA, but we are writing here 0xAA55 due to endianess.
*/

/*
    Use Makefile1 for build
    
    Now to run this in the system using qemu we will first create a binary file '.bin'.
    For this we will use this cmd - 'nasm -f bin ./boot.asm -o ./boot.bin'.
        (giving ./boot.bin as the bin file name will be boot.bin)

    We can see a binary file having 512 bytes of data.

    To see the asm file data in regs use this cmd in linux terminal
       cmd - 'ndisasm ./boot.bin'

    Now to run on qemu use this cmd
        cmd - 'qemu-system-x86_64 -hda ./boot.bin'
*/

/*
ORG 0x7c00
BITS 16

start:
    mov ah, 0eh
    mov al, 'A'
    mov bx, 0
    int 0x10

    jmp $

times 510-($ - $$) db 0
dw 0xAA55
*/


/*
    Use Makefile1 for build
    
    To print custom message on the booting screen we will use the below code.

ORG 0x7c00
BITS 16

start:
    mov si, message
    call print
    jmp $

print:
    mov bx, 0
.loop:
    lodsb
    cmp al, 0
    je .done
    call print_char
    jump .loop
.done:
    ret

print_char:
    mov ah, 0eh
    int 0x10
    ret

message: db 'Hello World!', 0

times 510-($ - $$) db 0
dw 0xAA55

*/


/*
    Use Makefile1 for build
    
    Ideally we place our start point 0x0000 and we jump from that to the desired location.
    As sometimes different system understand the code differently as placeing org as 0x7c00
    and then out desire location code is at the same location as we need the processor
    to go at that location so instead of this the system will understand to add these both location
    like 0x7c00 + 0x7c00 and the result will not be proper and will make the system to jump or look
    at undesired location.

    So, always we make ORG as 0x0000 and then we jump it to the desired location. 


ORG 0
BITS 16
_start:
    jmp short start
    nop

 times 33 db0

start:
    jmp 0x7c0:step2

step2:
    cli ; Clear Interrupts
    moc ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov ss, ax
    mov sp, 0x7c00
    sti ; Enables Interrupts

    mov si, message
    call print
    jmp $

print:
    mov bx, 0
.loop:
    lodsb
    cmp al, 0
    je .done
    call print_char
    jmp .loop
.done:
    ret

print_char:
    mov ah, 0eh
    int 0x10
    ret

message: db 'Hello World!', 0

times 510-($ - $$) db 0
dw 0xAA55
*/


/*
    Use Makefile1 for build
    
    This section we will write and call custom interrupts for Real Mode. 


ORG 0
BITS 16
_start:
    jmp short start
    nop

 times 33 db0

start:
    jmp 0x7c0:step2

handle_zero:
    mov ah, 0eh
    mov al, 'A'
    mov bx, 0x00
    int 0x10
    iret

handle_one:
    mov ah, 0eh
    mov al, 'V'
    mov bx, 0x00
    int 0x10
    iret

step2:
    cli ; Clear Interrupts
    moc ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov ss, ax
    mov sp, 0x7c00
    sti ; Enables Interrupts

    ;giving space to interrupt 0
    mov word[ss:0x00], handle_zero
    mov word[ss:0x02], 0x7c0

    ; used to call interrupt 0
    mov ax, 0x00
    div ax  

    ;giving space to interrupt 1
    mov word[ss:0x00], handle_one
    mov word[ss:0x02], 0x7c0

    ;using this to call interrupt 1
    int 1

    mov si, message
    call print
    jmp $

print:
    mov bx, 0
.loop:
    lodsb
    cmp al, 0
    je .done
    call print_char
    jmp .loop
.done:
    ret

print_char:
    mov ah, 0eh
    int 0x10
    ret

message: db 'Hello World!', 0

times 510-($ - $$) db 0
dw 0xAA55
*/



/*
    Use Makefile1 for build

    Removing the unnecessary things from the code like interrupts and able to read the message.txt
    file to display during the boot.


ORG 0
BITS 16
_start:
    jmp short start
    nop

 times 33 db0

start:
    jmp 0x7c0:step2

step2:
    cli ; Clear Interrupts
    moc ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov ss, ax
    mov sp, 0x7c00
    sti ; Enables Interrupts

    ; loading sector into memory to read
    mov ah, 2 ; read sector command
    mov al, 1 ; one sector to read
    mov ch, 0 ; cylinder low eight BITS
    mov cl, 2 ; read sector two
    mov dh, 0 ; head number
    mov bx, buffer
    int 0x13 ; calling interrupt to read the had drive for display
    jc error

    mov si, buffer
    call print

    jmp $

error:
    mov si, error_message
    call print
    jmp $

print:
    mov bx, 0
.loop:
    lodsb
    cmp al, 0
    je .done
    call print_char
    jmp .loop
.done:
    ret

print_char:
    mov ah, 0eh
    int 0x10
    ret

error_message: db 'Failed to load sector', 0

times 510-($ - $$) db 0
dw 0xAA55

buffer:
*/


/*
    For here use Makefile2 and earlier use Makefile1

    To start the processor in the protected mode after the real mode.
    In the protected mode now we cannot read from the message.txt as we did in the real mode.

    To read the message.txt we will need disk driver to read and if we try to read using biffer 
    like earlieer time bad things will happen.

    To use gdb, first install using this cmd - 'sudo apt install gdb'.
    Then use this cmd to run the prog with gdb debugging diretly write 'gdb' then enter.

    As you enter, you wiil enter in gdb mode and there you will write in gdb terminal is
        'target remote | qemu-system-x86_64 -hda ./boot.bin -S -gdb stdio'
        then type 'c' to continue and you can see the prog successfull.
        In the terminal the hold(ctrl +n+n+c).
        then type layout asm to visualize the code of assmebly.
        type 'info registers' to get info of regs


ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start:
    jmp short start
    nop

 times 33 db0

start:
    jmp 0:step2

step2:
    cli ; Clear Interrupts
    moc ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti ; Enables Interrupts

.load_protected:
    cli
    lgdt[gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:load32
    
; GDT - Global Descriptive Table for protection mode
gdt_start:
gdt_null:
    dd 0x0
    dd 0x0

; offset 0x8
gdt_code:       ; CS should point to this
    dw 0xffff   ; segment limit first 0-15 BITS
    dw 0        ; base first 0-15 BITS
    db 0        ; base 16-23 BITS
    db 0x9a     ; access bytes
    db 11001111b; high 4 bit flags and low 4 bit flags
    db 0        ; base 24-31 BITS

; offset 0x10
gdt_data:       ; DS, SS, ES, FS, GS
    dw 0xffff   ; segment limit first 0-15 bits
    dw 0        ; base first 0-15 bits
    db 0        ; base 16-23 BITS
    db 0x92     ; access bytes
    db 11001111b; high 4 bit flags and low 4 bit flags
    db 0        ; base 24-31 BITS

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start-1
    dd gdt_start

[BITS 32]
load32:
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

    jmp $

times 510-($ - $$) db 0
dw 0xAA55
*/


/*
    From here use Makefile and earlier use Makefile1

    Now we are creating another file called kernel.asm so we are using this
    to load only bootloader and then hand it over to kernel. 

    So this file will only read first section of mem and the reat of the 
    things will be done by kernel and so we are moving 32 read to kernel.asm
*/
ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start:
    jmp short start
    nop

 times 33 db0

start:
    jmp 0:step2

step2:
    cli ; Clear Interrupts
    moc ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti ; Enables Interrupts

.load_protected:
    cli
    lgdt[gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:load32
    
    
; GDT - Global Descriptive Table for protection mode
gdt_start:
gdt_null:
    dd 0x0
    dd 0x0

; offset 0x8
gdt_code:       ; CS should point to this
    dw 0xffff   ; segment limit first 0-15 BITS
    dw 0        ; base first 0-15 BITS
    db 0        ; base 16-23 BITS
    db 0x9a     ; access bytes
    db 11001111b; high 4 bit flags and low 4 bit flags
    db 0        ; base 24-31 BITS

; offset 0x10
gdt_data:       ; DS, SS, ES, FS, GS
    dw 0xffff   ; segment limit first 0-15 bits
    dw 0        ; base first 0-15 bits
    db 0        ; base 16-23 BITS
    db 0x92     ; access bytes
    db 11001111b; high 4 bit flags and low 4 bit flags
    db 0        ; base 24-31 BITS

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start-1
    dd gdt_start

 [BITS 32]
 load32:
    mov eax, 1
    mov ecx, 100
    mov edi, 0x0100000
    call ata_lba_read
    jmp CODE_SEG:0x0100000

ata_lba_read:
    mov ebx, eax, ; backup the LBA (Loagical Block Addressing)
    ; send the highest 8 bits of the lba to hard disk controller
    shr eax, 24
    or eax, 0xE0 ; select the master drive
    mov dx, 0x1F6
    out dx, al
    ; finished sending the highest 8 bits of the lba

    ; send the total sectors to read
    mov eax, ecx
    mov dx, 0x1F2
    out dx, al
    ; finished sending the total sectors to read

    ;send more bits of the LBA
    mov eax,ebx ; restore the backup LBA
    mov dx, 0x1F3
    out dx, al
    ; finished sending more bits of the LBA

    ; send more bits of the LBA
    mov dx, 0x1F4
    mov eax, ebx ; restored the backup LBA
    shr eax, 8
    out dx, al
    ; finished sending more bits of the LBA
    
    ; send upper 16 bits of the LBA
    mov dx, 0x1F5
    mov eax, ebx ; restored the backup LBA
    shr eax, 16
    out dx, al
    ; finished sending upper 16 bits of the LBA

    mov dx, 0x1f7
    mov al, 0x20
    out dx, al

; reading all sectors into memory
.next_sector:
    push ecx

; checking if we need to read
.try_again:
    mov dx, 0x1f7
    in al, dx
    test al, 8
    jz .try_again

; we need to read 256 words at a time
    mov ecx, 256
    mov dx, 0x1F0
    rep insw
    pop ecx
    loop .next_sector
    ; end of reading sectors into mem
    ret

times 510-($ - $$) db 0
dw 0xAA55