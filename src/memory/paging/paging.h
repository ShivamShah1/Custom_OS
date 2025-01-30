/*
    This is the header file for the paging in the system

    Benefits to paging: -
        to map the memory to a video buffer, so when we task switch we
        still write to the memory as buffer is mapped to the physical
        address
    
    We cannot map hardware addresses to mapping as everytime, the system
    use to interact with hardware, it needs to go through CPU kernel.

    So, in vedio memory, the CPU will look for mapped memory and
    then send it to the hardware memory or SD card.
*/

#ifndef PAGING_H
#define PAGING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define PAGING_CACHE_DISABLED   0b00010000
#define PAGING_WRITE_THROUGH    0b00001000
#define PAGING_ACCESS_FROM_ALL  0b00000100
#define PAGING_IS_WRITEABLE     0b00000010
#define PAGING_IS_PRESENT       0b00000001

#define PAGING_TOTAL_ENTRIES_PER_TABLE  1024
#define PAGING_PAGE_SIZE 4096

void paging_switch(uint32_t* directory);
struct paging_4gb_chunk* paging_new_4gb(uint8_t flags);
uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk);
void enable_paging();
int paging_set(uint32_t* directory, void* virt, uint32_t val);
int paging_get_indexes(void* virtual_address, uint32_t* directory_index_out, uint32_t* table_index_out);
bool pagin_is_aligned(void* addr);
void paging_free_4gb(struct paging_4gb_chunk* chunk);
int paging_map_range(uint32_t* directory, void* virt, void* phys, int count, int flags);
int paging_map(uint32_t* directory, void* virt, void* phys, int flags);
int paging_map_to(uint32_t* directory, void* virt, void* phys, void* phys_end, int flags);
void* paging_is_alligned_address(void* ptr);

struct paging_4gb_chunk{
    uint32_t* directory_entry;
};


#endif