/*
    Here we are specifing the sections that which section is doing which part
    and so on.

    Here in SECTIONS '. = 1M' means (1024x1024=100000 (in hex))
    So we are telling that after the boot go to this location where the
    kernel.bin is saved and start running from that part.

    Here we are aligning everything properly as we are now coding in C.
    During assmebly we need not wrry for the alignment but for both 
    asm and C we need to align otherwise code not work properly.

    So we added times 512 at the end of kernel.asm and here.
*/

ENTRY(_start)
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 0x400000;
    .text : ALIGN(4096)
    {
        *(.text)
    }

    .asm : ALIGN(4096)
    {
        *(.asm)
    }
    
    .rodata : ALIGN(4096)
    {
        *(.rodata)
    }

    .data : ALIGN(4096)
    {
        *(.data)
    }

    .bss : ALIGN(4096)
    {
        *(COMMON)
        *(.bss)
    }
}