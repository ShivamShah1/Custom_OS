/*
    first user program in C
*/

#include "peachos.h"
#include "stdlib.h"

/*
    normal user program
*/
int main(int argc, char** argv){

    print("Hello how are you?\n");

    void* ptr = malloc(512);
    free(ptr);

    while(1){
        if(getkey() != 0){
            print("Key was pressed.\n");
        }
    }

    return 0;
}