/*
    THis is used for the kernel allocation and freeing
*/

#include "kheap.h"
#include "heap.h"
#include "config.h"
#include "kernel.h"

struct heap kernel_heap;
struct heap_table kernel_Heap_table;

voidkheap_init(){
    int total_table_entries = PEACHOS_HEAP_SIZE_BYTES/PEACHOS_HEAP_BLOCK_SIZE;
    kernel_Heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*)(PEACHOS_HEAP_TABLE_ADDRESS);
    kernel_Heap_table.total = total_table_entries;

    void* end = (void*)(PEACHOS_HEAP_ADDRESS + PEACHOS_HEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void*)(PEACHOS_HEAP_ADDRESS), end, &kernel_Heap_table);

    if(res<0){
        print("Failed to create heap/n");
    }
}

void* kmalloc(size_t size){
    return heap_malloc(&kernel_heap, size);
}

void kfree(void* ptr){
    heap_free(&kernel_heap, ptr);
}