/*
    This is a source file which helps to create a FAT16 readable file
*/
#include "file.h"
#include "config.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "status.h"
#include "kernel.h"
#include "fat/fat16.h"
#include "disk/disk.h"
#include "string/string.h"

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
    fs_insert_filesystem(fat16_init());
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
    freeing the file descriptor after accessing fclose
*/
static void file_free_descriptor(struct file_descriptor* desc){
    file_descriptors[desc->index-1] = 0x00;
    kfree(desc);
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

/*
    structure of file descriptor which is used to perform tasks on file
*/
static struct file_descriptor* file_get_descriptor(int fd){
    /* if return fd id invalid number then return 0 */
    if(fd<=0 || fd >= PEACHOS_MAX_FILE_DESCRIPTOR){
        return 0;
    }

    /* descriptors start at 1 */
    int index = fd - 1;
    return file_descriptors[index];
}

/*
    providing the file descriptor from the filesystem
*/
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

/*
    providing mode to the file
*/
FILE_MODE file_get_mode_by_string(const char* str){
    FILE_MODE mode = FILE_MODE_INVALID;
    if(strncmp(str, "r", 1) == 0){
        /* here we are converting the mode into binary so that we do not */
        /* need to check what does this mode means */
        mode = FILE_MODE_READ;
    }
    else if(strncmp(str, "w", 1) == 0){
        mode = FILE_MODE_WRITE;
    }
    if(strncmp(str, "a", 1) == 0){
        mode = FILE_MODE_APPEND;
    }
    return mode;
}

/*
    opening a file with 
*/
int fopen(const char* filename, const char* mode_str){
    int res = 0;
    struct path_root* root_path = pathparser_parse(filename, NULL);
    if(!root_path){
        res = - EINVARG;
        goto out;
    }

    /* we cannot have just a root path 0:/0:/test.txt */
    if(!root_path->first){
        res =-EINVARG;
        goto out;
    }

    /* ensure the disk we are reading from exists */
    struct disk* disk = disk_get(root_path->driver_no);
    if(!disk){
        res = -EIO;
        goto out;
    }
    
    if(!disk->filesystem){
        res = -EIO;
        goto out;
    }

    FILE_MODE mode = file_get_mode_by_string(mode_str);
    if(mode == FILE_MODE_INVALID){
        res = -EINVARG;
        goto out;
    }

    void* descriptor_private_data = disk->filesystem->open(disk, root_path->first, mode);
    if(ISERR(descriptor_private_data)){
        res = ERROR_I(descriptor_private_data);
        goto out;
    }

    struct file_descriptor* desc = 0;
    res = file_new_descriptor(&disk);
    if(res < 0){
        goto out;
    }
    desc->filesystem = disk->filesystem;
    desc->private = descriptor_private_data;
    desc->disk = disk;
    res = desc->index;

out:
    /* fopen should not return negative values */
    if(res < 0){
        res = 0;
    }
    return res;
}

/*
    creating a fread function to read from the file
*/
int fread(void* ptr, uint32_t size, uint32_t nmemb, int fd){
    int res = 0;
    if(size == 0 || nmemb == 0 || fd < 1){
        res = -EINVARG;
        goto out;
    }

    struct file_descriptor* desc = file_get_descriptor(fd);
    if(!desc){
        res = -EINVARG;
        goto out;
    }

    res = desc->filesystem->read(desc->disk, desc->private, size, nmemb, (char*)ptr);

out:
    return res;
}

/*
    getting the stats of the file 
*/
int fstat(int fd, struct file_stat* stat){
    int res = 0;
    struct file_descriptor* desc = file_get_descriptor(fd);
    if(!fd){
        res = -EIO;
        goto out;
    }

    res = desc->filesystem->stat(desc->disk, desc->private, stat);

out:
    return res;
}

/*
    closing the opened file
*/
int fclose(int fd){
    int res = 0;
    struct file_descriptor* desc = file_get_descriptor(fd);
    if(!desc){
        res = -EIO;
        goto out;
    }

    res = desc->filesystem->close(desc->private);

out:
    return res;
}

/*
    creating a fseek function to place the pointer to any position
*/
int fseek(int fd, int offset, FILE_SEEK_MODE whence){
    int res = 0;
    struct file_descriptor* desc = file_get_descriptor(fd);
    if(!desc){
        res = -EIO;
        goto out;
    }

    res = desc->filesystem->seek(desc->private, offset, whence);

    /* free the file descriptor memory */
    if(res == PEACHOS_ALL_OK){
        file_free_descriptor(desc);
    }

out:
    return res;
}