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
    void* allocations[PEACHOS_MAX_PROGRAM_ALLOCATIONS];

    /* the physical pointer to the process memory */
    void* ptr;

    /* the physical pointer to the stack memory */
    void* stack;

    /* the size of the data pointed to by ptr */
    uint32_t size;
};

int process_load_for_slot(const char* filename, struct process** process, int process_slot);

#endif PROCESS_H