/*
    This is header file for processes which will be created and 
    handled properly by our OS.
*/
#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "config.h"
#include "memory/memory.h"
#include "status.h"
#include "task/task.h"
#include "memory/heap/kheap.h"
#include "fs/file.h"
#include "string/string.h"
#include "kernel.h"
#include "memory/paging/paging.h"
#include "loader/formats/elfloader.h"

#include <stdbool.h>

#define PROCESS_FILETYPE_ELF 0
#define PROCESS_FILETYPE_BINARY 1

typedef unsigned char PROCESS_FILETYPE;

struct process_allocation{
    void* ptr;
    size_t size;
};

/*
    This is the structure of all the processes which will be created in this system
*/
struct process{
    /* the process id */
    uint16_t id;

    char filename[PEACHOS_MAX_PATH];

    /* themain process task */
    struct task* task;

    /* this is to track the memory allocated by the kernel */
    /* so we can free them at the termination of program */
    struct process_allocation allocations[PEACHOS_MAX_PROGRAM_ALLOCATIONS];

    PROCESS_FILETYPE filetype;
    union
    {
        /* the physical pointer to the process memory */
        void* ptr;
        struct elf_file* elf_file;
    };
    
    /* the physical pointer to the stack memory */
    void* stack;

    /* the size of the data pointed to by ptr */
    uint32_t size;

    /* providing keyboard buffer required and possed by every process individually */
    struct keyboard_buffer{
        char buffer[PEACHOS_KEYBOARD_BUFFER_SIZE];
        int tail;
        int head;
    } keyboard;
};

int process_switch(struct process* process);
int process_load_switch(const char* filename, struct process** process);
int process_load(const char* filename, struct process** process);
int process_load_for_slot(const char* filename, struct process** process, int process_slot);
struct process* process_current();
struct process* process_get(int process_id);
static int process_find_free_allocation_index(struct process* process);
void process_free(struct process* process, void* ptr);

#endif