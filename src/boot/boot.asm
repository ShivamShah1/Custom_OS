%%if 0
    In this we are adding FAT16 system to our booting 
    part so that kernel knows how to read the files.
%%endif
ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

jmp short start
nop

; FAT16 Header
OEMIdentifier           db 'PEACHOS ' ; OEM is an 8 bytes identifier so we need to allocate 8 chars. So PEACHOS -7 and space -1 byte.
BytesPerSector          dw 0x200 ; we FAT16 needs 200 bytes per sector
SectorsPerCluster       db 0x80
ReservedSectors         dw 0x200 ; this is where the kernel will be store, not in a file or anywhere. So allocating kernel space/code
FATCopies               db 0x02 ; we are storing 2 copies, 1 for back-up
RootDirEntries          dw 0x40
NumSectors              dw 0x00
MediaType               db 0xF8
SectorsPerFat           dw 0x100
SectorsPerTrack         dw 0x20
NumberOfHeads           dw 0x40
HiddenSectors           dd 0x00
SectorsBig              dd 0x773594

; Extended BPB (Dos 4.0)
DriveNumber             db 0x80
WinNTBit                db 0x00
Signature               db 0x29
VolumeID                dd 0xD105
VolumeIDString          db 'PEACHOS BOO' ; can store only 11 bytes
SystemIDString          db 'FAT16   ' ; need to store 8 bytes, so 3 spaces


start:
    jmp 0:step2

step2:
    cli ; Clear Interrupts
    moc ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov gs, ax
    mov fs, ax
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
    mov es, ax
    mov ds, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    ; enabling A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; for loading...
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