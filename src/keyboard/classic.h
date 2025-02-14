/*
    header file for classic keyboard PS2
*/
#ifndef CLASSIC_KEYBOARD_H
#define CLASSIC_KEYBOARD_H

/* 0x64 is command reg, 0xAE enables the PS 2 port or driver */
#define PS2_PORT 0x64
#define PS2_COMMAND_ENABLE_FIRST_PORT 0xAE
 
struct keyboard* classic_init();

#endif