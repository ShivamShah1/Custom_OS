#include "file.h"
#include "config.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "status.h"
#include "kernel.h"

struct filesystem* filesystems[PEACHOS_MAX_FILESYSTEM];
struct file_descriptor* file_descriptors[PEACHOS_MAX_FILE_DESCRIPTOR];

/*
    provide psoition to store the pointer of file 
    with max PEACHOS_MAX_FILESYSTEM=12 capacity.
*/
static struct filesystem** fs_get_free_filesystem(){
    int i = 0;
    for(int i = 0; i < PEACHOS_MAX_FILESYSTEM; i++){
        if(filesystems[i] == 0){
            return &filesystems[i];
        }
    }

    /* no empty file location found */
    return 0;
}

/*
    insert the file in filesystem
*/
void fs_insert_filesystem(struct filesystem* filesystem){
    struct filesystem** fs;
    fs = fs_get_free_filesystem();
    if(!fs){
        print("problem inserting the filesystem");
        /* stuck in the infine loop so the system does not do go in panic state */
        while(1) {}
    }

    *fs = filesystem;
}

/*
    inserts the filesystem
*/
static void fs_static_load(){
    //fs_insert_filesystem(fat16_init());
}

/*
    load the file with the memory
*/
void fs_load(){
    memset(filesystems, 0, sizeof(filesystems));
    fs_static_load();
}

/*
    initializes the filesystem with memory
*/
void fs_init(){
    memset(file_descriptors, 0, sizeof(file_descriptors));
}

/*
    create new file descriptors

    so if creation of file descriptor goes well it will return 0;
    else it will return -ENOMEM
*/
static int file_new_descriptor(struct file_descriptor** desc_out){
    int res = -ENOMEM;
    for(int i = 0; i < PEACHOS_MAX_FILE_DESCRIPTOR; i++){
        /* look for a file descriptor space */
        /* if 0 then create file descriptor for that file and return */
        if(file_descriptors[i] == 0){
            struct file_descriptor* desc = kzalloc(sizeof(struct file_descriptor));
            /* descriptor start at 1 */
            desc->index = i+1;
            file_descriptors[i] = desc;
            *desc_out = desc;
            res = 0;
            break;
        }

        return res;
    }
}

static struct file_descriptor* file_get_dessriptor(int fd){
    /* if return fd id invalid number then return 0 */
    if(fd<=0 || fd >= PEACHOS_MAX_FILE_DESCRIPTOR){
        return 0;
    }

    /* descriptors start at 1 */
    int index = fd - 1;
    return file_descriptors[index];
}

struct filesystem* fs_resolve(struct disk* disk){
    struct filesystem* fs = 0;
    for(int i = 0; i < PEACHOS_MAX_FILESYSTEM; i++){
        if(filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0){
            fs = filesystems[i];
            break;
        }
    }

    return fs;
}

int fopen(const char* filename, const char* mode){
    return -EIO;
}