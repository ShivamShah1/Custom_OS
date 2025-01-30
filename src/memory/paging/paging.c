/*
    This is the to creating paging the files into the RAM for 
    hiding processes, easy RAM access, and much more.

    Benefits to paging: -
        to map the memory to a video buffer, so when we task switch we
        still write to the memory as buffer is mapped to the physical
        address
    
    We cannot map hardware addresses to mapping as everytime, the system
    use to interact with hardware, it needs to go through CPU kernel.

    So, in vedio memory, the CPU will look for mapped memory and
    then send it to the hardware memory or SD card.
*/

#include "paging.h"
#include "memory/heap/kheap.h"
#include "status.h"

void paging_load_directory(uint32_t* directory);

/*  Here we are using static word as we do not want to have 
    this curret_directory variable available to other files 
*/
static uint32_t* current_directory =0;

/*
    Providing 4096 bytes of chunk to a single page and
    having page table size of 1024
*/
struct paging_4gb_chunk* paging_new_4gb(uint8_t flags){
    uint32_t* directory = kzalloc(sizeof(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE));
    int offset = 0;
    for(int i = 0; i<PAGING_TOTAL_ENTRIES_PER_TABLE; i++){
        uint32_t* entry = kzalloc(sizeof(uint32_t)* PAGING_TOTAL_ENTRIES_PER_TABLE);
        for(int b = 0; b<PAGING_TOTAL_ENTRIES_PER_TABLE; b++){
            entry[b] = (offset + (b* PAGING_PAGE_SIZE)) | flags;
        }
        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
        directory[i] = (uint32_t)entry | flags | PAGING_IS_WRITEABLE;
    }
    /*
        Providing required space in the RAM
    */
    struct paging_4gb_chunk* chunk_4gb = kzalloc(sizeof(struct paging_4gb_chunk));
    chunk_4gb->directory_entry = directory;
    return chunk_4gb;
}

/*
    In-charge of switching pages as in paging there is no contigous memory location.
    So, if the memory is contigous then there is no meaning of paging.

    So, to get the data properly as the location of the data is scatter all the 
    place, to retrive the data properly we use page switching method. 
*/
void paging_switch(uint32_t* directory){
    paging_load_directory(directory);
    current_directory = directory;
}

/*
    freeing the paging section of 4gb
*/
void paging_free_4gb(struct paging_4gb_chunk* chunk){
    for(int i=0; i<1024; i++){
        uint32_t entry = chunk->directory_entry[i];
        uint32_t* table = (uint32_t*)(entry & 0xfffff000);
        kfree(table);
    }

    kfree(chunk->directory_entry);
    kfree(chunk);
}

/*
    getting the location of the page or chunk in ram or memory
*/
uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk){
    return chunk->directory_entry;
}

bool pagin_is_aligned(void* addr){
    return ((uint32_t)addr % PAGING_PAGE_SIZE) == 0;
}

int paging_get_indexes(void* virtual_address, uint32_t* directory_index_out, uint32_t* table_index_out){
    int res = 0;
    if(!pagin_is_aligned(virtual_address)){
        res = -EINVARG;
        goto out;
    }

    *directory_index_out = ((uint32_t)virtual_address/(PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));

    /* Using this to get accurate table index starting from 0 instead of the real address as per location */
    *table_index_out = ((uint32_t)virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);

out:
    return res;
}

/*
    making sure that virtual and physical pages are alligned properly
*/
void* paging_is_alligned_address(void* ptr){
    if((uint32_t)ptr % PAGING_PAGE_SIZE){
        return (void*)((uint32_t)ptr + PAGING_PAGE_SIZE - ((uint32_t)ptr % PAGING_PAGE_SIZE));
    }

    return ptr;
}

/*
    making sure that virtual and physical pages are mapped properly
*/
int paging_map(uint32_t* directory, void* virt, void* phys, int flags){
    if(((unsigned int)virt % PAGING_PAGE_SIZE) || ((unsigned int)phys % PAGING_PAGE_SIZE)){
        return -EINVARG;
    }

    return paging_set(directory, virt, (uint32_t)phys | flags);
}

/*
    mapping the range of paging of the process
*/
int paging_map_range(uint32_t* directory, void* virt, void* phys, int count, int flags){
    int res = 0;
    for(int i=0; i<count; i++){
        res = paging_map(directory, virt, phys, flags);
        if(res == 0){
            break;
        }
        virt += PAGING_PAGE_SIZE;
        phys += PAGING_PAGE_SIZE;
    }

    return res;
}

/*
    ensruring the paging is mapped to the process
*/
int paging_map_to(uint32_t* directory, void* virt, void* phys, void* phys_end, int flags){
    int res = 0;
    if((uint32_t)virt % PAGING_PAGE_SIZE){
        res = -EINVARG;
        goto out;
    }

    if((uint32_t)phys % PAGING_PAGE_SIZE){
        res = -EINVARG;
        goto out;
    }

    if((uint32_t)phys_end % PAGING_PAGE_SIZE){
        res = -EINVARG;
        goto out;
    }

    if((uint32_t)phys_end < (uint32_t)phys){
        res = -EINVARG;
        goto out;
    }

    uint32_t total_bytes = phys_end - phys;
    int total_pages = total_bytes / PAGING_PAGE_SIZE;
    res = paging_map_range(directory, virt, phys, total_pages, flags);

out:
    return res;
}

/*
    ensures that the paging is set and usable
*/
int paging_set(uint32_t* directory, void* virt, uint32_t val){
    if(!pagin_is_aligned(virt)){
        return -EINVARG;
    }

    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    int res = paging_get_indexes(virt, &directory_index, &table_index);

    if(res < 0){
        return res;
    }

    /* This entry will store the starting address of the directory index */
    uint32_t entry = directory[directory_index];

    /* we want to extract only 31-11 bits as per the page_directory_entry struct */
    uint32_t* table = (uint32_t*)(entry & 0xfffff000);
}