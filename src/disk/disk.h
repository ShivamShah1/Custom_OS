/*
    This is a header file of read from a disk program and also provide an interface 
    from which disk the user need to read and how much he needs to read.

    disk0 - primary hardware disk, and so on.
*/

#ifndef DISK_H
#define DISK_H

#include "fs/file.h"

/* this is a primary hard disk */
#define PEACHOS_DISK_TYYPE_REAL 0

typedef unsigned int PEACHOS_DISK_TYPE;

struct disk{
    PEACHOS_DISK_TYPE type;
    int sector_size;

    /* id of the disk */
    int id;
    
    struct filesystem* filesystem;

    /* the private data of our filesystem */
    void* fs_private;
};

struct disk* disk_get(int index);

/* we are not giving this disk_read_sector to kernel as we are using it as low level extraction. so should not provide this file */
//int disk_read_sector(int lba, int total, void* buf);
void disk_search_and_init();
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf);

#endif