/*
    This is to read a single or specific length of chars
    instead of reading whole sector of the disk.
*/
#ifndef DISKSTREAMER_H
#define DISKSTREAMER_H

#include "disk.h"

struct disk_stream{
    /* to hold the pos of the cursor in the disk to read */
    int pos;
    struct disk* disk;
};

struct disk_streamer* diskstreamer_new(int disk_id);
int diskstreamer_seek(struct disk_stream* stream, int pos);
int diskstreamer_read(struct disk_stream* stream, void* out, int total);
void diskstream_close(struct disk_stream* stream);

#endif