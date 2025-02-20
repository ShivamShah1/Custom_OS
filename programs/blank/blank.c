/*
    first user program in C
*/

#include "peachos.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

/*
    normal user program
*/
int main(int argc, char** argv){
    /* continously runts the task */
    /* kernel.c has 2 task, so runs both by switching using timer interrupts*/
    /* like scheduler */
    while(1){
        print(argv[0]);
    }

    return 0;
}