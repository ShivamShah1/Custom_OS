/*
    The config file to config the system
*/

#ifndef CONFIG_H
#define CONFIG_H

#define KERNEL_CODE_SELECTOR 0X08
#define KERNEL_DATA_SELECTOR 0X10

#define PEACHOS_TOTAL_INTERRUPTS 512

#define PEACHOS_HEAP_SIZE_BYTES 104857600 //100MB
#define PEACHOS_HEAP_BLOCK_SIZE 4096
#define PEACHOS_HEAP_ADDRESS 0x01000000
#define PEACHOS_HEAP_TABLE_ADDRESS 0x00007E00

#define PEACHOS_SECTOR_SIZE 512
#define PEACHOS_MAX_FILESYSTEM 12
#define PEACHOS_MAX_FILE_DESCRIPTOR 512
#define PEACHOS_MAX_PATH 108

#define PEACHOS_TOTAL_GDT_SEGMENTS 6

#define PEACHOS_PROGRAM_VIRTUAL_ADDRESS 0x400000 
#define PEACHOS_USER_PROGRAM_STACK_SIZE 1024*16
#define PEACHOS_PROGRAM_VIRTUAL_STACK_ADDRESS_STATE 0x3FF000
#define PEACHOS_PROGRAM_VIRTUAL_STACK_ADDRESS_END PEACHOS_PROGRAM_VIRTUAL_STACK_ADDRESS_STATE - PEACHOS_USER_PROGRAM_STACK_SIZE

#define USER_DATA_SEGMENT 0x23
#define USER_CODE_SEGMEnt 0x1b

#define PEACHOS_MAX_PROGRAM_ALLOCATIONS 1024
#define PEACHOS_MAX_PROCESSES 12

#define PEACHOS_MAX_ISR80H_COMMANDS 1024

#define PEACHOS_KEYBOARD_BUFFER_SIZE 1024

#endif