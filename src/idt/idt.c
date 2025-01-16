/*
    this file is for creating or handling the interrupt using interrupt descriptor table
*/

#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
#include "io/io.h"

struct idt_desc idt_descriptors[PEACHOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

/*
    this is called in idt.asm file
*/  
extern void idt_load(struct idtr_desc* ptr);
extern void int21h();
extern void no_interrupt();

/*
    IO interrupt to recognize the keyboard interrupt
    here the keyboard interrupt is IRQ 1 and the IRQ
    address start from the location 0x20.

    So, 21 is the location of int so we writing int21 
*/
void int21h_handler(){
    print("Keyboard pressed!\n");
    outb(0x20, 0x20);
}

/*
    this is no interrupt handler so that is someother interrupt occur
    it does not get struct in the bios mode
*/
void no_interrupt_handler(){
    outb(0x20, 0x20);
}

/*
    this is the handler to handle the interrupt for zero division
*/ 
void idt_zero(){
    print("Divide by zero error\n");
}

/*
    providing the memery space for the interrupt in the memorry file.
*/
void idt_set(int interrupt_no, void* address){
    struct idt_desc* desc = &idt_descriptors[interrupt_no];
    desc->offset_1 = (uint32_t)address&0x0000ffff;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = 0xEE;
    desc->offset_2 = (uint32_t)address>>16;
}

/*
    this is called to generate the interrupt
*/
void idt_init(){
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) -1;
    idtr_descriptor.base = (uint32_t)idt_descriptors;

    for(int i=0; i<PEACHOS_TOTAL_INTERRUPTS; i++){
        idt_set(i, no_interrupt);
    }
    
    idt_set(0, idt_zero);
    idt_set(0x21, int21h);

    /*
        load the interrupt descriptor table
    */ 
    idt_load(&idtr_descriptor);
}