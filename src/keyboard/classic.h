/*
    header file for classic keyboard PS2
*/
#ifndef CLASSIC_KEYBOARD_H
#define CLASSIC_KEYBOARD_H

/* 0x64 is command reg, 0xAE enables the PS 2 port or driver */
#define PS2_PORT 0x64
#define PS2_COMMAND_ENABLE_FIRST_PORT 0xAE

#define CLASSIC_KEYBOARD_KEY_RELEASE 0x80
#define ISR_KEYBOARD_INTERRUPT 0x21
#define KEYBOARD_INPUT_PORT 0x60

struct keyboard* classic_init();

#endif