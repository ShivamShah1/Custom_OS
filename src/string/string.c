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
    comparing the first n number of elements in strings
*/
int strncmp(const char* str1, const char* str2, int n){
    unsigned char u1,u2;

    while(n-- > 0){
        u1 = (unsigned char)*str1++;
        u2 = (unsigned char)*str2++;
        if(u1 != u2){
            return u1-u2;
        }
        if(u1 == '\0'){
            return 0;
        }
    }
    return 0;
}

/*
    finding the length of the word in the string
*/
int strnlen_terminator(const char* str, int max, char terminator){
    int i = 0;
    for(int i=0; i < max; i++){
        if(str[i] == '\0' || str[i] == terminator){
            break;
        }
    }

    return i;
}

/*
    to lower the char
*/
char tolower(char s1){
    if(s1 >= 65 && s1 <= 90){
        s1 += 32;
    }

    return s1;
}

/*
    in fat16 the filename is not case sensitive
    so checking the file name irrespective of case sensitivity
*/
int istrncmp(const char* s1, const char* s2, int n){
    unsigned char u1, u2;
    while(n-- > 0){
        u1 = (const char)*s1++;
        u2 = (const char)*s2++;
        if(u1 != u2 && tolower(u1) != tolower(u2)){
            return u1 - u2;
        }
        if(u1 == 0){
            return 0;
        }
    }

    return 0;
}

/*
    to copy the source data to the destination place 
*/
char* strcpy(char* dest, const char* src){
    char* res = dest;
    while(*src != 0){
        *dest = *src;
        src += 1;
        dest += 1;
    }

    /* to end the string with NULL terminator */
    *dest = 0x00;

    return res;
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