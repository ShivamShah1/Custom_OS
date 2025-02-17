/*
    this is file which will help to create a static ELF file for our OS
*/

#include "elf.h"

/*
    to set a pointer to the address of the entry section of the program
*/
void* elf_get_entry_ptr(struct elf_header* elf_header){
    return (void*)elf_header->e_entry;
}

/*
    to get the address location of entry
*/
uint32_t elf_get_entry(struct elf_header* elf_header){
    return elf_header->e_entry;
}