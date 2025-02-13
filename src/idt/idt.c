/*
    this file is for creating or handling the interrupt using interrupt descriptor table
*/

#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
#include "io/io.h"
#include "task/task.h"

struct idt_desc idt_descriptors[PEACHOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

static ISR80H_COMMAND isr80h_commands[PEACHOS_MAX_ISR80H_COMMANDS];

/*
    this is called in idt.asm file
*/  
extern void idt_load(struct idtr_desc* ptr);
extern void int21h();
extern void no_interrupt();
extern void isr80h_wrapper();

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
    the interrupts will be initialized which can be used later
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
    idt_set(0x80, isr80h_wrapper);

    /*
        load the interrupt descriptor table
    */ 
    idt_load(&idtr_descriptor);
}

/*
    to make sure that the interrupt is valid
*/
void isr80h_register_command(int command_id, ISR80H_COMMAND command){
    if(command_id < 0 || command_id >= PEACHOS_MAX_ISR80H_COMMANDS){
        panic("The command is out of box\n");
    }

    if(isr80h_commands[command_id]){
        panic("You are attempting to overwrite an existing command\n");
    }

    isr80h_commands[command_id] = command;
}

/*
    to handle the interrupt
*/
void* isr80h_handler_command(int command, struct interrupt_frame* frame){
    void* result = 0;
    if(command < 0 || command >= PEACHOS_MAX_ISR80H_COMMANDS){
        return 0;
    }

    ISR80H_COMMAND command_func = isr80h_commands[command];
    if(!command_func){
        return 0;
    }

    result = command_func(frame);
    return result;
}

/*
    Interrupt handler when called will pass the command to kernel
    to print the message from userspace
*/
void* isr80h_handler(int command, struct interrupt_frame* frame){
    void* res = 0;
    kernel_page();
    /* for multi-tasking/threading pusrposes by saving old task state */
    task_current_save_status(frame);
    res = isr80h_handler_command(command, frame);
    task_page();
    return res;
}