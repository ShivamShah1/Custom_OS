/*
    This will represent PS2 keyboard 
*/

#include "classic.h"
#include "keyboard.h"
#include "io/io.h"
#include <stdint.h>
#include <stddef.h>

int classic_keyboard_init();

/*
    creating an array of scan set codes using PS2 port driver
    originaly used in computers
*/
static uint8_t keyboard_scan_set_one[] = {
    0x00, 0x1B, '1', '2', '3', '4', '5',
    '6', '7', '8', '9', '0', '-', '=',
    0x08, '\t', 'Q', 'W', 'E', 'R', 'T',
    'Y', 'U', 'I', 'O', 'P', '[', ']',
    0x0d, 0x00, 'A', 'S', 'D', 'F', 'G',
    'H', 'J', 'K', 'L', ';', '\'', '`',
    0x00, '\\', 'Z', 'X', 'C', 'V', 'B',
    'N', 'M', ',', '.', '/', 0x00, '*',
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, '7', '8', '9', '-', '4', '5',
    '6', '+', '1', '2', '3', '0', '.'
};


struct keyboard classic_keyboard = {
    .name = {"Classic"},
    .init = classic_keyboard_init
};

/*
    initializing classical keyboard
*/
int classic_keyboard_init(){
    
    outb(PS2_PORT, PS2_COMMAND_ENABLE_FIRST_PORT);

    return 0;
}

/*
    going into the scan code array and pulling out the correct value
*/
uint8_t classic_keyboard_scancode_to_char(uint8_t scancode){
    size_t size_of_keyboard_set_one = sizeof(keyboard_scan_set_one) / sizeof(uint8_t);
    if(scancode > size_of_keyboard_set_one){
        return 0;
    }
    
    char c = keyboard_scan_set_one[scancode];
    return c;
}

/*
    interrupt generated from keyboard for input to register
*/
void classic_keyboard_handle_interrupt(){

}

/*
    initializing the separate virtual keyboard buffer for all processes 
*/
struct keyboard* classic_init(){
    return &classic_keyboard;
}