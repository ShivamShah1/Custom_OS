/*
    for pasing the data from sting procided in th shell
*/
#ifndef PEACHOS_STRING_H
#define PEACHOS_STRING_H

#include <stdbool.h>

int strlen(const char* ptr);
int strncmp(const char* str1, const char* str2, int n);
int strnlen_terminator(const char* str, int max, char terminator);
char tolower(char s1);
int istrncmp(const char* s1, const char* s2, int n);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, int count);
bool isdigit(char c);
int tonumericdigit(char c);
int strnlen(const char* ptr, int max);
char* strtok(char* str, const char* delimiters);

#endif