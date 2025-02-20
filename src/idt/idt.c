/*
    this file is for creating or handling the interrupt using interrupt descriptor table
*/

#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
#include "io/io.h"
#include "task/task.h"
#include "status.h"
#include "task/process.h"

struct idt_desc idt_descriptors[PEACHOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

/*
    the idea of creating this table is to have the address of the interrupts
    created in idt.asm code, as we do not have the address of the created
    interrupts. 
    So we will use this table to to map the interrupt generated in .asm file
    by storing that address using this table.
*/
extern void* interrupt_pointer_table[PEACHOS_TOTAL_INTERRUPTS];

static INTERRUPT_CALLBACK_FUNCTION interrupt_callbacks[PEACHOS_TOTAL_INTERRUPTS];
static ISR80H_COMMAND isr80h_commands[PEACHOS_MAX_ISR80H_COMMANDS];

/*
    this is called in idt.asm file
*/  
extern void idt_load(struct idtr_desc* ptr);
extern void int21h();
extern void no_interrupt();
extern void isr80h_wrapper();

/*
    this is no interrupt handler so that is someother interrupt occur
    it does not get struck in the bios mode
*/
void no_interrupt_handler(){
    outb(0x20, 0x20);
}

/*
    to handle the interrupts
*/
void interrupt_handler(int interrupt, struct interrupt_frame* frame){
    kernel_page();
    if(interrupt_callbacks[interrupt] != 0){
        task_current_save_state(frame);
        interrupt_callbacks[interrupt](frame);
    }

    task_page();
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
    to handle execption
*/
void idt_handler_exception(){
    process_terminate(task_current()->process);
    task_next();
}

/*
    to change the task using timer interrupt
*/
void idt_clock(){
    /* to enable interrupt */
    outb(0x20, 0x20);
    /* switch to next task */
    task_next();
} 

/*
    the interrupts will be initialized which can be used later
*/
void idt_init(){
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) -1;
    idtr_descriptor.base = (uint32_t)idt_descriptors;

    /* mapping the generated address to the table */
    for(int i=0; i<PEACHOS_TOTAL_INTERRUPTS; i++){
        idt_set(i, interrupt_pointer_table[i]);
    }
    
    idt_set(0, idt_zero);
    idt_set(0x80, isr80h_wrapper);

    for(int i = 0; i < 0x20; i++){
        idt_register_interrupt_callback(i, idt_handler_exception);
    }

    /* 0x20 is for timer interrupt */
    idt_register_interrupt_callback(0x20, idt_clock);

    /* load the interrupt descriptor table */ 
    idt_load(&idtr_descriptor);
}

/*
    this is to register interrupt callback with the table
*/
int idt_register_interrupt_callback(int interrupt, INTERRUPT_CALLBACK_FUNCTION interrupt_callback){
    if(interrupt < 0 || interrupt >= PEACHOS_TOTAL_INTERRUPTS){
        return -EINVARG;
    }

    interrupt_callbacks[interrupt] = interrupt_callback;
    
    return 0;
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