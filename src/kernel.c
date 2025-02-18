#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "fs/pparser.h"
#include "string/string.h"
#include "disk/streamer.h"
#include "fs/file.h"
#include "config.h"
#include "gdt/gdt.h"
#include "task/tss.h"
#include "task/task.h"
#include "task/process.h"
#include "status.h"
#include "isr80h/isr80h.h"
#include "keyboard/keyboard.h"

uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

/*
    this can be found in kernel.asm to call the interrupt
*/
extern void problem();

uint16_t terminal_make_char(char c, char colour){
    return (colour << 8) | c;
}

/*
    Creating our own stlen function to Find the length of a string.

size_t strlen(const char* str){
    size_t len =0;
    while(str[len]){
        len++;
    }
    return len;
}
*/

/*
    Placing the char value at the specified location in the screen.
*/
void terminal_putchar(int x, int y, char c, char colour){
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c,colour);
}

/*
    for using backspace in writing  
*/
void terminal_backspace(){
    if(terminal_row == 0 && terminal_col == 0){
        return;
    }

    if(terminal_col == 0){
        terminal_row -= 1;
        terminal_col = VGA_WIDTH;
    }

    terminal_col -= 1;
    terminal_writechar(' ', 15);
    terminal_col -= 1;
}

/*
    To write on the terminal with incrementing position like we normally write.
    Here we write from the start of the terminal and goes increasing first cols and then rows.
*/
void terminal_writechar(char c, char colour){
    if(c == '\n'){
        terminal_col = 0;
        terminal_row += 1;
        return;
    }

    if(c == 0x08){
        terminal_backspace();
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, colour);
    terminal_col += 1;
    if(terminal_col >= VGA_WIDTH){
        terminal_col = 0;
        terminal_row +=1;
    }
}

/*
    We are goint to blank the screen. So during the bois mode we can see different stuff
    so we are going to blank it and then write out word.
*/
void terminal_initialize(){
    video_mem = (uint16_t*)(0xB8000);
    terminal_row = 0;
    terminal_col = 0;
    for(int y=0; y<VGA_HEIGHT; y++){
        for(int x=0; x<VGA_WIDTH; x++){
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

/*
    We created a custom print function to print the given string on the terminal.
*/
void print(const char* str){
    size_t len = strlen(str);
    for(int i=0; i<len; i++){
        terminal_writechar(str[i], 15);
    }
}

/*
    Here are proving static to this struct so that it is only available to this process
*/
static struct paging_4gb_chunk* kernel_chunk = 0;

/*
    incase of kernel panic or error, it will display the symbol
*/
void panic(const char* msg){
    print(msg);
    /* to stop the kernel to do something else and hold it here */
    while(1){}
}

/*
    Will switch page directory from kernel page directory
    and also change from registers from kernel registers
*/
void kernel_page(){
    kernel_registers();
    paging_switch(kernel_chunk);
}

/*
    to declare the user and kernel segments
*/
struct tss tss;
struct gdt gdt_real[PEACHOS_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[PEACHOS_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},                    // NULL segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x9a},              // kernel code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x92},              // kernel data segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf8},              // user code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf2},              // user data segment
    {.base = (uint32_t)&tss, .limit = sizeof(tss), .type = 0xE9}    // tss segment
};
/*
    used for timer callback interrupt

void pic_timer_callback(){
    print("timer activated\n");
}
*/

void kernel_main(){
    /*
        Everytime its not good to provide it with hex values.

        So we create a function 'terminal_make_colour' to convert it. 
    */
    terminal_initialize();
    print("Hello world!\ntest");

    /*
        allocating memory to GDT and structuring it
    */
    memset(gdt_real, 0x00, sizeof(gdt_real));
    gdt_structred_to_gdt(gdt_real, gdt_structured, PEACHOS_TOTAL_GDT_SEGMENTS);

    /*
        load the gdt
    */
    gdt_load(gdt_real, sizeof(gdt_real));

    /*
        Here now we are goining to implement heap allocation
    */
    kheap_init();

    /*
        after memory allocation we initializes filesystem
    */
    fs_init();
    
    /*
        Search and initialilze the disk
    */
    disk_search_and_init();

    /*
        initialize the interrupt descriptor table 
    */
    idt_init();

    /*
        implementing tss for task switching between user and kernel

        here every time process is hand overed the kernel it will 
        set its pointer to 0x600000, this is the kernel stack location
        and when it is returned to user space the location will be reset to 
        0x00.
    */
    memset(&tss, 0x00, sizeof(tss));
    tss.esp0 = 0x600000;    // kernel stack location
    tss.ss0 = KERNEL_DATA_SELECTOR;

    /* load the tss */
    tss_load(0x28); // this is the offset of gdt_real 

    /*
        Providing 4096 bytes of chunk to a single page and
        having page table size of 1024
    */
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    /*
        In-charge of switching pages as in paging there is no contigous memory location.
        So, if the memory is contigous then there is no meaning of paging.

        So, to get the data properly as the location of the data is scatter all the 
        place, to retrive the data properly we use page switching method. 
        
        paging_4gb_chunk_get_directory is for getting location of page.
    */
    paging_switch(kernel_chunk);

    /*
        enabling the paging
    */
    enable_paging;

    /*
        registering kernel commands
    */
    isr80h_register_commands();

    /* 
        initialize the virtual keyboard
    */
    keyboard_init();
    
    /*
        registering timer with 0x20 interrupt
        as first interrupt is timer, and next is keyboard
        so 0x21 link with keyboard
        
        so when we link it with time, it will continously does the
        pic_timer_callback fuction as timer interrupt will occur 
        all the time. 
    
    idt_register_interrupt_callback(0x20, pic_timer_callback);
    */
    
    /*
        providing the process
    */
    struct process* process = 0;
    int res = process_load_switch("0:/shell.elf", &process);
    if(res != PEACHOS_ALL_OK){
        panic("Failed to load shell.elf\n");
    }

    struct command_argument argument;
    strcpy(argument.argument, "testing");
    argument.next = 0x00;

    process_inject_arguments(process, &argument);

    task_run_first_ever_task();

    while(1) {}
}