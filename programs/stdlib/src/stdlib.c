/*
    creating basic functions
*/
#include "stdlib.h"
#include "peachos.h"

/*
    allocating heap memory
*/
void* malloc(size_t size){
    return peachos_malloc(size);
}

/*
    free allocated memory
*/
void free(void* ptr){
    peachos_free(ptr);
}