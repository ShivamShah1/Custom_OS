#include "string.h"

/*
    To find the len of the string
*/
int strlen(const char* ptr){
    int i = 0;
    while(*ptr != 0){
        i++;
        ptr += 1;
    }

    return i;
}

/*
    to check if the char passed is a digit no not
*/
bool isdigit(char c){
    return c >= 48 && c <= 57;
}

/*
    To convert the char into respective int value
*/
int tonumericdigit(char c){
    return c - 48;
}

/*
    To get the len of the string till the length of string 
    or max len specified.
*/
int strnlen(const char* ptr, int max){
    int i = 0;
    for(i = 0; i<max; i++){
        if(ptr[i] == 0){
            break;
        }
    }

    return i;
}