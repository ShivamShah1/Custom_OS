#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "fs/pparser.h"
#include "string/string.h"
#include "disk/streamer.h"
#include "fs/file.h"

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

/*
    Here are proving static to this struct so that it is only available to this process
*/
static struct paging_4gb_chunk* kernel_chunk = 0;

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
        Providing 4096 bytes of chunk to a single page and
        having page table size of 1024
    */
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    /*
        To directly read from the location into buffer
    
    char buf[512];
    disk_read_block(disk_get(0), 20, 4, &buf);
    */

    /*
        In-charge of switching pages as in paging there is no contigous memory location.
        So, if the memory is contigous then there is no meaning of paging.

        So, to get the data properly as the location of the data is scatter all the 
        place, to retrive the data properly we use page switching method. 
        
        paging_4gb_chunk_get_directory is for getting location of page.
    */
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    /*
        mapping the memory
    
    char *ptr = kzalloc(4096);
    */

    /*
        setting up for paging, so here in virtual addr 0x1000 should map to the ptr addr. 
        So if we use another ptr pointing to 0x1000, the data will be same as even though
        physical address is different as it is mapped to 0x1000 on virtual address.
    
    paging_set(paging_4gb_chunk_get_directory(kernel_chunk), (void*)0x1000, (uint32_t)ptr | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
    */

    /*
        testing the above mapping mechanism in working as we have a ptr pointing to 0x1000 virtual address
        and the above ptr is pointing to 0x1000 virtual address having different physical address.

        Here we have not yet implemented the enable_pagin(), so it is not enabled. =
        So by changing the values at ptr2, we will not change the data at different physical address. 

        Output- ptr2 -AB and ptr - empty.
    
    char* ptr2 = (char*) 0x1000;
    ptr2[0] = 'A';
    ptr2[1] = 'B';
    print(ptr2);
    print(ptr);
    */

    /*
        enabling the paging
    */
    enable_paging;

    /*
        sometimes setting paging, it disables or triggers unneccessay interrupts
    */
    enable_interrupts;

    /*
        using path parser to move from one part to another
    
    struct path_root* root_path = pathparser_parse("0:/bin/shell.exe", NULL);
    if(root_path){

    }
    */
    
    /*
        Assigning disk 0 to read, then selecting single byte to read which is
        0x201 and storing that location data to c.
    
    struct disk_stream* stream = diskstreamer_new(0);
    diskstreamer_seek(stream, 0x201);
    unsigned char c = 0;
    diskstreamer_read(stream, &c, 1);
        to hold to see the data in qemu
    while(1){}
    */
    
    /*
        to check if the strcpy works properly
    
    char buf[20];
    strcpy(buf, "hello!");
    */

    /*
        testing the fat16 implementation using file descriptor using fopen, fread and fseek.

        first fd is created and from that, it will look for a file name hello.txt
        if it finds then loads its directory and items.
        Once loaded, we can now read and write into them in streams also.
    
    int fd = fopen("0:/hello.txt", "r");
    if(fd){
        print("\n we opend hello.txt file \n");
        char buf[14];
        fseek(fd, 2, SEEK_SET);
        // now reading from pos 2 instead of 0
        fread(buf, 11, 1, fd);
        // NULL terminator 
        buf[13] = 0x00;
        print(buf);
    }
    */

    /*
        testing filesystem using file programs
    */
    int fd = fopen("0:/hello.txt", "r");
    print("\n file opened\n");
    if(fd){
        struct file_stat s;
        fstat(fd, &s);
        print("\n status of file\n");
        fclose(fd);
        print("\n file closed\n");
    }

    /*
        reading into the buffer
        Place the init code for disk at the start of the kernel so that it init the disk
        at the start so that it is easy to map and access the location through an interface.

        We can directly use the below without disk init, but a proper interface wont be create to regualrlly
        and randomly accessing the data using API. We will need to know the exact location in the 
        physical memory for paging and maping.
 
    char buf[512];
    disk_read_sector(0, 1, buf);
    */

    /*
        testing the above mapping mechanism in working as we have a ptr pointing to 0x1000 virtual address
        and the above ptr is pointing to 0x1000 virtual address having different physical address.

        Here we have enabled paging  
        So by changing the values at ptr2, we will change the data at different physical address. 

        Output - ptr2 - AB and ptr - AB.
    
    char* ptr2 = (char*) 0x1000;
    ptr2[0] = 'A';
    ptr2[1] = 'B';
    print(ptr2);
    print(ptr);
    */

    void* ptr = kmalloc(50);
    void* ptr2 = kmalloc(5000);
    void* ptr3 = kmalloc(5600);
    kfree(ptr);
    void* ptr4 = kmalloc(50);
    /* here we are using this to see if we have aloocated memory or not 
    if( ptr || ptr2 || ptr3 || ptr4){

    }
    */
    kfree(ptr2);
    kfree(ptr3);
    kfree(ptr4);
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
}