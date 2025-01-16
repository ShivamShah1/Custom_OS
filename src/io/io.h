/*
    this is a header file for the input/output processes done by the system
    like to get keyboard input. 

    Intel does not provide IO support with C lang so we will use asm  
*/
#ifndef IO_H
#define IO_H

unsigned char insb(unsigned short port);
unsigned short insw(unsigned short port);

void outb(unsigned short port, unsigned char val);
void outw(unsigned short port, unsigned short val);

#endif