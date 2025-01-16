#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"

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
*/
size_t strlen(const char* str){
    size_t len =0;
    while(str[len]){
        len++;
    }
    return len;
}

/*
    Placing the char value at the specified location in the screen.
*/
void terminal_putchar(int x, int y, char c, char colour){
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c,colour);
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

void kernel_main(){
    /*
        here we will see a letter 'A' in blue colour at the booting process with other data
        on the screen.

        To run this run ./build.sh in cmd list and then go to bin dir and then run the
        'qemu-system-x86_64 -hda ./os.bin' 

    char* video_mem = (char*)(0xB8000);
    video_mem[0] = 'A';
    video_mem[1] = 1;
    video_mem[2] = 'B';
    video_mem[3] = 2;
    */

    /*
        Here we know this uses 2 bytes so we use uint_16 for more precision.
        
        For this we need to change the values which we provides like
        we know A=0x41 and 1=0x01, so we will provide as '0x4101'.

        Here we need to provide 0x0141 because of edianess.
    
    uint16_t* video_mem = (uint16_t*)(0xB8000);
    video_mem[0] = 0x0141;
    */

    /*
        Everytime its not good to provide it with hex values.

        So we create a function 'terminal_make_colour' to convert it. 
    */
    terminal_initialize();
    print("Hello world!\ntest");

    /*
        initialize the interrupt descriptor table 
    */
    idt_init();

    /*
        disable interrupts so the system does not get interrupted while doing this 
    */
    disable_interrupts();

    /*
        generating an interrupt as we are dividing it number by zero 
    */
    problem();

    /*
        enabling interrupts so the system does get interrupted for another interrupts 
    */
    enable_interrupts();

    /*
        using io to provide output this char
        The below code after outb is to do io using interrupts
    
    outb(0x60, 0xff);
    */

    /*
        the above one is to directly call the i/o
        This is used to handle the i/o using interrupts

        We are using keyboard to call this interrupt.
        
        this way we are only taking interrupt from keyboard only once
        so we need will change it that is takes interrupt everytime.  

        If we change the interrupt from 0x21 to 0x20, as 20 is for timer
        then we will get continous interrupt on the screen for keyboard.
    */


    /*
        Here now we are goining to implement heap allocation
    */
    kheap_init();

    void* ptr = kmalloc(50);
    void* ptr2 = kmalloc(5000);
    void* ptr3 = kmalloc(5600);
    kfree(ptr);
    void* ptr4 = kmalloc(50);
    /* here we are using this to see if we have aloocated memory or not */
    if( ptr || ptr2 || ptr3 || ptr4){

    }
}