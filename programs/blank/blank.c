/*
    first user program in C
*/

#include "peachos.h"
#include "stdlib.h"
#include "stdio.h"

/*
    normal user program
*/
int main(int argc, char** argv){

    print("Hello how are you?\n");

    print(itoa(87));

    putchar('Z');

    printf("my age is %i\n", 25);
    
    void* ptr = malloc(512);
    free(ptr);

    char buf[1024];
    peachos_terminal_readline(buf, sizeof(buf), true);
    print(buf);
    
    while(1){
        
    }

    return 0;
}