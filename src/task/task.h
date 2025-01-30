/*
    this is header file for the task holding registers
    and information
*/
#ifndef TASK_H
#define TASK_H

#include "config.h"
#include "kernel.h"
#include "status.h"
#include "memory/paging/paging.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "process.h"
#include <stdint.h>

/*
    here we are creating structures for register as we 
    need to move and store registers as moving from kernel 
    to user space and so on.
*/
struct registers{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    /* program counter which stores the last address which was ran */
    /*by computer so it will help the system to run from next correct location */
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
};

struct task{
    /* page directory of the task */
    struct paging_4gb_chunk* page_directory;

    /* the registers of the task when the task is not running */
    struct registers registers;

    /* the process of the task */
    struct process* process;

    /* the next task in the linked list */
    struct task* next;

    /* previous task in the linked list */
    struct task* prev;
};

struct task* task_new(struct process* process);
struct task* task_current();
struct task* task_get_next();
int task_free(struct task* task);

#endif