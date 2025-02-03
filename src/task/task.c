/*
    this is source file for the task holding registers
    and information
*/

#include "task.h"

/* 
    the current task that is running
*/
struct task* current_task = 0;

/* 
    task linked list 
*/
struct task* task_tail = 0;
struct task* task_head = 0;

int task_init(struct task* task, struct process* process);

struct task* task_current(){
    return current_task;
};

struct task* task_new(struct process* process){
    int res = 0;
    struct task* task = kzalloc(sizeof(struct task));
    if(!task){
        res = -ENOMEM;
        goto out;
    }

    res = task_init(task, process);
    if(res != PEACHOS_ALL_OK){
        goto out;
    }

    if(task_head == 0){
        task_head = task;
        task_tail = task;
        goto out;
    }

    task_tail->next = task;
    task->prev = task_tail;
    task_tail = task;

out:
    if(ISERR(res)){
        task_free(task);
        return ERROR(res);
    }

    return task;
};

/*
    get the next task from the linked list
*/
struct task* task_get_next(){
    if(!current_task->next){
        return task_head;
    }

    return current_task->next;
};

/*
    removing the task from the linked list
*/
static void task_list_remove(struct task* task){
    if(task->prev){
        task->prev->next = task->next;
    }

    if(task == task_head){
        task_head == task->next;
    }

    if(task == task_tail){
        task_tail = task->prev;
    }

    if(task == current_task){
        current_task = task_get_next();
    }
}

/*
    freeing the task
*/
int task_free(struct task* task){
    paging_free_4gb(task->page_directory);
    task_list_remove(task);

    /* finally free the task data */
    kfree(task);

    return 0;
}

/*
    switching the context of the tasks for task scheduling and task switching
*/
int task_switch(struct task* task){
    current_task = task;
    paging_switch(task->page_directory->directory_entry);
    return 0;
}

/*
    takes page from kernel directory and loads into the task/page directory

    as during the interrupts, the kernel will called and when we switch back
    we will look into task directory/space so when it return they see the 
    memory as it was before  
*/
int task_page(){
    user_registers();
    task_switch(current_task);
    return 0;
}

/*
    to run the very first task in the system
*/
void task_run_first_ever_task(){
    if(!current_task){
        panic("task_run_first_ever_task(): No curretn task exists!");
    }

    task_switch(task_head);
    task_return(&task_head->registers);
}

/*
    to initialize the task with the regesters
*/
int task_init(struct task* task, struct process* process){
    memset(task, 0, sizeof(struct task));

    /* mape the entire 4GB address space to itself */
    task->page_directory = paging_new_4gb(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    if(!task->page_directory){
        return -EIO;
    }

    /* for the first time program counter will point to the virtual address of process */
    task->registers.ip = PEACHOS_PROGRAM_VIRTUAL_ADDRESS;
    task->registers.ss = USER_DATA_SEGMENT;
    task->registers.esp = PEACHOS_PROGRAM_VIRTUAL_STACK_ADDRESS_STATE;

    task->process = process;

    return 0;
}