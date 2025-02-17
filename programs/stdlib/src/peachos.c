/*
    more OS functions
*/

#include "peachos.h"

/*
    get the key and then block
*/
int peachos_getkeyblock(){
    int val = 0;
    do{
        val = peachos_getkey();
    }
    while(val == 0);
    return val;
}

/*
    this will print only when you press enter like filling password
*/
void peachos_terminal_readline(char* out, int max, bool output_while_typing){
    int i = 0;
    for(i = 0; i < max-1; i++){
        char key = peachos_getkeyblock();

        /* carriage return means we have read the line */
        if(key == 13){
            break;
        }

        if(output_while_typing){
            peachos_putchar(key);
        }

        /* backspace */
        if(key == 0x08 && i >= 1){
            out[i-1] = 0x00;
            /* -2 because we will +1 when we go to the continue */
            i -= 2;
            continue;
        }
        out[i] = key;
    }

    /* add the null terminator at the end */
    out[i] = 0x00;
}