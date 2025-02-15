/*
    this is to read a particular part of a file instead of whole sector.
*/
#include "streamer.h"
#include "memory/heap/kheap.h"
#include "config.h"

#include <stdbool.h>

/*
    Struct to read the part of the file along with maintaining the seek pointer.
*/
struct disk_streamer* diskstreamer_new(int disk_id){
    struct disk* disk = disk_get(disk_id);
    if(!disk){
        return 0;
    }

    struct disk_stream* streamer = kzalloc(sizeof(struct disk_stream));
    streamer->pos = 0;
    streamer->disk = disk;
    return streamer;
};

/*
    assigning the seek pos to read
*/
int diskstreamer_seek(struct disk_stream* stream, int pos){
    stream->pos = pos;
    return 0;
}

/*
    To read the part 1 byte at a time. 
    So using recursion to read the specified length
*/
int diskstreamer_read(struct disk_stream* stream, void* out, int total){
    int sector = stream->pos / PEACHOS_SECTOR_SIZE;
    int offset = stream->pos % PEACHOS_SECTOR_SIZE;

    int total_to_read = total;
    bool overflow = (offset + total_to_read) >= PEACHOS_SECTOR_SIZE;
    char buf[PEACHOS_SECTOR_SIZE];

    if(overflow){
        total_to_read -= (offset + total_to_read) - PEACHOS_SECTOR_SIZE;
    }

    int res = disk_read_block(stream->disk, sector, 1, buf);
    if(res < 0){
        goto out;
    }

    for(int i = 0; i < total_to_read; i++){
        *(char*)out++ = buf[offset+i];
    }

    /* Adjust the stream 
       Also make sure that it does not fall for stack overflow
    */
    stream->pos += total_to_read;
    if(overflow){
        res = diskstreamer_read(stream, out, total - total_to_read);
    }

out:
    return res;
}

/*
    To free the aloocated space
*/
void diskstream_close(struct disk_stream* stream){
    kfree(stream);
}