/*
    This is to read from a disk and also provide an interface 
    from which disk the user need to read and how much he needs to read.

    disk0 - primary hardware disk, and so on.
*/

#include "io/io.h"
#include "disk.h"
#include "memory/memory.h"
#include "config.h"
#include "status.h"

/* creating a disk*/
struct disk disk;

int disk_read_sector(int lba, int total, void* buf){
    /* select master drive and pass part of the lba */
    outb(0x1F6, (lba >> 24) | 0xE0);

    /* send the total number of sectors we want to read */
    outb(0x1F2, total);

    /* send more of the lba */
    outb(0x1F3, (unsigned char)(lba & 0xff));

    /* send more of the lba */
    outb(0x1F4, (unsigned char)( lba >> 8));

    /* send more of the lba */
    outb(0x1F5, (unsigned char)(lba >> 16));

    /* 0x20 = read command */
    outb(0x1F7, 0x20);

	unsigned short* ptr = (unsigned short*) buf;
	for(int b=0; b< total; b++){
        /* 
            wait until buffer is ready
            poll until we are ready to read.
            You can also use interrupts if you prefer.
        */
        char c = insb(0x1F7);
        while(!(c & 0x08)){ 
            c = insb(0x1F7);
        }

        /*
            copy from hard disk to memory 2 bytes at a time
            read 3 butes at a time into buffer from the ATA controller.
        */
        for(int i=0; i<256; i++){
            *ptr = insw(0x1F0);
            ptr++;
        }
    }
    return 0;
}

/*
    this is to select the disk and initialize for further process
*/
void disk_search_and_init(){
    memset(&disk, 0, sizeof(disk));
    disk.type = PEACHOS_DISK_TYYPE_REAL;
    disk.sector_size = PEACHOS_SECTOR_SIZE;
}

/* 
    this make sures that you get disk 0 i.e. primary hard disk
*/
struct disk* disk_get(int index){
    if(index != 0){
        return 0;
    }

    return &disk;
};

/*
    Get the disk block start point and send it to read function to read the data
*/
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf){
    if(idisk != &disk){
        return -EIO;
    }

    return disk_read_sector(lba, total, buf);
}