/*
    This is header file for processes which will be created and 
    handled properly by our OS.
*/

#include "process.h"

/*
    the current process that is running
*/
struct process* current_process = 0;

static struct process* processes[PEACHOS_MAX_PROCESSES] = {};

/*
    initialize the process by allocating the required memory
*/
static void process_init(struct process* process){
    memset(process, 0, sizeof(struct process));
}

struct process* process_current(){
    return current_process;
};

/*
    this will check if the process id to find the process is valid or not
*/
struct process* process_get(int process_id){
    if(process_id<0 || process_id>=PEACHOS_MAX_PROCESSES){
        return NULL;
    }

    return processes[process_id];
}

/*
    process switching
*/
int process_switch(struct process* process){
    current_process = process;
    return 0;
}

/*
    loading the binaries to the data to support the ELF file
*/
static int process_load_binary(const char* filename, struct process* process){
    int res = 0;
    int fd = fopen(filename, "r");
    if(!fd){
        res = -EIO;
        goto out;
    }

    struct file_stat stat;
    res = fstat(fd, &stat);
    if(res != PEACHOS_ALL_OK){
        goto out;
    }

    void* program_data_ptr = kzalloc(stat.filesize);
    if(!program_data_ptr){
        res = -ENOMEM;
        goto out;
    }

    if(fread(program_data_ptr, stat.filesize, 1, fd) != 1){
        res = -EIO;
        goto out;
    }

    process->filetype = PROCESS_FILETYPE_BINARY;
    process->ptr = program_data_ptr;
    process->size = stat.filesize;

out:
    fclose(fd);
    return res;
}

/*
    process the load data to elf file
*/
static int process_load_elf(const char* filename, struct process* process){
    int res = 0;
    struct elf_file* elf_file = 0;
    res = elf_load(filename, &elf_file);
    if(ISERR(res)){
        goto out;
    }
    
    process->filetype = PROCESS_FILETYPE_ELF;
    process->elf_file = elf_file;

out:
    return res;
}

/*
    loading the data to the process 
*/
static int process_load_data(const char* filename, struct process* process){
    int res = 0;
    /* incase we support ELF files */
    res = process_load_elf(filename, process);
    if(res == -EINFORMAT){
        res = process_load_binary(filename, process);
    }
    return res;
}

/*
    to convert the process mapped memory into binary for ELF files
*/
int process_map_binary(struct process* process){
    int res = 0;
    paging_map_to(process->task->page_directory, (void*)PEACHOS_PROGRAM_VIRTUAL_ADDRESS, process->ptr, paging_align_address(process->ptr + process->size), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
    return res;
}

/*
    mapping the process of elf filetype
*/
static int process_map_elf(struct process* process){
    int res = 0;

    struct elf_file* elf_file = process->elf_file;
    res = paging_map_to(process->task->page_directory, paging_align_to_lower_page(elf_virtual_base(elf_file)), elf_phys_base(elf_file), paging_align_address(elf_phys_end(elf_file)), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
    
    return res;
}

/*
    this functions assuems that the process is initialized correctly
    otherwise calling this function before that will create kernel panic
*/
int process_map_memory(struct process* process){
    int res = 0;

    switch(process->filetype){
        case PROCESS_FILETYPE_ELF:
            res = process_map_elf(process);
        break;

        case PROCESS_FILETYPE_BINARY:
            res = process_map_binary(process);
        break;

        default:
            panic("Process_map_memory: invalid filetype\n");
    }
    
    if(res < 0){
        goto out;
    }

    /* mapping the stack pointer */
    paging_map_to(process->task->page_directory, (void*)PEACHOS_PROGRAM_VIRTUAL_STACK_ADDRESS_END, process->stack, paging_align_address(process->stack+PEACHOS_USER_PROGRAM_STACK_SIZE), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
out: 
    return res;
}

/*
    find the free slot in the process array to load and perform tasks  
*/
int process_get_free_slot(){
    for(int i=0; i<PEACHOS_MAX_PROCESSES; i++){
        if(processes[i] == 0){
            return i;
        }
    }

    return -EISTKN;
}

/*
    load the process
*/
int process_load(const char* filename, struct process** process){
    int res = 0;
    int process_slot = process_get_free_slot();
    if(process_slot < 0){
        res = -EISTKN;
        goto out;
    }

    res = process_load_for_slot(filename, process, process_slot);

out:
    return res;
}

/*
    to load the process and switch old with new process
*/
int process_load_switch(const char* filename, struct process** process){
    int res = process_load(filename, process);
    if(res == 0){
        process_switch(*process);
    }

    return res;
}

/*
    this will help to load the process in the processs arrary define on the top
    and will also provide id to the process, so that if we want to find a 
    process we just look at the array
*/
int process_load_for_slot(const char* filename, struct process** process, int process_slot){
    int res = 0;
    struct task* task = 0;
    struct process* _process;
    void* program_stack_ptr = 0;
    if(process_get(process_slot) != 0){
        res = -EISTKN;
        goto out;
    }

    _process = kzalloc(sizeof(struct process));
    if(!_process){
        res = -ENOMEM;
        goto out;
    }

    process_init(_process);
    res = process_load_data(filename, _process);
    if(res<0){
        goto out;
    }

    program_stack_ptr = kzalloc(PEACHOS_USER_PROGRAM_STACK_SIZE);
    if(!program_stack_ptr){
        res = -ENOMEM;
        goto out;
    }

    strncpy(_process->filename, filename, sizeof(_process->filename));
    _process->stack = program_stack_ptr;
    _process->id = process_slot;

    /* create a task */
    task = task_new(_process);
    if(ERROR_I(task == 0)){
        res = ERROR_I(task);
        goto out;
    }

    _process->task = task;

    res = process_map_memory(_process);
    if(res<0){
        goto out;
    }

    *process = _process;

    /* add the process to the array */
    processes[process_slot] = _process;

out:
    if(ISERR(res)){
        if(_process && _process->task){
            task_free(_process->task);
        }

        kfree(_process);
    }
    return res;
}