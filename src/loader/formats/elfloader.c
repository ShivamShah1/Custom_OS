/*
    this is the file which helps to load the elf file to the OS 
*/

#include "elfloader.h"
#include "fs/file.h"
#include "status.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "string/string.h"
#include "memory/paging/paging.h"
#include "kernel.h"
#include "config.h"

#include <stdbool.h>

/*
    creating elf signature to identify that the program is for this file
*/
const char elf_signature[] = {0x7f, 'E', 'L', 'F'};

/*
    checking a match for signature, 
    so that it only runs if the signature matches
*/
static bool elf_valid_signature(void* buffer){
    return memcmp(buffer, (void*)elf_signature, sizeof(elf_signature)) == 0;
}

/*
    making sure that the elf class is valid
*/
static bool elf_valid_class(struct elf_header* header){
    /* we only support 32 bit binaries */
    return header->e_ident[EI_CLASS] == ELFCLASSNONE || header->e_ident[EI_CLASS] == ELFCLASS32;
}

/*
    making sure that the encoding for the elf is proper
*/
static bool elf_valid_encoding(struct elf_header* header){
    return header->e_ident[EI_DATA] == ELFDATANONE || header->e_ident[EI_DATA] == ELFDATA2LSB;
}

/*
    making sure that the elf file is with executable permission
*/
static bool elf_is_executable(struct elf_header* header){
    /* making sure that the address is in the range of valid virtual memory */
    /* ie. >0x40000000 as we dont want someone accessing lower region */
    return header->e_type == ET_EXEC && header->e_entry >= PEACHOS_PROGRAM_VIRTUAL_ADDRESS;
}

/*
    making sure that the elf file has program header
*/
static bool elf_has_program_header(struct elf_header* header){
    return header->e_phoff != 0;
}

/*
    returning the pointer to elf file memory address
*/
void* elf_memory(struct elf_file* file){
    return file->elf_memory;
}

/*
    return pointer to the elf file header memory
*/
struct elf_header* elf_header(struct elf_file* file){
    return file->elf_memory;
}

/*
    returning the pointer to the section header of elf file
*/
struct elf32_shdr* elf_sheader(struct elf_header* header){
    return (struct elf32_shdr*)((int)header + header->e_shoff);
}

/*
    returning the pointer to the program header of elf file
*/
struct elf32_phdr* elf_pheader(struct elf_header* header){
    if(header->e_phoff == 0){
        return 0;
    }

    return (struct elf32_pphdr*)((int)header + header->e_phoff);
}

/*
    returning the individual pointer to the program header of elf file
*/
struct elf32_phdr* elf_program_header(struct elf_header* header, int index){
    return &elf_pheader(header)[index];    
}

/*
    returning the individual pointer to the section header of elf file
*/
struct elf32_shdr* elf_section(struct elf_header* header, int index){
    return &elf_sheader(header)[index];
}

/*
    provides pointer to the elf program header physical header
*/
void* elf_phdr_phys_address(struct elf_file* file, struct elf32_phdr* phdr){
    return elf_memory(file) + phdr->p_offset;
}

/*
    returning the pointer to the string table of elf file
*/
char* elf_str_table(struct elf_header* header){
    return (char*)header + elf_section(header, header->e_shstrndx)->sh_offset;
}

/*
    returning the pointer to the virtual base address of elf file
*/
void* elf_virtual_base(struct elf_file* file){
    return file->virtual_base_address;
}

/*
    returning the pointer to the virtual end address of elf file
*/
void* elf_virtual_end(struct elf_file* file){
    return file->virtual_end_address;
}

/*
    returning the pointer to the physical base address of elf file
*/
void* elf_phys_base(struct elf_file* file){
    return file->physical_base_address;
}

/*
    returning the pointer to the physical end address of elf file
*/
void* elf_phys_end(struct elf_file* file){
    return file->physical_end_address;
}

/*
    validate that we can load this elf file
*/
int elf_validate_loaded(struct elf_header* header){
    return (elf_valid_signature(header) && elf_valid_class(header) && elf_valid_encoding(header) && elf_has_program_header(header) && elf_is_executable(header)) ? (PEACHOS_ALL_OK) : (-EINFORMAT);
}

/*
    process the program header pointer to load
*/
int elf_process_phdr_pt_load(struct elf_file* elf_file, struct elf32_phdr* phdr){
    if(elf_file->virtual_base_address >= (void*)phdr->p_vaddr || elf_file->virtual_base_address == 0x00){
        elf_file->virtual_base_address = (void*)phdr->p_vaddr;
        elf_file->physical_base_address = elf_memory(elf_file) + phdr->p_offset;
    }

    unsigned int end_virtual_address = phdr->p_vaddr + phdr->p_filesz;
    if(elf_file->virtual_end_address >= (void*)end_virtual_address || elf_file->virtual_end_address == 0x00){
        elf_file->virtual_end_address = (void*)end_virtual_address;
        elf_file->physical_end_address = elf_memory(elf_file) + phdr->p_offset + phdr->p_filesz;
    }
    return 0;
}

/*
    process the elf header individual
*/
int elf_process_pheader(struct elf_file* elf_file, struct elf32_phdr* phdr){
    int res = 0;
    switch(phdr->p_type){
        case PT_LOAD:
            res = elf_process_phdr_pt_load(elf_file, phdr);
        break;
    }

    return res;
}

/*
    process the elf header file
*/
int elf_process_pheaders(struct elf_file* elf_file){
    int res = 0;
    struct elf_header* header = elf_header(elf_file);

    for(int i= 0; i < header->e_phnum; i++){
        struct elf32_phdr* phdr = elf_program_header(header, i);
        res = elf_process_pheader(elf_file, phdr);
        if(res < 0){
            break;
        }
    }

    return res;
}

/*
    process the loading of elf file
*/
int elf_process_loaded(struct elf_file* elf_file){
    int res = 0;
    struct elf_header* header = elf_header(elf_file);
    
    res = elf_validate_loaded(header);
    if(res < 0){
        goto out;
    }

    res = elf_process_pheaders(elf_file);
    if(res < 0){
        goto out;
    }

out:
    return res;
}

/*
    to free the memory from of the elf file
*/
void elf_file_free(struct elf_file* elf_file){
    if(elf_file->elf_memory){
        kfree(elf_file->elf_memory);
    }

    kfree(elf_file);
}

/*
    allocating the memory so that it always gets free by elf_file_free
*/
struct elf_file* elf_file_new(){
    return (struct elf_file*)kzalloc(sizeof(struct elf_file));
}

/*
    to load the elf file
*/
int elf_load(const char* filename, struct elf_file** file_out){
    struct elf_file* elf_file = elf_file_new();
    
    int fd = 0;
    int res = fopen(filename, "r");
    if(res <= 0){
        res = -EIO;
        goto out;
    }

    fd = res;

    struct file_stat stat;
    res = fstat(fd, &stat);
    if(res < 0){
        goto out;
    }

    elf_file->elf_memory = kzalloc(stat.filesize);

    res = fread(elf_file->elf_memory, stat.filesize, 1 , fd);
    if(res < 0){
        goto out;
    }

    res = elf_process_loaded(elf_file);
    if(res < 0){
        goto out;
    }
    
    *file_out = elf_file;

out:
    if(res < 0){
        elf_file_free(elf_file);
    }
    fclose(fd);
    return res;
}

/*
    closing the elf file and clearing the allocated memory
*/
void elf_close(struct elf_file* file){
    if(!file){
        return;
    }

    kfree(file->elf_memory);
    kfree(file);
}