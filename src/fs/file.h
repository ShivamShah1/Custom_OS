#ifndef FILE_H
#define FILE_H

#include "pparser.h"

typedef unsigned int FILE_SEEK_MODE;
enum{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

typedef unsigned int FILE_MODE;
enum{
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

struct disk;
typedef void*(*FS_OPEN_FUNCTION)(struct disk* disk, struct path_part* path, FILE_MODE mode);

/*
    to check if the file system is present or not
    to check if the function/file is valid or not

    So it will check if the FAT16 header is 
    available in the first sector or not
*/
typedef int(*FS_RESOLVE_FUNTION)(struct disk* disk);

struct filesystem{
    /* Filesystem should return zero from resolve if the provided disk is using its filesystem */
    FS_RESOLVE_FUNTION resolve;
    FS_OPEN_FUNCTION open;

    /* allowing the system to name this process with limit of 20 bytes */
    char name[20];
};

struct file_descriptor{
    /* the descriptor index */
    int index;
    struct filesystem* filesystem;

    /* private data for internal file descriptor */
    /* like where is the path, in which mode, etc */
    void* private;

    /* the disk that the file descriptor should be used on */
    struct disk* disk;
};

/*
    to initialize the file system
*/
void fs_init();

/*
    to open the file
*/
int fopen(const char* filename, const char* mode);

/*
    to insert the file system
*/
void fs_insert_filesystem(struct filesystem* filesystem);

struct filesystem* fs_resolve(struct disk* disk);
#endif