/*
    This is the header file for the struct of elf and to extract info related 
    to memory size, name, physical and virtual address.
*/
#ifndef ELFLOADER_H
#define ELFLOADER_H

#include <stdint.h>
#include <stddef.h>

#include "elf.h"
#include "config.h"

/*
    the struct of the elf loadable file
*/
struct elf_file{
    /* the name of the file */
    char filename[PEACHOS_MAX_PATH];

    /* size of the loadable elf file */
    int in_memory_size;

    /* the physical memory address that this elf file is loaded at */
    void* elf_memory;

    /* the virtual base memory address for this binary */
    /* will point to the first loadable section in virtual memory */
    void* virtual_base_address;

    /* the virtual end memory address for this binary */
    void* virtual_end_address;

    /* the physical base address of this binary */
    /* will point to the first loadable section in physical memory */
    void* physical_base_address;

    /* the physical end address of this binary */
    void* physical_end_address;
};

int elf_load(const char* filename, struct elf_file** file_out);
void elf_close(struct elf_file* file);
void* elf_virtual_base(struct elf_file* file);
void* elf_virtual_end(struct elf_file* file);
void* elf_phys_base(struct elf_file* file);
void* elf_phys_end(struct elf_file* file);

#endif ELFLOADER_H