/*
    This is a header file which helps to create a FAT16 readable file
*/
#ifndef FILE_H
#define FILE_H

#include "pparser.h"
#include <stdint.h>

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

/*
    this will be set if the file is read only
*/
enum{
    FILE_STAT_READ_ONLY = 0b00000001
};

/*
    getting the status of the file
*/
struct file_stat{
    FILE_STAT_FLAGS flags;
    uint32_t filesize;
};

typedef unsigned int FILE_STAT_FLAGS;

struct disk;
typedef void*(*FS_OPEN_FUNCTION)(struct disk* disk, struct path_part* path, FILE_MODE mode);
typedef int (*FS_READ_FUNCTION)(struct disk* disk, void* private, uint32_t size, uint32_t nmemb, char* out);
typedef int (*FS_SEEK_FUNCTION)(void* private, uint32_t offset, FILE_SEEK_MODE seek_mode);
typedef int (*FS_STAT_FUNCTION)(struct disk* disk, void* private, struct file_stat* stat);
typedef int (*FS_CLOSE_FUNCTION)(void* private);

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
    FS_READ_FUNCTION read;
    FS_SEEK_FUNCTION seek;
    FS_STAT_FUNCTION stat;
    FS_CLOSE_FUNCTION close;

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
int fopen(const char* filename, const char* mode_str);

/*
    to read the file
*/
int fread(void* ptr, uint32_t size, uint32_t nmemb, int fd);

/*
    to seek the position of pointer in a file
*/
int fseek(int fd, int offset, FILE_SEEK_MODE whence);

/*
    to get the stats of the file
*/
int fstat(int fd, struct file_stat* stat);

/*
    to close the file
*/
int fclose(int fd);

/*
    to insert the file system
*/
void fs_insert_filesystem(struct filesystem* filesystem);

struct filesystem* fs_resolve(struct disk* disk);

#endif