/*
    GDT - Global Descriptive Table
    This is a header file which helps to change our 
    kernel segments and data descriptors to be written in C 
*/

#ifndef GDT_H
#define GDT_H

#include <stdint.h>
#include "kernel.h"

/*
    this is the stucture mentioned in boot.asm
*/
struct gdt{
    uint16_t segment;
    uint16_t base_first;
    uint8_t base;
    uint8_t access;
    uint8_t high_flags;
    uint8_t base_24_31_bits;
} __attribute__((packed));

/*
    create another sturcture which will use the gdt struct
    to create a proper gdt segment everytime we use so that 
    we dont need to make and initialize the gdt.
*/
struct gdt_structured{
    uint32_t base;
    uint32_t limit;
    uint8_t type;
};

void gdt_load(struct gdt* gdt, int size);
void gdt_structred_to_gdt(struct gdt* gdt, struct gdt_structured* structured_gdt, int total_entries);
#endif